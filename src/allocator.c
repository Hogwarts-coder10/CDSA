#include "CDSA/allocator.h"
#include <stddef.h>
#include <stdint.h> // for uintptr_t
#include <stdlib.h>
#include <string.h>

size_t cdsa_global_memory_used = 0;

void *cdsa_malloc_track(size_t size) {
  // Over-allocate to store the block size
  size_t *ptr = malloc(size + sizeof(size_t));
  if (!ptr)
    return NULL;

  *ptr = size;                     // Store size in the hidden header
  cdsa_global_memory_used += size; // Update global telemetry

  return (void *)(ptr + 1); // Give the user the memory right after the header
}

void cdsa_free_track(void *user_ptr) {
  if (!user_ptr)
    return;

  // Step backward to read the hidden size
  size_t *ptr = ((size_t *)user_ptr) - 1;
  cdsa_global_memory_used -= *ptr;

  free(ptr);
}

void *cdsa_calloc_track(size_t count, size_t size) {
  size_t total = count * size;
  void *ptr = cdsa_malloc_track(total);
  if (ptr)
    memset(ptr, 0, total);
  return ptr;
}

void *cdsa_realloc_track(void *user_ptr, size_t new_size) {
  if (!user_ptr)
    return cdsa_malloc_track(new_size);
  if (new_size == 0) {
    cdsa_free_track(user_ptr);
    return NULL;
  }

  size_t *old_ptr = ((size_t *)user_ptr) - 1;
  size_t old_size = *old_ptr;

  size_t *new_ptr = realloc(old_ptr, new_size + sizeof(size_t));
  if (!new_ptr)
    return NULL;

  *new_ptr = new_size;

  // Adjust the global counter by the difference
  cdsa_global_memory_used = cdsa_global_memory_used - old_size + new_size;

  return (void *)(new_ptr + 1);
}

typedef struct kpool_node {
  struct kpool_node *next;
} kpool_node;

struct kpool {
  size_t block_size;
  size_t capacity;
  void *memory_block;
  kpool_node *free_list;
};

kpool *kpool_create(size_t block_size, size_t capacity) {
  if (block_size == 0 || capacity == 0)
    return NULL;

  // Safety padding: Block size must be large enough to hold a pointer
  if (block_size < sizeof(kpool_node)) {
    block_size = sizeof(kpool_node);
  }

  // Standardise alignment to 8 bytes to prevent hardware alignment faults
  if (block_size % 8 != 0) {
    block_size += 8 - (block_size % 8);
  }

  // Using the custom telemetry memory allocator
  kpool *pool = CDSA_MALLOC(sizeof(kpool));
  if (!pool)
    return NULL;

  pool->block_size = block_size;
  pool->capacity = capacity;

  // Allocate massive contiguous memory slab through the telemetry sensor
  pool->memory_block = CDSA_MALLOC(block_size * capacity);
  if (!pool->memory_block) {
    CDSA_FREE(pool->memory_block);
    CDSA_FREE(pool);
    return NULL;
  }

  // Wire up the free list
  pool->free_list = (kpool_node *)pool->memory_block;
  kpool_node *current = pool->free_list;

  // Link every chunk to the next chunk in the slab
  for (size_t i = 0; i < capacity - 1; i++) {
    kpool_node *next_node =
        (kpool_node *)((char *)pool->memory_block + ((i + 1) * block_size));
    current->next = next_node;
    current = next_node;
  }

  current->next = NULL;

  return pool;
}

void *kpool_alloc(kpool *pool) {
  /* if the pool is NULL or the free list is empty,
   * we've ran out of pre-allocated memory
   */

  if (!pool || !pool->free_list) {
    return NULL;
  }

  // POP: Grab the block at the head of the free_list
  kpool_node *node = pool->free_list;

  // Move the free pointer to the next available block
  pool->free_list = node->next;

  // Hand over the memory to the user (cast back to a void pointer)
  return (void *)node;
}

void kpool_free(kpool *pool, void *ptr) {
  if (!pool || !ptr) {
    return;
  }

  // PUSH: Cast the user's returned memory back into a node
  kpool_node *node = (kpool_node *)ptr;

  // Link it to the current head of the free list
  node->next = pool->free_list;

  // Update the free list to point to this newly returned block
  pool->free_list = node;
}

void kpool_destroy(kpool *pool) {
  if (!pool) {
    return;
  }

  // Hand the massive contiguous slab back to Layer 1 (this updates your global
  // telemetry!)
  if (pool->memory_block) {
    CDSA_FREE(pool->memory_block);
  }

  // Free the pool manager struct itself
  CDSA_FREE(pool);
}

// --- cdsa_arena Implementation ---

#include <stdint.h> // For uintptr_t

struct cdsa_arena {
  size_t capacity;
  size_t offset;
  unsigned char *buffer;
};

// Helper macro to align memory addresses (usually to 8 bytes)
#define ALIGN_FORWARD(x, align) (((size_t)(x) + ((align) - 1)) & ~((align) - 1))
cdsa_arena *arena_create(size_t capacity) {
  if (capacity == 0)
    return NULL;

  cdsa_arena *arena = CDSA_MALLOC(sizeof(cdsa_arena));
  if (!arena)
    return NULL;

  arena->buffer = CDSA_MALLOC(capacity);
  if (!arena->buffer) {
    CDSA_FREE(arena);
    return NULL;
  }

  arena->capacity = capacity;
  arena->offset = 0;

  return arena;
}

void *arena_alloc(cdsa_arena *arena, size_t size) {
  if (!arena || size == 0)
    return NULL;

  // Calculate the current unaligned memory address
  uintptr_t current_addr = (uintptr_t)arena->buffer + (uintptr_t)arena->offset;

  // Calculate where the next 8-byte aligned address is
  uintptr_t aligned_addr = ALIGN_FORWARD(current_addr, 8);

  // Figure out how much padding we had to add to reach that alignment
  size_t padding = aligned_addr - current_addr;

  // Check if we have enough space left in the arena (including padding)
  if (arena->offset + padding + size > arena->capacity) {
    return NULL; // Out of memory in this arena
  }

  // Move the offset forward past the padding and the requested size
  arena->offset += padding + size;

  return (void *)aligned_addr;
}

void arena_reset(cdsa_arena *arena) {
  if (arena) {
    // The magic bullet: Instantly clear everything
    arena->offset = 0;
  }
}

void arena_destroy(cdsa_arena *arena) {
  if (arena) {
    if (arena->buffer) {
      CDSA_FREE(arena->buffer);
    }
    CDSA_FREE(arena);
  }
}

#ifndef CDSA_ALLOCATOR_H
#define CDSA_ALLOCATOR_H

#include <stddef.h>

// Global telemetry sensor
extern size_t cdsa_global_memory_used;

void *cdsa_malloc_track(size_t size);
void *cdsa_calloc_track(size_t count, size_t size);
void *cdsa_realloc_track(void *ptr, size_t new_size);
void cdsa_free_track(void *ptr);

// kpool (Global Memory Pool) API
typedef struct kpool kpool;

/*
 * @brief Creates a new memory Pool.
 * @param block_size the size in the bytes of each induvidual bytes.
 * @param capacity The total number of items the pool can hold.
 * @return Pointer to the kpool, or NULL on failure
 */

kpool *kpool_create(size_t block_size, size_t);

/**
 * @brief allocates one block from the pool in O(1) time.
 */

void *kpool_alloc(kpool *pool);

/**
 * @brief returns a block to the pool in O(1) time.
 */

void kpool_free(kpool *pool, void *ptr);

/**
 * @brief Destroys the pool and frees the entire memory slab.
 */
void kpool_destroy(kpool *pool);

// -- cdsa_arena (Transaction Workspace) API

typedef struct cdsa_arena cdsa_arena;

/*
 * @brief Creates a linear Arena
 * @param capacity The total size of arena in bytes
 * @return pointer to the arena, or NULL on  failure
 */

cdsa_arena *arena_create(size_t capacity);

/*
 * @brief Allocates bytes from the arena by bumping the offset pointer.
 * @param arena The arena to allocate from.
 * @param size The number of bytes to allocate.
 * @return Pointer to the allocated memory, or NULL if out of space.
 */

void *arena_alloc(cdsa_arena *arena, size_t size);

/*
 * @brief resets the arena offset to 0. Instantly frees all allocations.
 */

void arena_reset(cdsa_arena *arena);

/**
 * @brief Destroys the arena and returns the underlying buffer to the system.
 */
void arena_destroy(cdsa_arena *arena);

/*
 * By wrapping standard memory functions in #ifndef blocks,
 * you allow the user to override them at compile time.
 * For example: gcc -DCDSA_MALLOC=my_custom_malloc ...
 */

#ifndef CDSA_MALLOC
#define CDSA_MALLOC(size) cdsa_malloc_track(size)
#endif

#ifndef CDSA_CALLOC
#define CDSA_CALLOC(num, size) cdsa_calloc_track(num, size)
#endif

#ifndef CDSA_REALLOC
#define CDSA_REALLOC(ptr, size) cdsa_realloc_track(ptr, size)
#endif

#ifndef CDSA_FREE
#define CDSA_FREE(ptr) cdsa_free_track(ptr)
#endif

#endif

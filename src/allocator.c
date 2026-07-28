#include "CDSA/allocator.h"
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

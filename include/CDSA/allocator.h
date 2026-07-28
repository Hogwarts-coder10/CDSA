#ifndef CDSA_ALLOCATOR_H
#define CDSA_ALLOCATOR_H

#include <stddef.h>

// Global telemetry sensor
extern size_t cdsa_global_memory_used;

void *cdsa_malloc_track(size_t size);
void *cdsa_calloc_track(size_t count, size_t size);
void *cdsa_realloc_track(void *ptr, size_t new_size);
void cdsa_free_track(void *ptr);

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

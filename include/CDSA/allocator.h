#ifndef CDSA_ALLOCATOR_H
#define CDSA_ALLOCATOR_H

#include <stdlib.h>

/*
 * By wrapping standard memory functions in #ifndef blocks,
 * you allow the user to override them at compile time.
 * For example: gcc -DCDSA_MALLOC=my_custom_malloc ...
 */

#ifndef CDSA_MALLOC
#define CDSA_MALLOC(size) malloc(size)
#endif

#ifndef CDSA_CALLOC
#define CDSA_CALLOC(num, size) calloc(num, size)
#endif

#ifndef CDSA_REALLOC
#define CDSA_REALLOC(ptr, size) realloc(ptr, size)
#endif

#ifndef CDSA_FREE
#define CDSA_FREE(ptr) free(ptr)
#endif

#endif

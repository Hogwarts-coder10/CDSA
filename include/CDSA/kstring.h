#ifndef KSTRING_H
#define KSTRING_H

#include "CDSA/error.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct KString KString;

KString *create_kstring(void);
void free_kstring(KString *str);
size_t size_kstring(KString *str);
const char *c_str_kstring(KString *str);

/**
 * @brief Appends a standard C string to the end of the KString buffer.
 * * @ownership
 * - VALUE: The library takes ownership by copying the characters into its own
 * internal dynamically resizing buffer.
 * - MEMORY: The caller retains ownership of the original 'text' pointer passed
 * in, and can safely modify or free it immediately after the append operation.
 */
CDSA_STATUS append_kstring(KString *str, const char *text);

void clear_kstring(KString *str);
size_t capacity_kstring(KString *str);
bool is_empty_kstring(KString *str);

#endif

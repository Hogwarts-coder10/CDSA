#ifndef KSTRING_H
#define KSTRING_H

#include "CDSA/error.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct cdsa_kstring cdsa_kstring;

cdsa_kstring *cdsa_create_kstring(void);
void cdsa_free_kstring(cdsa_kstring *str);
size_t cdsa_size_kstring(const cdsa_kstring *str);
const char *c_str_kstring(cdsa_kstring *str);

/**
 * @brief Appends a standard C string to the end of the cdsa_kstring buffer.
 * * @ownership
 * - VALUE: The library takes ownership by copying the characters into its own
 * internal dynamically resizing buffer.
 * - MEMORY: The caller retains ownership of the original 'text' pointer passed
 * in, and can safely modify or free it immediately after the append operation.
 */
CDSA_STATUS append_kstring(cdsa_kstring *str, const char *text);

void cdsa_clear_kstring(cdsa_kstring *str);
size_t capacity_kstring(cdsa_kstring *str);
bool cdsa_is_empty_kstring(const cdsa_kstring *str);

#endif

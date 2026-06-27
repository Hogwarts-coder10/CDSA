#ifndef KSTRING_H
#define KSTRING_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {
  char *data;
  size_t size;
  size_t capacity;
} KString;

KString *create_kstring(void);
void free_kstring(KString *str);
size_t size_kstring(KString *str);
const char *c_str_kstring(KString *str);
void append_kstring(KString *str, const char *text);
void clear_kstring(KString *str);
size_t capacity_kstring(KString *str);
bool is_empty_kstring(KString *str);

#endif

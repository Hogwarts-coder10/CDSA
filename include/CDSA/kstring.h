#ifndef KSTRING_H
#define KSTRING_H

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

#endif

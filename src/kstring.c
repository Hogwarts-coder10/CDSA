#include "CDSA/kstring.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define INITIAL_CAPACITY 16

KString *create_kstring(void) {
  KString *str = malloc(sizeof(KString));
  str->size = 0;
  str->capacity = INITIAL_CAPACITY;
  str->data = malloc(str->capacity);

  str->data[0] = '\0';
  return str;
}

void free_kstring(KString *str) {
  if (str == NULL) {
    return;
  }

  free(str->data);
  free(str);
}

size_t size_kstring(KString *str) { return str->size; }

const char *c_str_kstring(KString *str) { return str->data; }

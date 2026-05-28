#include "CDSA/kstring.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

size_t capacity_kstring(KString *str) { return str->capacity; }

bool is_empty_kstring(KString *str) { return str->size == 0; }

void clear_kstring(KString *str) {
  str->size = 0;
  str->data[0] = '\0';
}

void append_kstring(KString *str, const char *text) {
  size_t text_len = strlen(text);

  size_t required_space =
      str->size + text_len + 1; // 1 is for the null terminator

  if (required_space > str->capacity) {

    while (str->capacity < required_space) {
      str->capacity *= 2;
    }

    str->data = realloc(str->data, str->capacity);
  }

  memcpy(str->data + str->size, text, text_len);

  str->size += text_len;
  str->data[str->size] = '\0';
}

#include "CDSA/kstring.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 16

KString *create_kstring(void) {
  KString *str = malloc(sizeof(KString));
  if (str == NULL)
    return NULL;

  str->size = 0;
  str->capacity = INITIAL_CAPACITY;
  str->data = malloc(str->capacity);

  if (str->data == NULL) {
    free(str);
    return NULL;
  }

  str->data[0] = '\0';
  return str;
}

void free_kstring(KString *str) {
  if (str == NULL)
    return;
  free(str->data);
  free(str);
}

size_t size_kstring(KString *str) {
  if (str == NULL)
    return 0;
  return str->size;
}

const char *c_str_kstring(KString *str) {
  if (str == NULL)
    return NULL;
  return str->data;
}

size_t capacity_kstring(KString *str) {
  if (str == NULL)
    return 0;
  return str->capacity;
}

bool is_empty_kstring(KString *str) {
  if (str == NULL)
    return true;
  return str->size == 0;
}

void clear_kstring(KString *str) {
  if (str == NULL)
    return;
  str->size = 0;
  str->data[0] = '\0';
}

void append_kstring(KString *str, const char *text) {
  if (str == NULL || text == NULL)
    return;

  size_t text_len = strlen(text);
  size_t required_space = str->size + text_len + 1;

  if (required_space > str->capacity) {
    while (str->capacity < required_space) {
      str->capacity *= 2;
    }
    // THE FIX -> Safe memory reallocation
    char *temp = realloc(str->data, str->capacity);
    if (temp == NULL)
      return;
    str->data = temp;
  }

  memcpy(str->data + str->size, text, text_len);
  str->size += text_len;
  str->data[str->size] = '\0';
}

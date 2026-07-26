#include "CDSA/kstring.h"
#include "CDSA/allocator.h"
#include "CDSA/error.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 16

struct cdsa_kstring {
  char *data;
  size_t size;
  size_t capacity;
};

cdsa_kstring *cdsa_create_kstring(void) {
  cdsa_kstring *str = CDSA_MALLOC(sizeof(cdsa_kstring));
  if (str == NULL)
    return NULL;

  str->size = 0;
  str->capacity = INITIAL_CAPACITY;
  str->data = CDSA_MALLOC(str->capacity);

  if (str->data == NULL) {
    CDSA_FREE(str);
    return NULL;
  }

  str->data[0] = '\0';
  return str;
}

void cdsa_free_kstring(cdsa_kstring *str) {
  if (str == NULL)
    return;
  CDSA_FREE(str->data);
  CDSA_FREE(str);
}

size_t cdsa_size_kstring(const cdsa_kstring *str) {
  if (str == NULL)
    return 0;
  return str->size;
}

const char *c_str_kstring(cdsa_kstring *str) {
  if (str == NULL)
    return NULL;
  return str->data;
}

size_t capacity_kstring(cdsa_kstring *str) {
  if (str == NULL)
    return 0;
  return str->capacity;
}

bool cdsa_is_empty_kstring(const cdsa_kstring *str) {
  if (str == NULL)
    return true;
  return str->size == 0;
}

void cdsa_clear_kstring(cdsa_kstring *str) {
  if (str == NULL)
    return;
  str->size = 0;
  str->data[0] = '\0';
}

CDSA_STATUS append_kstring(cdsa_kstring *str, const char *text) {
  if (str == NULL || text == NULL)
    return CDSA_ERR_INVALID;

  size_t text_len = strlen(text);
  size_t required_space = str->size + text_len + 1;

  if (required_space > str->capacity) {
    while (str->capacity < required_space) {
      str->capacity *= 2;
    }
    // Safe memory reallocation with explicit error reporting
    char *temp = CDSA_REALLOC(str->data, str->capacity);
    if (temp == NULL) {
      return CDSA_ERR_OOM; // The caller now knows it truncated!
    }
    str->data = temp;
  }

  memcpy(str->data + str->size, text, text_len);
  str->size += text_len;
  str->data[str->size] = '\0';

  return CDSA_OK;
}

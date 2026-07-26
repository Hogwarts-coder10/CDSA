#include "CDSA/vector.h"
#include "CDSA/allocator.h"
#include "CDSA/error.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 4

struct cdsa_vector {
  void *data;
  cdsa_size_t size;
  cdsa_size_t capacity;
  cdsa_size_t elem_size;
  cdsa_size_t version;
};

cdsa_vector *cdsa_create_vector(cdsa_size_t elem_size) {
  cdsa_vector *vec = CDSA_MALLOC(sizeof(cdsa_vector));
  if (vec == NULL)
    return NULL;

  vec->size = 0;
  vec->capacity = INITIAL_CAPACITY;
  vec->elem_size = elem_size;
  vec->version = 0;
  vec->data = CDSA_MALLOC(vec->capacity * elem_size);

  if (vec->data == NULL) {
    CDSA_FREE(vec);
    return NULL;
  }

  return vec;
}

CDSA_STATUS cdsa_push_vector(cdsa_vector *vec, void *elem) {
  if (vec == NULL || elem == NULL)
    return CDSA_ERR_INVALID;

  if (vec->size >= vec->capacity) {
    vec->capacity *= 2;
    // THE FIX = Use a temporary pointer to prevent memory leaks if realloc
    // fails!
    void *temp = CDSA_REALLOC(vec->data, vec->capacity * vec->elem_size);
    if (temp == NULL)
      return CDSA_ERR_OOM;
    vec->data = temp;
  }

  void *target = (char *)vec->data + (vec->size * vec->elem_size);
  memcpy(target, elem, vec->elem_size);
  vec->size++;
  vec->version++;
  return CDSA_OK;
}

void *get_vector(cdsa_vector *vec, cdsa_size_t index) {
  if (vec == NULL || index >= vec->size)
    return NULL;
  return (char *)vec->data + (index * vec->elem_size);
}

void cdsa_free_vector(cdsa_vector *vec) {
  if (vec == NULL)
    return;
  CDSA_FREE(vec->data);
  CDSA_FREE(vec);
}

CDSA_STATUS cdsa_pop_vector(cdsa_vector *vec) {
  if (vec == NULL) {
    return CDSA_ERR_INVALID;
  }

  if (vec->size == 0) {
    return CDSA_ERR_NOT_FOUND;
  }

  vec->size--;
  vec->version++;
  return CDSA_OK;
}

void *cdsa_front_vector(cdsa_vector *vec) {
  if (vec == NULL || vec->size == 0)
    return NULL;
  return vec->data;
}

void *cdsa_back_vector(cdsa_vector *vec) {
  if (vec == NULL || vec->size == 0)
    return NULL;
  return (char *)vec->data + ((vec->size - 1) * vec->elem_size);
}

cdsa_size_t cdsa_size_vector(cdsa_vector *vec) {
  if (vec == NULL)
    return 0;
  return vec->size;
}

cdsa_size_t capacity_vector(cdsa_vector *vec) {
  if (vec == NULL)
    return 0;
  return vec->capacity;
}

CDSA_STATUS set_vector(cdsa_vector *vec, cdsa_size_t index, void *elem) {
  if (vec == NULL || elem == NULL) {
    return CDSA_ERR_INVALID;
  }

  if (index >= vec->size) {
    return CDSA_ERR_NOT_FOUND;
  }

  void *target = (char *)vec->data + (index * vec->elem_size);
  memcpy(target, elem, vec->elem_size);
  return CDSA_OK;
}

void cdsa_clear_vector(cdsa_vector *vec) {
  if (vec == NULL)
    return;
  vec->size = 0;
}

bool cdsa_is_empty_vector(cdsa_vector *vec) {
  if (vec == NULL)
    return true;
  return vec->size == 0;
}

// --- Iterator Implementation ---

struct cdsa_vector_iterator {
  cdsa_vector *vec;
  cdsa_size_t current_index;
  cdsa_size_t snapshot_version;
};

cdsa_vector_iterator *cdsa_create_vector_iterator(cdsa_vector *vec) {
  if (vec == NULL)
    return NULL;

  cdsa_vector_iterator *iter = CDSA_MALLOC(sizeof(cdsa_vector_iterator));
  if (iter == NULL)
    return NULL;

  iter->vec = vec;
  iter->current_index = 0;
  iter->snapshot_version = vec->version; // Capture the safety snapshot

  return iter;
}

bool cdsa_has_next_vector(cdsa_vector_iterator *iter) {
  if (iter == NULL || iter->vec == NULL)
    return false;

  // Guard Check: Has the vector morphed since we started?
  if (iter->vec->version != iter->snapshot_version) {
    return false;
  }

  return iter->current_index < iter->vec->size;
}

CDSA_STATUS cdsa_next_vector(cdsa_vector_iterator *iter, void **out_value) {
  if (iter == NULL || out_value == NULL)
    return CDSA_ERR_INVALID;

  // Guard Check: Fail-fast if structural mutation occurred
  if (iter->vec->version != iter->snapshot_version) {
    return CDSA_ERR_ITER_INVALIDATED;
  }

  if (!cdsa_has_next_vector(iter)) {
    return CDSA_ERR_NOT_FOUND;
  }

  // Yield the pointer directly into the array at the current index
  // Note: Adjust the pointer arithmetic based on your internal get_vector
  // implementation
  char *byte_array = (char *)iter->vec->data;
  *out_value =
      (void *)(byte_array + (iter->current_index * iter->vec->elem_size));

  iter->current_index++;
  return CDSA_OK;
}

void cdsa_free_vector_iterator(cdsa_vector_iterator *iter) {
  if (iter == NULL)
    return;
  CDSA_FREE(iter);
}

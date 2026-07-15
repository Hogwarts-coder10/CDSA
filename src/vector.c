#include "CDSA/vector.h"
#include "CDSA/allocator.h"
#include "CDSA/error.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 4

struct Vector {
  void *data;
  size_t size;
  size_t capacity;
  size_t elem_size;
  size_t version;
};

Vector *create_vector(size_t elem_size) {
  Vector *vec = CDSA_MALLOC(sizeof(Vector));
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

CDSA_STATUS push_vector(Vector *vec, void *elem) {
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

void *get_vector(Vector *vec, size_t index) {
  if (vec == NULL || index >= vec->size)
    return NULL;
  return (char *)vec->data + (index * vec->elem_size);
}

void free_vector(Vector *vec) {
  if (vec == NULL)
    return;
  CDSA_FREE(vec->data);
  CDSA_FREE(vec);
}

CDSA_STATUS pop_vector(Vector *vec) {
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

void *front_vector(Vector *vec) {
  if (vec == NULL || vec->size == 0)
    return NULL;
  return vec->data;
}

void *back_vector(Vector *vec) {
  if (vec == NULL || vec->size == 0)
    return NULL;
  return (char *)vec->data + ((vec->size - 1) * vec->elem_size);
}

size_t size_vector(Vector *vec) {
  if (vec == NULL)
    return 0;
  return vec->size;
}

size_t capacity_vector(Vector *vec) {
  if (vec == NULL)
    return 0;
  return vec->capacity;
}

CDSA_STATUS set_vector(Vector *vec, size_t index, void *elem) {
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

void clear_vector(Vector *vec) {
  if (vec == NULL)
    return;
  vec->size = 0;
}

bool is_empty_vector(Vector *vec) {
  if (vec == NULL)
    return true;
  return vec->size == 0;
}

// --- Iterator Implementation ---

struct VectorIterator {
  Vector *vec;
  size_t current_index;
  size_t snapshot_version;
};

VectorIterator *create_vector_iterator(Vector *vec) {
  if (vec == NULL)
    return NULL;

  VectorIterator *iter = CDSA_MALLOC(sizeof(VectorIterator));
  if (iter == NULL)
    return NULL;

  iter->vec = vec;
  iter->current_index = 0;
  iter->snapshot_version = vec->version; // Capture the safety snapshot

  return iter;
}

bool has_next_vector(VectorIterator *iter) {
  if (iter == NULL || iter->vec == NULL)
    return false;

  // Guard Check: Has the vector morphed since we started?
  if (iter->vec->version != iter->snapshot_version) {
    return false;
  }

  return iter->current_index < iter->vec->size;
}

CDSA_STATUS next_vector(VectorIterator *iter, void **out_value) {
  if (iter == NULL || out_value == NULL)
    return CDSA_ERR_INVALID;

  // Guard Check: Fail-fast if structural mutation occurred
  if (iter->vec->version != iter->snapshot_version) {
    return CDSA_ERR_ITER_INVALIDATED;
  }

  if (!has_next_vector(iter)) {
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

void free_vector_iterator(VectorIterator *iter) {
  if (iter == NULL)
    return;
  CDSA_FREE(iter);
}

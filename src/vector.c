#include "CDSA/vector.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 4

Vector *create_vector(size_t elem_size) {
  Vector *vec = malloc(sizeof(Vector));
  if (vec == NULL)
    return NULL;

  vec->size = 0;
  vec->capacity = INITIAL_CAPACITY;
  vec->elem_size = elem_size;
  vec->data = malloc(vec->capacity * elem_size);

  if (vec->data == NULL) {
    free(vec);
    return NULL;
  }

  return vec;
}

void push_vector(Vector *vec, void *elem) {
  if (vec == NULL || elem == NULL)
    return;

  if (vec->size >= vec->capacity) {
    vec->capacity *= 2;
    // THE FIX = Use a temporary pointer to prevent memory leaks if realloc
    // fails!
    void *temp = realloc(vec->data, vec->capacity * vec->elem_size);
    if (temp == NULL)
      return;
    vec->data = temp;
  }

  void *target = (char *)vec->data + (vec->size * vec->elem_size);
  memcpy(target, elem, vec->elem_size);
  vec->size++;
}

void *get_vector(Vector *vec, size_t index) {
  if (vec == NULL || index >= vec->size)
    return NULL;
  return (char *)vec->data + (index * vec->elem_size);
}

void free_vector(Vector *vec) {
  if (vec == NULL)
    return;
  free(vec->data);
  free(vec);
}

void pop_vector(Vector *vec) {
  if (vec == NULL)
    return;
  if (vec->size > 0)
    vec->size--;
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

void set_vector(Vector *vec, size_t index, void *elem) {
  if (vec == NULL || elem == NULL || index >= vec->size)
    return;
  void *target = (char *)vec->data + (index * vec->elem_size);
  memcpy(target, elem, vec->elem_size);
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

#include "CDSA/vector.h"
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 4

Vector *create_vector(size_t elem_size) {
  Vector *vec = malloc(sizeof(Vector));

  vec->size = 0;
  vec->capacity = INITIAL_CAPACITY;
  vec->elem_size = elem_size;
  vec->data = malloc(vec->capacity * elem_size);

  return vec;
}

void push_vector(Vector *vec, void *elem) {
  if (vec->size >= vec->capacity) {
    vec->capacity *= 2;

    vec->data = realloc(vec->data, vec->capacity * vec->elem_size);
  }

  void *target = (char *)vec->data + (vec->size * vec->elem_size);

  memcpy(target, elem, vec->elem_size);

  vec->size++;
}

void *get_vector(Vector *vec, size_t index) {
  return (char *)vec->data + (index * vec->elem_size);
}

void free_vector(Vector *vec) {
  free(vec->data);
  free(vec);
}

void pop_vector(Vector *vec) {
  if (vec->size > 0) {
    vec->size--;
  }
}

void *front_vector(Vector *vec) {
  if (vec->size == 0) {
    return NULL;
  }

  return vec->data;
}

void *back_vector(Vector *vec) {
  if (vec->size == 0) {
    return NULL;
  }

  return (char *)vec->data + ((vec->size - 1) * vec->elem_size);
}

size_t size_vector(Vector *vec) { return vec->size; }

size_t capacity_vector(Vector *vec) { return vec->capacity; }

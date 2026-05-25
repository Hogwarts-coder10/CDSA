#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>

typedef struct {
  void *data;
  size_t size;
  size_t capacity;
  size_t elem_size;
} Vector;

Vector *create_vector(size_t elem_size);
void push_vector(Vector *vec, void *elem);
void *get_vector(Vector *vec, size_t index);
void free_vector(Vector *vec);
void pop_vector(Vector *vec);
void *front_vector(Vector *vec);
void *back_vector(Vector *vec);
#endif

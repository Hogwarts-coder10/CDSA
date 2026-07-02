#ifndef VECTOR_H
#define VECTOR_H

#include <stdbool.h>
#include <stddef.h>

typedef struct Vector Vector;

Vector *create_vector(size_t elem_size);
void push_vector(Vector *vec, void *elem);
void *get_vector(Vector *vec, size_t index);
void free_vector(Vector *vec);
void pop_vector(Vector *vec);
void *front_vector(Vector *vec);
void *back_vector(Vector *vec);
size_t size_vector(Vector *vec);
size_t capacity_vector(Vector *vec);
void set_vector(Vector *vec, size_t index, void *elem);
void clear_vector(Vector *vec);
bool is_empty_vector(Vector *vec);

#endif

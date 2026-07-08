#ifndef VECTOR_H
#define VECTOR_H

#include "CDSA/error.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct Vector Vector;

Vector *create_vector(size_t elem_size);
/**
 * @brief Pushes an element into the collection.
 *
 * @ownership
 * - VALUE: The library creates a shallow, byte-for-byte copy using memcpy
 *   based on the collection's configured elem_size.
 * - MEMORY: If the element is a pointer to dynamically allocated memory,
 *   the caller retains ownership of that underlying memory and must free it.
 */

CDSA_STATUS push_vector(Vector *vec, void *elem);
void *get_vector(Vector *vec, size_t index);
void free_vector(Vector *vec);
CDSA_STATUS pop_vector(Vector *vec);
void *front_vector(Vector *vec);
void *back_vector(Vector *vec);
size_t size_vector(Vector *vec);
size_t capacity_vector(Vector *vec);
CDSA_STATUS set_vector(Vector *vec, size_t index, void *elem);
void clear_vector(Vector *vec);
bool is_empty_vector(Vector *vec);

#endif

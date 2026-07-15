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

// --- Iterator API ---

typedef struct VectorIterator VectorIterator;

/**
 * @brief Creates a new iterator for the Vector.
 * @warning The caller must free the iterator using free_vector_iterator.
 */
VectorIterator *create_vector_iterator(Vector *vec);

/**
 * @brief Checks if there are more elements to read.
 */
bool has_next_vector(VectorIterator *iter);

/**
 * @brief Advances the iterator and retrieves a pointer to the next element.
 * * @ownership
 * - YIELD: Returns a temporary pointer DIRECTLY into the vector's internal
 * array.
 * - WARNING: Do NOT free this pointer. It will be invalidated immediately if
 * the vector undergoes any structural modifications (resizing, pushing, or
 * popping).
 * * @return CDSA_OK on success, or CDSA_ERR_ITER_INVALIDATED if a concurrent
 * mutation occurred during the walk pass.
 */
CDSA_STATUS next_vector(VectorIterator *iter, void **out_value);

/**
 * @brief Frees the iterator memory.
 */
void free_vector_iterator(VectorIterator *iter);
#endif

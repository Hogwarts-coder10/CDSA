#ifndef VECTOR_H
#define VECTOR_H

#include "CDSA/error.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct cdsa_vector cdsa_vector;

cdsa_vector *cdsa_create_vector(size_t elem_size);
/**
 * @brief Pushes an element into the collection.
 *
 * @ownership
 * - VALUE: The library creates a shallow, byte-for-byte copy using memcpy
 *   based on the collection's configured elem_size.
 * - MEMORY: If the element is a pointer to dynamically allocated memory,
 *   the caller retains ownership of that underlying memory and must free it.
 */

CDSA_STATUS cdsa_push_vector(cdsa_vector *vec, void *elem);
void *get_vector(cdsa_vector *vec, size_t index);
void cdsa_free_vector(cdsa_vector *vec);
CDSA_STATUS cdsa_pop_vector(cdsa_vector *vec);
void *cdsa_front_vector(const cdsa_vector *vec);
void *cdsa_back_vector(const cdsa_vector *vec);
size_t cdsa_size_vector(const cdsa_vector *vec);
size_t capacity_vector(cdsa_vector *vec);
CDSA_STATUS set_vector(cdsa_vector *vec, size_t index, void *elem);
void cdsa_clear_vector(cdsa_vector *vec);
bool cdsa_is_empty_vector(const cdsa_vector *vec);

// --- Iterator API ---

typedef struct cdsa_vector_iterator cdsa_vector_iterator;

/**
 * @brief Creates a new iterator for the cdsa_vector.
 * @warning The caller must free the iterator using cdsa_free_vector_iterator.
 */
cdsa_vector_iterator *cdsa_create_vector_iterator(const cdsa_vector *vec);

/**
 * @brief Checks if there are more elements to read.
 */
bool cdsa_has_next_vector(cdsa_vector_iterator *iter);

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
CDSA_STATUS cdsa_next_vector(cdsa_vector_iterator *iter, void **out_value);

/**
 * @brief Frees the iterator memory.
 */
void cdsa_free_vector_iterator(cdsa_vector_iterator *iter);
#endif

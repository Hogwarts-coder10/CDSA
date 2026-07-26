#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include "CDSA/error.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct cdsa_ringbuffer cdsa_ringbuffer;

cdsa_ringbuffer *cdsa_create_ringbuffer(cdsa_size_t capacity, cdsa_size_t elem_size);
void cdsa_free_ringbuffer(cdsa_ringbuffer *rb);

// --- cdsa_queue Operations (FIFO) ---
/**
 * @brief Pushes an element into the collection.
 *
 * @ownership
 * - VALUE: The library creates a shallow, byte-for-byte copy using memcpy
 *   based on the collection's configured elem_size.
 * - MEMORY: If the element is a pointer to dynamically allocated memory,
 *   the caller retains ownership of that underlying memory and must free it.
 */
CDSA_STATUS cdsa_push_back_ringbuffer(cdsa_ringbuffer *rb, void *elem);
CDSA_STATUS cdsa_pop_front_ringbuffer(cdsa_ringbuffer *rb);
void *cdsa_front_ringbuffer(cdsa_ringbuffer *rb);

// --- cdsa_deque Operations (Double-Ended) ---
CDSA_STATUS cdsa_push_front_ringbuffer(cdsa_ringbuffer *rb, void *elem);
CDSA_STATUS cdsa_pop_back_ringbuffer(cdsa_ringbuffer *rb);
void *cdsa_back_ringbuffer(cdsa_ringbuffer *rb);

// --- Utilities ---
cdsa_size_t cdsa_size_ringbuffer(cdsa_ringbuffer *rb);
bool cdsa_is_empty_ringbuffer(cdsa_ringbuffer *rb);
bool cdsa_is_full_ringbuffer(cdsa_ringbuffer *rb);

// --- Iterator API ---

typedef struct cdsa_ringbuffer_iterator cdsa_ringbuffer_iterator;

/**
 * @brief Creates a new iterator for the cdsa_ringbuffer.
 * @warning The caller must free the iterator using cdsa_free_ringbuffer_iterator.
 */
cdsa_ringbuffer_iterator *cdsa_create_ringbuffer_iterator(cdsa_ringbuffer *rb);

/**
 * @brief Checks if there are more elements to read.
 */
bool cdsa_has_next_ringbuffer(cdsa_ringbuffer_iterator *iter);

/**
 * @brief Advances the iterator and retrieves a pointer to the next element.
 * * @ownership
 * - YIELD: Returns a temporary pointer directly to the slot inside the internal
 * circular array.
 * - WARNING: Do NOT free this pointer. It is invalidated if the ringbuffer is
 * modified.
 * * @return CDSA_OK on success, or CDSA_ERR_ITER_INVALIDATED if a concurrent
 * mutation occurred.
 */
CDSA_STATUS cdsa_next_ringbuffer(cdsa_ringbuffer_iterator *iter, void **out_value);

/**
 * @brief Frees the iterator memory.
 */
void cdsa_free_ringbuffer_iterator(cdsa_ringbuffer_iterator *iter);

#endif

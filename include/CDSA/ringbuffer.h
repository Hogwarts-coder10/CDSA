#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include "CDSA/error.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct RingBuffer RingBuffer;

RingBuffer *create_ringbuffer(size_t capacity, size_t elem_size);
void free_ringbuffer(RingBuffer *rb);

// --- Queue Operations (FIFO) ---
/**
 * @brief Pushes an element into the collection.
 *
 * @ownership
 * - VALUE: The library creates a shallow, byte-for-byte copy using memcpy
 *   based on the collection's configured elem_size.
 * - MEMORY: If the element is a pointer to dynamically allocated memory,
 *   the caller retains ownership of that underlying memory and must free it.
 */
CDSA_STATUS push_back_ringbuffer(RingBuffer *rb, void *elem);
CDSA_STATUS pop_front_ringbuffer(RingBuffer *rb);
void *front_ringbuffer(RingBuffer *rb);

// --- Deque Operations (Double-Ended) ---
CDSA_STATUS push_front_ringbuffer(RingBuffer *rb, void *elem);
CDSA_STATUS pop_back_ringbuffer(RingBuffer *rb);
void *back_ringbuffer(RingBuffer *rb);

// --- Utilities ---
size_t size_ringbuffer(RingBuffer *rb);
bool is_empty_ringbuffer(RingBuffer *rb);
bool is_full_ringbuffer(RingBuffer *rb);

// --- Iterator API ---

typedef struct RingBufferIterator RingBufferIterator;

/**
 * @brief Creates a new iterator for the RingBuffer.
 * @warning The caller must free the iterator using free_ringbuffer_iterator.
 */
RingBufferIterator *create_ringbuffer_iterator(RingBuffer *rb);

/**
 * @brief Checks if there are more elements to read.
 */
bool has_next_ringbuffer(RingBufferIterator *iter);

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
CDSA_STATUS next_ringbuffer(RingBufferIterator *iter, void **out_value);

/**
 * @brief Frees the iterator memory.
 */
void free_ringbuffer_iterator(RingBufferIterator *iter);

#endif

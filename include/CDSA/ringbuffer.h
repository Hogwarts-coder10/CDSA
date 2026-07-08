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

#endif

#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <stdbool.h>
#include <stddef.h>

typedef struct RingBuffer RingBuffer;

RingBuffer *create_ringbuffer(size_t capacity, size_t elem_size);
void free_ringbuffer(RingBuffer *rb);

// --- Queue Operations (FIFO) ---
bool push_back_ringbuffer(RingBuffer *rb, void *elem);
void pop_front_ringbuffer(RingBuffer *rb);
void *front_ringbuffer(RingBuffer *rb);

// --- Deque Operations (Double-Ended) ---
bool push_front_ringbuffer(RingBuffer *rb, void *elem);
void pop_back_ringbuffer(RingBuffer *rb);
void *back_ringbuffer(RingBuffer *rb);

// --- Utilities ---
size_t size_ringbuffer(RingBuffer *rb);
bool is_empty_ringbuffer(RingBuffer *rb);
bool is_full_ringbuffer(RingBuffer *rb);

#endif

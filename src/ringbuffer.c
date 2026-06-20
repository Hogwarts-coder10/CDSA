#include "CDSA/ringbuffer.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// --- Core Lifecycle ---

RingBuffer *create_ringbuffer(size_t capacity, size_t elem_size) {
  RingBuffer *rb = malloc(sizeof(RingBuffer));

  rb->capacity = capacity;
  rb->elem_size = elem_size;
  rb->size = 0;
  rb->head = 0;
  rb->tail = 0;

  // Allocate the continuous block of memory
  rb->data = malloc(capacity * elem_size);

  return rb;
}

void free_ringbuffer(RingBuffer *rb) {
  if (rb == NULL)
    return;
  free(rb->data);
  free(rb);
}

// --- Utilities ---

size_t size_ringbuffer(RingBuffer *rb) { return rb->size; }
bool is_empty_ringbuffer(RingBuffer *rb) { return rb->size == 0; }
bool is_full_ringbuffer(RingBuffer *rb) { return rb->size == rb->capacity; }

// --- Queue Operations (FIFO) ---

bool push_back_ringbuffer(RingBuffer *rb, void *elem) {
  if (is_full_ringbuffer(rb)) {
    return false; // Buffer is full, drop the insertion
  }

  // Calculate the actual memory address for the tail
  void *target = (char *)rb->data + (rb->tail * rb->elem_size);
  memcpy(target, elem, rb->elem_size);

  // THE MAGIC: Move tail forward, wrap around to 0 if it hits capacity
  rb->tail = (rb->tail + 1) % rb->capacity;
  rb->size++;

  return true;
}

void pop_front_ringbuffer(RingBuffer *rb) {
  if (is_empty_ringbuffer(rb)) {
    return;
  }

  // THE MAGIC: We don't delete the data or shift the array.
  // We just move the head pointer forward and wrap it around!
  rb->head = (rb->head + 1) % rb->capacity;
  rb->size--;
}

void *front_ringbuffer(RingBuffer *rb) {
  if (is_empty_ringbuffer(rb)) {
    return NULL;
  }

  return (char *)rb->data + (rb->head * rb->elem_size);
}

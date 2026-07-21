#include "CDSA/ringbuffer.h"
#include "CDSA/allocator.h"
#include "CDSA/error.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

struct RingBuffer {
  void *data;
  size_t head;
  size_t tail;
  size_t size;
  size_t capacity;
  size_t elem_size;
  size_t version;
};

// --- Core Lifecycle ---

RingBuffer *create_ringbuffer(size_t capacity, size_t elem_size) {
  RingBuffer *rb = CDSA_MALLOC(sizeof(RingBuffer));
  if (rb == NULL) {
    return NULL;
  }

  rb->capacity = capacity;
  rb->elem_size = elem_size;
  rb->size = 0;
  rb->head = 0;
  rb->tail = 0;
  rb->version = 0;

  // Allocate the continuous block of memory
  rb->data = CDSA_MALLOC(capacity * elem_size);
  if (rb->data == NULL) {
    CDSA_FREE(rb);
    return NULL;
  }

  return rb;
}

void free_ringbuffer(RingBuffer *rb) {
  if (rb == NULL)
    return;
  CDSA_FREE(rb->data);
  CDSA_FREE(rb);
}

// --- Utilities ---

size_t size_ringbuffer(RingBuffer *rb) {
  if (rb == NULL)
    return 0;
  return rb->size;
}

bool is_empty_ringbuffer(RingBuffer *rb) {
  if (rb == NULL)
    return true;
  return rb->size == 0;
}

bool is_full_ringbuffer(RingBuffer *rb) {
  if (rb == NULL)
    return false;
  return rb->size == rb->capacity;
}

// --- Queue Operations (FIFO) ---

CDSA_STATUS push_back_ringbuffer(RingBuffer *rb, void *elem) {
  if (rb == NULL || elem == NULL) {
    return CDSA_ERR_INVALID;
  }

  if (is_full_ringbuffer(rb)) {
    return CDSA_ERR_FULL; // Buffer is full, drop the insertion explicitly
  }

  // Calculate the actual memory address for the tail
  void *target = (char *)rb->data + (rb->tail * rb->elem_size);
  memcpy(target, elem, rb->elem_size);

  // THE MAGIC: Move tail forward, wrap around to 0 if it hits capacity
  rb->tail = (rb->tail + 1) % rb->capacity;
  rb->size++;
  rb->version++;
  return CDSA_OK;
}

CDSA_STATUS pop_front_ringbuffer(RingBuffer *rb) {
  if (rb == NULL) {
    return CDSA_ERR_INVALID;
  }

  if (is_empty_ringbuffer(rb)) {
    return CDSA_ERR_EMPTY;
  }

  // THE MAGIC: We don't delete the data or shift the array.
  // We just move the head pointer forward and wrap it around!
  rb->head = (rb->head + 1) % rb->capacity;
  rb->size--;
  rb->version++;

  return CDSA_OK;
}

void *front_ringbuffer(RingBuffer *rb) {
  if (rb == NULL || is_empty_ringbuffer(rb)) {
    return NULL;
  }

  return (char *)rb->data + (rb->head * rb->elem_size);
}

// --- Deque Operations (Double-Ended) ---

CDSA_STATUS push_front_ringbuffer(RingBuffer *rb, void *elem) {
  if (rb == NULL || elem == NULL) {
    return CDSA_ERR_INVALID;
  }

  if (is_full_ringbuffer(rb)) {
    return CDSA_ERR_FULL;
  }

  // REVERSE MAGIC: Move head backwards, wrapping to the end if it hits 0
  rb->head = (rb->head + rb->capacity - 1) % rb->capacity;

  void *target = (char *)rb->data + (rb->head * rb->elem_size);
  memcpy(target, elem, rb->elem_size);

  rb->size++;
  rb->version++;
  return CDSA_OK;
}

CDSA_STATUS pop_back_ringbuffer(RingBuffer *rb) {
  if (rb == NULL) {
    return CDSA_ERR_INVALID;
  }

  if (is_empty_ringbuffer(rb)) {
    return CDSA_ERR_EMPTY;
  }

  // REVERSE MAGIC: Move tail backwards, wrapping around
  rb->tail = (rb->tail + rb->capacity - 1) % rb->capacity;
  rb->size--;
  rb->version++;

  return CDSA_OK;
}

void *back_ringbuffer(RingBuffer *rb) {
  if (rb == NULL || is_empty_ringbuffer(rb)) {
    return NULL;
  }

  // The "back" element is always one step behind the current tail
  size_t last_idx = (rb->tail + rb->capacity - 1) % rb->capacity;
  return (char *)rb->data + (last_idx * rb->elem_size);
}

// --- Iterator Implementation ---

struct RingBufferIterator {
  RingBuffer *rb;
  size_t progress;         // Tracks logical steps: from 0 to rb->size - 1
  size_t snapshot_version; // Safety lock against mid-walk modifications
};

RingBufferIterator *create_ringbuffer_iterator(RingBuffer *rb) {
  if (rb == NULL)
    return NULL;

  RingBufferIterator *iter = CDSA_MALLOC(sizeof(RingBufferIterator));
  if (iter == NULL)
    return NULL;

  iter->rb = rb;
  iter->progress = 0;
  iter->snapshot_version = rb->version;

  return iter;
}

bool has_next_ringbuffer(RingBufferIterator *iter) {
  if (iter == NULL || iter->rb == NULL)
    return false;

  // Guard Check
  if (iter->rb->version != iter->snapshot_version) {
    return false;
  }

  return iter->progress < iter->rb->size;
}

CDSA_STATUS next_ringbuffer(RingBufferIterator *iter, void **out_value) {
  if (iter == NULL || out_value == NULL)
    return CDSA_ERR_INVALID;

  // Guard Check
  if (iter->rb->version != iter->snapshot_version) {
    return CDSA_ERR_ITER_INVALIDATED;
  }

  if (!has_next_ringbuffer(iter)) {
    return CDSA_ERR_NOT_FOUND;
  }

  // Calculate the circular array index based on current progress
  size_t physical_index =
      (iter->rb->head + iter->progress) % iter->rb->capacity;

  // Point directly to the element at that index
  char *byte_array = (char *)iter->rb->data;
  *out_value = (void *)(byte_array + (physical_index * iter->rb->elem_size));

  iter->progress++;
  return CDSA_OK;
}

void free_ringbuffer_iterator(RingBufferIterator *iter) {
  if (iter == NULL)
    return;
  CDSA_FREE(iter);
}

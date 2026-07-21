#include "CDSA/deque.h"
#include "CDSA/allocator.h"
#include "CDSA/error.h"
#include "CDSA/ringbuffer.h"
#include <stdlib.h>
#include <string.h>

struct Deque {
  RingBuffer *rb;
};

// --- Core Lifecycle ---

Deque *create_deque(size_t capacity, size_t elem_size) {
  Deque *deque = CDSA_MALLOC(sizeof(Deque));

  if (deque == NULL) {
    return NULL;
  }

  deque->rb = create_ringbuffer(capacity, elem_size);

  if (deque->rb == NULL) {
    CDSA_FREE(deque);
    return NULL;
  }
  return deque;
}

void free_deque(Deque *deque) {
  if (deque == NULL)
    return;
  free_ringbuffer(deque->rb);
  CDSA_FREE(deque);
}

// --- Front Operations ---

CDSA_STATUS push_front_deque(Deque *deque, void *elem) {
  if (deque == NULL || elem == NULL) {
    return CDSA_ERR_INVALID;
  }
  return push_front_ringbuffer(deque->rb, elem);
}

CDSA_STATUS pop_front_deque(Deque *deque) {
  if (deque == NULL) {
    return CDSA_ERR_INVALID;
  }
  return pop_front_ringbuffer(deque->rb);
}

void *front_deque(Deque *deque) {
  if (deque == NULL)
    return NULL;
  return front_ringbuffer(deque->rb);
}

// --- Back Operations ---

CDSA_STATUS push_back_deque(Deque *deque, void *elem) {
  if (deque == NULL || elem == NULL) {
    return CDSA_ERR_INVALID;
  }
  return push_back_ringbuffer(deque->rb, elem);
}

CDSA_STATUS pop_back_deque(Deque *deque) {
  if (deque == NULL) {
    return CDSA_ERR_INVALID;
  }
  return pop_back_ringbuffer(deque->rb);
}

void *back_deque(Deque *deque) {
  if (deque == NULL)
    return NULL;
  return back_ringbuffer(deque->rb);
}

// --- Utilities ---

size_t size_deque(Deque *deque) {
  if (deque == NULL)
    return 0;
  return size_ringbuffer(deque->rb);
}

bool is_empty_deque(Deque *deque) {
  if (deque == NULL)
    return true;
  return is_empty_ringbuffer(deque->rb);
}

bool is_full_deque(Deque *deque) {
  if (deque == NULL)
    return false;
  return is_full_ringbuffer(deque->rb);
}

// --- Iterator Implementation ---

DequeIterator *create_deque_iterator(Deque *d) {
  if (d == NULL)
    return NULL;
  return (DequeIterator *)create_ringbuffer_iterator(d->rb);
}

bool has_next_deque(DequeIterator *iter) {
  return has_next_ringbuffer((RingBufferIterator *)iter);
}

CDSA_STATUS next_deque(DequeIterator *iter, void **out_value) {
  return next_ringbuffer((RingBufferIterator *)iter, out_value);
}

void free_deque_iterator(DequeIterator *iter) {
  free_ringbuffer_iterator((RingBufferIterator *)iter);
}

#include "CDSA/deque.h"
#include "CDSA/ringbuffer.h"
#include <stdlib.h>
#include <string.h>

struct Deque {
  RingBuffer *rb;
};

// --- Core Lifecycle ---

Deque *create_deque(size_t capacity, size_t elem_size) {
  Deque *deque = malloc(sizeof(Deque));

  if (deque == NULL) {
    return NULL;
  }

  deque->rb = create_ringbuffer(capacity, elem_size);

  if (deque->rb == NULL) {
    free(deque);
    return NULL;
  }
  return deque;
}

void free_deque(Deque *deque) {
  if (deque == NULL)
    return;
  free_ringbuffer(deque->rb);
  free(deque);
}

// --- Front Operations ---

bool push_front_deque(Deque *deque, void *elem) {
  return push_front_ringbuffer(deque->rb, elem);
}

void pop_front_deque(Deque *deque) { pop_front_ringbuffer(deque->rb); }

void *front_deque(Deque *deque) { return front_ringbuffer(deque->rb); }

// --- Back Operations ---

bool push_back_deque(Deque *deque, void *elem) {
  return push_back_ringbuffer(deque->rb, elem);
}

void pop_back_deque(Deque *deque) { pop_back_ringbuffer(deque->rb); }

void *back_deque(Deque *deque) { return back_ringbuffer(deque->rb); }

// --- Utilities ---

size_t size_deque(Deque *deque) { return size_ringbuffer(deque->rb); }

bool is_empty_deque(Deque *deque) { return is_empty_ringbuffer(deque->rb); }

bool is_full_deque(Deque *deque) { return is_full_ringbuffer(deque->rb); }

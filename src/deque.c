#include "CDSA/deque.h"
#include "CDSA/allocator.h"
#include "CDSA/error.h"
#include "CDSA/ringbuffer.h"
#include <stdlib.h>
#include <string.h>

struct cdsa_deque {
  cdsa_ringbuffer *rb;
};

// --- Core Lifecycle ---

cdsa_deque *cdsa_create_deque(size_t capacity, size_t elem_size) {
  cdsa_deque *deque = CDSA_MALLOC(sizeof(cdsa_deque));

  if (deque == NULL) {
    return NULL;
  }

  deque->rb = cdsa_create_ringbuffer(capacity, elem_size);

  if (deque->rb == NULL) {
    CDSA_FREE(deque);
    return NULL;
  }
  return deque;
}

void cdsa_free_deque(cdsa_deque *deque) {
  if (deque == NULL)
    return;
  cdsa_free_ringbuffer(deque->rb);
  CDSA_FREE(deque);
}

// --- Front Operations ---

CDSA_STATUS cdsa_push_front_deque(cdsa_deque *deque, void *elem) {
  if (deque == NULL || elem == NULL) {
    return CDSA_ERR_INVALID;
  }
  return cdsa_push_front_ringbuffer(deque->rb, elem);
}

CDSA_STATUS cdsa_pop_front_deque(cdsa_deque *deque) {
  if (deque == NULL) {
    return CDSA_ERR_INVALID;
  }
  return cdsa_pop_front_ringbuffer(deque->rb);
}

void *cdsa_front_deque(const cdsa_deque *deque) {
  if (deque == NULL)
    return NULL;
  return cdsa_front_ringbuffer(deque->rb);
}

// --- Back Operations ---

CDSA_STATUS cdsa_push_back_deque(cdsa_deque *deque, void *elem) {
  if (deque == NULL || elem == NULL) {
    return CDSA_ERR_INVALID;
  }
  return cdsa_push_back_ringbuffer(deque->rb, elem);
}

CDSA_STATUS cdsa_pop_back_deque(cdsa_deque *deque) {
  if (deque == NULL) {
    return CDSA_ERR_INVALID;
  }
  return cdsa_pop_back_ringbuffer(deque->rb);
}

void *cdsa_back_deque(const cdsa_deque *deque) {
  if (deque == NULL)
    return NULL;
  return cdsa_back_ringbuffer(deque->rb);
}

// --- Utilities ---

size_t cdsa_size_deque(const cdsa_deque *deque) {
  if (deque == NULL)
    return 0;
  return cdsa_size_ringbuffer(deque->rb);
}

bool cdsa_is_empty_deque(const cdsa_deque *deque) {
  if (deque == NULL)
    return true;
  return cdsa_is_empty_ringbuffer(deque->rb);
}

bool cdsa_is_full_deque(const cdsa_deque *deque) {
  if (deque == NULL)
    return false;
  return cdsa_is_full_ringbuffer(deque->rb);
}

// --- Iterator Implementation ---

cdsa_deque_iterator *cdsa_create_deque_iterator(const cdsa_deque *d) {
  if (d == NULL)
    return NULL;
  return (cdsa_deque_iterator *)cdsa_create_ringbuffer_iterator(d->rb);
}

bool cdsa_has_next_deque(cdsa_deque_iterator *iter) {
  return cdsa_has_next_ringbuffer((cdsa_ringbuffer_iterator *)iter);
}

CDSA_STATUS cdsa_next_deque(cdsa_deque_iterator *iter, void **out_value) {
  return cdsa_next_ringbuffer((cdsa_ringbuffer_iterator *)iter, out_value);
}

void cdsa_free_deque_iterator(cdsa_deque_iterator *iter) {
  cdsa_free_ringbuffer_iterator((cdsa_ringbuffer_iterator *)iter);
}

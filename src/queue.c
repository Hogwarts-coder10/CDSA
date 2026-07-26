#include "CDSA/queue.h"
#include "CDSA/allocator.h"
#include "CDSA/error.h"
#include "CDSA/ringbuffer.h"
#include <stdlib.h>
#include <string.h>

struct cdsa_queue {
  cdsa_ringbuffer *rb;
};

cdsa_queue *cdsa_create_queue(cdsa_size_t capacity, cdsa_size_t elem_size) {
  cdsa_queue *queue = CDSA_MALLOC(sizeof(cdsa_queue));
  if (queue == NULL) {
    return NULL;
  }

  queue->rb = cdsa_create_ringbuffer(capacity, elem_size);

  if (queue->rb == NULL) {
    CDSA_FREE(queue);
    return NULL;
  }

  return queue;
}

void cdsa_free_queue(cdsa_queue *queue) {
  if (queue == NULL)
    return;

  cdsa_free_ringbuffer(queue->rb);
  CDSA_FREE(queue);
}

CDSA_STATUS cdsa_enqueue(cdsa_queue *queue, void *elem) {
  if (queue == NULL || elem == NULL) {
    return CDSA_ERR_INVALID;
  }
  // Enqueue is just a restricted cdsa_push_back
  return cdsa_push_back_ringbuffer(queue->rb, elem);
}

CDSA_STATUS cdsa_dequeue(cdsa_queue *queue) {
  if (queue == NULL) {
    return CDSA_ERR_INVALID;
  }
  // Dequeue is just a restricted cdsa_pop_front
  return cdsa_pop_front_ringbuffer(queue->rb);
}

void *cdsa_front_queue(cdsa_queue *queue) {
  if (queue == NULL)
    return NULL;
  return cdsa_front_ringbuffer(queue->rb);
}

// --- Utilities ---

cdsa_size_t cdsa_size_queue(cdsa_queue *queue) {
  if (queue == NULL)
    return 0;
  return cdsa_size_ringbuffer(queue->rb);
}

bool cdsa_is_empty_queue(cdsa_queue *queue) {
  if (queue == NULL)
    return true;
  return cdsa_is_empty_ringbuffer(queue->rb);
}

bool cdsa_is_full_queue(cdsa_queue *queue) {
  if (queue == NULL)
    return false;
  return cdsa_is_full_ringbuffer(queue->rb);
}

// --- Iterator Implementation ---

// --- Iterator Implementation ---

cdsa_queue_iterator *cdsa_create_queue_iterator(cdsa_queue *q) {
  if (q == NULL)
    return NULL;
  // Route directly to the underlying cdsa_ringbuffer
  return (cdsa_queue_iterator *)cdsa_create_ringbuffer_iterator(q->rb);
}

bool cdsa_has_next_queue(cdsa_queue_iterator *iter) {
  return cdsa_has_next_ringbuffer((cdsa_ringbuffer_iterator *)iter);
}

CDSA_STATUS cdsa_next_queue(cdsa_queue_iterator *iter, void **out_value) {
  return cdsa_next_ringbuffer((cdsa_ringbuffer_iterator *)iter, out_value);
}

void cdsa_free_queue_iterator(cdsa_queue_iterator *iter) {
  cdsa_free_ringbuffer_iterator((cdsa_ringbuffer_iterator *)iter);
}

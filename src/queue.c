#include "CDSA/queue.h"
#include "CDSA/ringbuffer.h"
#include <stdlib.h>
#include <string.h>

struct Queue {
  RingBuffer *rb;
};

Queue *create_queue(size_t capacity, size_t elem_size) {
  Queue *queue = malloc(sizeof(Queue));
  if (queue == NULL) {
    return NULL;
  }

  queue->rb = create_ringbuffer(capacity, elem_size);

  if (queue->rb == NULL) {
    free(queue);
    return NULL;
  }

  return queue;
}

void free_queue(Queue *queue) {
  if (queue == NULL)
    return;

  free_ringbuffer(queue->rb);
  free(queue);
}

bool enqueue(Queue *queue, void *elem) {
  // Enqueue is just a restricted push_back
  return push_back_ringbuffer(queue->rb, elem);
}

void dequeue(Queue *queue) {
  // Dequeue is just a restricted pop_front
  pop_front_ringbuffer(queue->rb);
}

void *front_queue(Queue *queue) { return front_ringbuffer(queue->rb); }

// --- Utilities ---

size_t size_queue(Queue *queue) { return size_ringbuffer(queue->rb); }

bool is_empty_queue(Queue *queue) { return is_empty_ringbuffer(queue->rb); }

bool is_full_queue(Queue *queue) { return is_full_ringbuffer(queue->rb); }

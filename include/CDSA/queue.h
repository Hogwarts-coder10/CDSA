#ifndef QUEUE_H
#define QUEUE_H

#include "CDSA/ringbuffer.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct {
  RingBuffer *rb;
} Queue;

Queue *create_queue(size_t capacity, size_t elem_size);
void free_queue(Queue *queue);

bool enqueue(Queue *queue, void *elem);
void dequeue(Queue *queue);
void *front_queue(Queue *queue);

size_t size_queue(Queue *queue);
bool is_empty_queue(Queue *queue);
bool is_full_queue(Queue *queue);

#endif

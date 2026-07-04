#ifndef QUEUE_H
#define QUEUE_H

#include "CDSA/error.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct Queue Queue;

Queue *create_queue(size_t capacity, size_t elem_size);
void free_queue(Queue *queue);

CDSA_STATUS enqueue(Queue *queue, void *elem);
CDSA_STATUS dequeue(Queue *queue);
void *front_queue(Queue *queue);

size_t size_queue(Queue *queue);
bool is_empty_queue(Queue *queue);
bool is_full_queue(Queue *queue);

#endif

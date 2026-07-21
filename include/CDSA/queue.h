#ifndef QUEUE_H
#define QUEUE_H

#include "CDSA/error.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct Queue Queue;

Queue *create_queue(size_t capacity, size_t elem_size);
void free_queue(Queue *queue);
/**
 * @brief Pushes an element into the collection.
 *
 * @ownership
 * - VALUE: The library creates a shallow, byte-for-byte copy using memcpy
 *   based on the collection's configured elem_size.
 * - MEMORY: If the element is a pointer to dynamically allocated memory,
 *   the caller retains ownership of that underlying memory and must free it.
 */

CDSA_STATUS enqueue(Queue *queue, void *elem);
CDSA_STATUS dequeue(Queue *queue);
void *front_queue(Queue *queue);

size_t size_queue(Queue *queue);
bool is_empty_queue(Queue *queue);
bool is_full_queue(Queue *queue);

// --- Iterator API ---

typedef struct QueueIterator QueueIterator;

QueueIterator *create_queue_iterator(Queue *q);
bool has_next_queue(QueueIterator *iter);
CDSA_STATUS next_queue(QueueIterator *iter, void **out_value);
void free_queue_iterator(QueueIterator *iter);

#endif

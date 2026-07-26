#ifndef QUEUE_H
#define QUEUE_H

#include "CDSA/error.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct cdsa_queue cdsa_queue;

cdsa_queue *cdsa_create_queue(cdsa_size_t capacity, cdsa_size_t elem_size);
void cdsa_free_queue(cdsa_queue *queue);
/**
 * @brief Pushes an element into the collection.
 *
 * @ownership
 * - VALUE: The library creates a shallow, byte-for-byte copy using memcpy
 *   based on the collection's configured elem_size.
 * - MEMORY: If the element is a pointer to dynamically allocated memory,
 *   the caller retains ownership of that underlying memory and must free it.
 */

CDSA_STATUS cdsa_enqueue(cdsa_queue *queue, void *elem);
CDSA_STATUS cdsa_dequeue(cdsa_queue *queue);
void *cdsa_front_queue(cdsa_queue *queue);

cdsa_size_t cdsa_size_queue(cdsa_queue *queue);
bool cdsa_is_empty_queue(cdsa_queue *queue);
bool cdsa_is_full_queue(cdsa_queue *queue);

// --- Iterator API ---

typedef struct cdsa_queue_iterator cdsa_queue_iterator;

cdsa_queue_iterator *cdsa_create_queue_iterator(cdsa_queue *q);
bool cdsa_has_next_queue(cdsa_queue_iterator *iter);
CDSA_STATUS cdsa_next_queue(cdsa_queue_iterator *iter, void **out_value);
void cdsa_free_queue_iterator(cdsa_queue_iterator *iter);

#endif

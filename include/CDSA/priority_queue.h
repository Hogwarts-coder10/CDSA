#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "CDSA/error.h"
#include <stdbool.h>
#include <stddef.h>

typedef bool (*PriorityCompareFn)(void *a, void *b);

typedef struct cdsa_priority_queue cdsa_priority_queue;

// --- LifeCycle ---
cdsa_priority_queue *cdsa_create_pq(size_t elem_size, PriorityCompareFn cmp_func);
void cdsa_free_pq(cdsa_priority_queue *pq);

// --- Operations ---
/**
 * @brief Pushes an element into the priority queue and sifts it to the correct
 * position.
 * * @ownership
 * - VALUE: The library creates a shallow, byte-for-byte copy using memcpy
 * based on the queue's configured elem_size (managed by the underlying cdsa_vector).
 * - MEMORY: If the element is a pointer to dynamically allocated memory,
 * the caller retains ownership of that underlying memory and must free it
 * after popping it or before destroying the queue.
 */
CDSA_STATUS cdsa_push_pq(cdsa_priority_queue *pq, void *elem);

// Copies the highest priority element into 'out_elem' and removes it.
// Returns false if the queue is empty.
CDSA_STATUS cdsa_pop_pq(cdsa_priority_queue *pq, void *out_elem);

// Returns a pointer to the highest priority element without removing it.
void *cdsa_peek_pq(const cdsa_priority_queue *pq);

size_t cdsa_size_pq(const cdsa_priority_queue *pq);
bool cdsa_is_empty_pq(const cdsa_priority_queue *pq);
void cdsa_clear_pq(cdsa_priority_queue *pq);

// --- Iterator API ---

// Typedef directly to the cdsa_vector_iterator
typedef struct cdsa_vector_iterator cdsa_priority_queue_iterator;

/**
 * @brief Creates a new iterator for the Priority cdsa_queue.
 * @warning The caller must free the iterator using
 * cdsa_free_priority_queue_iterator.
 * @note Elements are yielded in heap-array order (level-order), NOT strictly
 * sorted order.
 */
cdsa_priority_queue_iterator *cdsa_create_priority_queue_iterator(const cdsa_priority_queue *pq);

bool cdsa_has_next_priority_queue(cdsa_priority_queue_iterator *iter);

CDSA_STATUS cdsa_next_priority_queue(cdsa_priority_queue_iterator *iter, void **out_value);

void cdsa_free_priority_queue_iterator(cdsa_priority_queue_iterator *iter);

#endif

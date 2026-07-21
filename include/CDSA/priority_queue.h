#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "CDSA/error.h"
#include <stdbool.h>
#include <stddef.h>

typedef bool (*PriorityCompareFn)(void *a, void *b);

typedef struct PriorityQueue PriorityQueue;

// --- LifeCycle ---
PriorityQueue *create_pq(size_t elem_size, PriorityCompareFn cmp_func);
void free_pq(PriorityQueue *pq);

// --- Operations ---
/**
 * @brief Pushes an element into the priority queue and sifts it to the correct
 * position.
 * * @ownership
 * - VALUE: The library creates a shallow, byte-for-byte copy using memcpy
 * based on the queue's configured elem_size (managed by the underlying Vector).
 * - MEMORY: If the element is a pointer to dynamically allocated memory,
 * the caller retains ownership of that underlying memory and must free it
 * after popping it or before destroying the queue.
 */
CDSA_STATUS push_pq(PriorityQueue *pq, void *elem);

// Copies the highest priority element into 'out_elem' and removes it.
// Returns false if the queue is empty.
CDSA_STATUS pop_pq(PriorityQueue *pq, void *out_elem);

// Returns a pointer to the highest priority element without removing it.
void *peek_pq(PriorityQueue *pq);

size_t size_pq(PriorityQueue *pq);
bool is_empty_pq(PriorityQueue *pq);
void clear_pq(PriorityQueue *pq);

// --- Iterator API ---

// Typedef directly to the VectorIterator
typedef struct VectorIterator PriorityQueueIterator;

/**
 * @brief Creates a new iterator for the Priority Queue.
 * @warning The caller must free the iterator using
 * free_priority_queue_iterator.
 * @note Elements are yielded in heap-array order (level-order), NOT strictly
 * sorted order.
 */
PriorityQueueIterator *create_priority_queue_iterator(PriorityQueue *pq);

bool has_next_priority_queue(PriorityQueueIterator *iter);

CDSA_STATUS next_priority_queue(PriorityQueueIterator *iter, void **out_value);

void free_priority_queue_iterator(PriorityQueueIterator *iter);

#endif

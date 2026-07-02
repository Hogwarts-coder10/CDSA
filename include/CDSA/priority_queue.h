#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <stdbool.h>
#include <stddef.h>

typedef bool (*PriorityCompareFn)(void *a, void *b);

typedef struct PriorityQueue PriorityQueue;

// --- LifeCycle ---
PriorityQueue *create_pq(size_t elem_size, PriorityCompareFn cmp_func);
void free_pq(PriorityQueue *pq);

// --- Operations ---
void push_pq(PriorityQueue *pq, void *elem);

// Copies the highest priority element into 'out_elem' and removes it.
// Returns false if the queue is empty.
bool pop_pq(PriorityQueue *pq, void *out_elem);

// Returns a pointer to the highest priority element without removing it.
void *peek_pq(PriorityQueue *pq);

size_t size_pq(PriorityQueue *pq);
bool is_empty_pq(PriorityQueue *pq);
void clear_pq(PriorityQueue *pq);

#endif

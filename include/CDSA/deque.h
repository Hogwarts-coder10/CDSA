#ifndef DEQUE_H
#define DEQUE_H

#include "CDSA/error.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct Deque Deque;

// -- Core LifeCycle
Deque *create_deque(size_t capacity, size_t elem_size);
void free_deque(Deque *dequeue);

// --- Front Operations ---
/**
 * @brief Pushes an element into the deque (applies to front and back).
 * * @ownership
 * - VALUE: The library creates a shallow, byte-for-byte copy using memcpy
 * based on the deque's configured elem_size (managed by the underlying
 * RingBuffer).
 * - MEMORY: If the element is a pointer to dynamically allocated memory,
 * the caller retains ownership of that underlying memory and must free it
 * after popping it or before destroying the deque.
 */
CDSA_STATUS push_front_deque(Deque *deque, void *elem);
CDSA_STATUS pop_front_deque(Deque *deque);
void *front_deque(Deque *deque);

// --- Back Operations ---
CDSA_STATUS push_back_deque(Deque *deque, void *elem);
CDSA_STATUS pop_back_deque(Deque *deque);
void *back_deque(Deque *deque);

// --- Utilities ---
size_t size_deque(Deque *deque);
bool is_empty_deque(Deque *deque);
bool is_full_deque(Deque *deque);

// --- Iterator API ---

typedef struct DequeIterator DequeIterator;

DequeIterator *create_deque_iterator(Deque *d);
bool has_next_deque(DequeIterator *iter);
CDSA_STATUS next_deque(DequeIterator *iter, void **out_value);
void free_deque_iterator(DequeIterator *iter);

#endif

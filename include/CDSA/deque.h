#ifndef DEQUE_H
#define DEQUE_H

#include "CDSA/error.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct cdsa_deque cdsa_deque;

// -- Core LifeCycle
cdsa_deque *cdsa_create_deque(size_t capacity, size_t elem_size);
void cdsa_free_deque(cdsa_deque *cdsa_dequeue);

// --- Front Operations ---
/**
 * @brief Pushes an element into the deque (applies to front and back).
 * * @ownership
 * - VALUE: The library creates a shallow, byte-for-byte copy using memcpy
 * based on the deque's configured elem_size (managed by the underlying
 * cdsa_ringbuffer).
 * - MEMORY: If the element is a pointer to dynamically allocated memory,
 * the caller retains ownership of that underlying memory and must free it
 * after popping it or before destroying the deque.
 */
CDSA_STATUS cdsa_push_front_deque(cdsa_deque *deque, void *elem);
CDSA_STATUS cdsa_pop_front_deque(cdsa_deque *deque);
void *cdsa_front_deque(const cdsa_deque *deque);

// --- Back Operations ---
CDSA_STATUS cdsa_push_back_deque(cdsa_deque *deque, void *elem);
CDSA_STATUS cdsa_pop_back_deque(cdsa_deque *deque);
void *cdsa_back_deque(const cdsa_deque *deque);

// --- Utilities ---
size_t cdsa_size_deque(const cdsa_deque *deque);
bool cdsa_is_empty_deque(const cdsa_deque *deque);
bool cdsa_is_full_deque(const cdsa_deque *deque);

// --- Iterator API ---

typedef struct cdsa_deque_iterator cdsa_deque_iterator;

cdsa_deque_iterator *cdsa_create_deque_iterator(const cdsa_deque *d);
bool cdsa_has_next_deque(cdsa_deque_iterator *iter);
CDSA_STATUS cdsa_next_deque(cdsa_deque_iterator *iter, void **out_value);
void cdsa_free_deque_iterator(cdsa_deque_iterator *iter);

#endif

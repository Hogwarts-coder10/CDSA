#ifndef DEQUE_H
#define DEQUE_H

#include "CDSA/ringbuffer.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct {
  RingBuffer *rb;
} Deque;

// -- Core LifeCycle
Deque *create_deque(size_t capacity, size_t elem_size);
void free_deque(Deque *dequeue);

// --- Front Operations ---
bool push_front(Deque *deque, void *elem);
void pop_front(Deque *deque);
void *front_deque(Deque *deque);

// --- Back Operations ---
bool push_back(Deque *deque, void *elem);
void pop_back(Deque *deque);
void *back_deque(Deque *deque);

// --- Utilities ---
size_t size_deque(Deque *deque);
bool is_empty_deque(Deque *deque);
bool is_full_deque(Deque *deque);

#endif

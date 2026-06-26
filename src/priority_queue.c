#include "CDSA/priority_queue.h"
#include <stdlib.h>
#include <string.h>

// --- Internal Helpers: Heap Math ---

static void sift_up(PriorityQueue *pq, size_t index) {
  if (index == 0)
    return;

  void *temp = malloc(pq->elem_size);

  while (index > 0) {
    size_t parent_idx = (index - 1) / 2;

    void *current_val = get_vector(pq->data, index);
    void *parent_val = get_vector(pq->data, parent_idx);

    // If current has higher priority, swap them!
    if (pq->cmp(current_val, parent_val)) {
      memcpy(temp, current_val, pq->elem_size);
      set_vector(pq->data, index, parent_val);
      set_vector(pq->data, parent_idx, temp);

      index = parent_idx;
    } else {
      break;
    }
  }

  free(temp);
}

static void sift_down(PriorityQueue *pq, size_t index) {
  size_t size = size_vector(pq->data);
  void *temp = malloc(pq->elem_size);

  while (true) {
    size_t left_child = 2 * index + 1;
    size_t right_child = 2 * index + 2;
    size_t highest_priority = index;

    if (left_child < size) {
      void *left_val = get_vector(pq->data, left_child);
      void *target_val = get_vector(pq->data, highest_priority);
      if (pq->cmp(left_val, target_val)) {
        highest_priority = left_child;
      }
    }

    if (right_child < size) {
      void *right_val = get_vector(pq->data, right_child);
      void *target_val = get_vector(pq->data, highest_priority);
      if (pq->cmp(right_val, target_val)) {
        highest_priority = right_child;
      }
    }

    if (highest_priority != index) {
      void *current_val = get_vector(pq->data, index);
      void *target_val = get_vector(pq->data, highest_priority);

      // Swap
      memcpy(temp, current_val, pq->elem_size);
      set_vector(pq->data, index, target_val);
      set_vector(pq->data, highest_priority, temp);

      index = highest_priority;
    } else {
      break;
    }
  }

  free(temp);
}

// --- LifeCycle ---

PriorityQueue *create_pq(size_t elem_size, PriorityCompareFn cmp_func) {
  PriorityQueue *pq = malloc(sizeof(PriorityQueue));
  pq->data = create_vector(elem_size);
  pq->elem_size = elem_size;
  pq->cmp = cmp_func;
  return pq;
}

void free_pq(PriorityQueue *pq) {
  if (pq == NULL)
    return;
  free_vector(pq->data);
  free(pq);
}

// --- Operations ---

void push_pq(PriorityQueue *pq, void *elem) {
  push_vector(pq->data, elem);
  sift_up(pq, size_vector(pq->data) - 1);
}

bool pop_pq(PriorityQueue *pq, void *out_elem) {
  if (is_empty_pq(pq))
    return false;

  // 1. Copy the highest priority item to the user's out parameter
  memcpy(out_elem, front_vector(pq->data), pq->elem_size);

  // 2. Overwrite the root with the very last element in the vector
  void *last_val = back_vector(pq->data);
  set_vector(pq->data, 0, last_val);

  // 3. Remove the last element
  pop_vector(pq->data);

  // 4. Sink the new root down to restore the heap property
  if (!is_empty_pq(pq)) {
    sift_down(pq, 0);
  }

  return true;
}

void *peek_pq(PriorityQueue *pq) { return front_vector(pq->data); }

size_t size_pq(PriorityQueue *pq) { return size_vector(pq->data); }

bool is_empty_pq(PriorityQueue *pq) { return is_empty_vector(pq->data); }

void clear_pq(PriorityQueue *pq) { clear_vector(pq->data); }

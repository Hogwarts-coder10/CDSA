#include "CDSA/priority_queue.h"
#include "CDSA/allocator.h"
#include "CDSA/error.h"
#include "CDSA/vector.h"
#include <stdlib.h>
#include <string.h>

struct cdsa_priority_queue {
  cdsa_vector *data;
  size_t elem_size;
  PriorityCompareFn cmp;
};

// --- Internal Helpers: Heap Math ---

static CDSA_STATUS sift_up(cdsa_priority_queue *pq, size_t index) {
  if (index == 0)
    return CDSA_OK;

  void *temp = CDSA_MALLOC(pq->elem_size);

  if (temp == NULL) {
    return CDSA_ERR_OOM; // Catch the memory failure
  }

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

  CDSA_FREE(temp);
  return CDSA_OK;
}

static CDSA_STATUS sift_down(cdsa_priority_queue *pq, size_t index) {
  size_t size = cdsa_size_vector(pq->data);
  void *temp = CDSA_MALLOC(pq->elem_size);

  if (temp == NULL) {
    return CDSA_ERR_OOM; // Catch the memory failure
  }

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

  CDSA_FREE(temp);
  return CDSA_OK;
}

// --- LifeCycle ---

cdsa_priority_queue *cdsa_create_pq(size_t elem_size, PriorityCompareFn cmp_func) {
  cdsa_priority_queue *pq = CDSA_MALLOC(sizeof(cdsa_priority_queue));

  if (pq == NULL) {
    return NULL;
  }

  pq->data = cdsa_create_vector(elem_size);

  if (pq->data == NULL) {
    CDSA_FREE(pq);
    return NULL;
  }
  pq->elem_size = elem_size;
  pq->cmp = cmp_func;
  return pq;
}

void cdsa_free_pq(cdsa_priority_queue *pq) {
  if (pq == NULL)
    return;
  cdsa_free_vector(pq->data);
  CDSA_FREE(pq);
}

// --- Operations ---

CDSA_STATUS cdsa_push_pq(cdsa_priority_queue *pq, void *elem) {
  if (pq == NULL || elem == NULL) {
    return CDSA_ERR_INVALID;
  }

  // Push vector handles its own OOM check
  CDSA_STATUS cdsa_push_status = cdsa_push_vector(pq->data, elem);
  if (cdsa_push_status != CDSA_OK) {
    return cdsa_push_status;
  }

  return sift_up(pq, cdsa_size_vector(pq->data) - 1);
}

CDSA_STATUS cdsa_pop_pq(cdsa_priority_queue *pq, void *out_elem) {
  if (pq == NULL || out_elem == NULL) {
    return CDSA_ERR_INVALID;
  }

  if (cdsa_is_empty_pq(pq)) {
    return CDSA_ERR_EMPTY;
  }

  // 1. Copy the highest priority item to the user's out parameter
  memcpy(out_elem, cdsa_front_vector(pq->data), pq->elem_size);

  // 2. Overwrite the root with the very last element in the vector
  void *last_val = cdsa_back_vector(pq->data);
  set_vector(pq->data, 0, last_val);

  // 3. Remove the last element
  cdsa_pop_vector(pq->data);

  // 4. Sink the new root down to restore the heap property
  if (!cdsa_is_empty_pq(pq)) {
    return sift_down(pq, 0);
  }

  return CDSA_OK;
}

void *cdsa_peek_pq(const cdsa_priority_queue *pq) {
  if (pq == NULL)
    return NULL;
  return cdsa_front_vector(pq->data);
}

size_t cdsa_size_pq(const cdsa_priority_queue *pq) {
  if (pq == NULL)
    return 0;
  return cdsa_size_vector(pq->data);
}

bool cdsa_is_empty_pq(const cdsa_priority_queue *pq) {
  if (pq == NULL)
    return true;
  return cdsa_is_empty_vector(pq->data);
}

void cdsa_clear_pq(cdsa_priority_queue *pq) {
  if (pq == NULL)
    return;
  cdsa_clear_vector(pq->data);
}

// --- Iterator Implementation ---

cdsa_priority_queue_iterator *cdsa_create_priority_queue_iterator(const cdsa_priority_queue *pq) {
  if (pq == NULL)
    return NULL;

  // Route directly to the underlying cdsa_vector iterator
  return (cdsa_priority_queue_iterator *)cdsa_create_vector_iterator(pq->data);
}

bool cdsa_has_next_priority_queue(cdsa_priority_queue_iterator *iter) {
  return cdsa_has_next_vector((cdsa_vector_iterator *)iter);
}

CDSA_STATUS cdsa_next_priority_queue(cdsa_priority_queue_iterator *iter, void **out_value) {
  return cdsa_next_vector((cdsa_vector_iterator *)iter, out_value);
}

void cdsa_free_priority_queue_iterator(cdsa_priority_queue_iterator *iter) {
  cdsa_free_vector_iterator((cdsa_vector_iterator *)iter);
}

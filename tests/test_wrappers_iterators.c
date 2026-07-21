#include "CDSA/deque.h"
#include "CDSA/error.h"
#include "CDSA/priority_queue.h"
#include "CDSA/queue.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

// --- 1. Queue Test ---
void test_queue_iterator() {
  printf("--- Testing Queue Iterator ---\n");

  Queue *q = create_queue(10, sizeof(int));

  for (int i = 1; i <= 3; i++) {
    enqueue(q, &i); // Updated to use enqueue[cite: 6]
  }

  QueueIterator *iter = create_queue_iterator(q);
  void *value_ptr;
  int expected[] = {1, 2, 3};
  int count = 0;

  while (has_next_queue(iter)) {
    assert(next_queue(iter, &value_ptr) == CDSA_OK);
    int val = *(int *)value_ptr;
    printf("  Queue Element: %d (Expected: %d)\n", val, expected[count]);
    assert(val == expected[count]);
    count++;
  }
  assert(count == 3);

  // Mutation Guard Test
  int extra = 99;
  enqueue(q, &extra); // Updated to use enqueue[cite: 6]
  assert(has_next_queue(iter) == false);
  assert(next_queue(iter, &value_ptr) == CDSA_ERR_ITER_INVALIDATED);

  free_queue_iterator(iter);
  free_queue(q);
  printf("Queue Iterator: PASS\n\n");
}

// --- 2. Deque Test ---
void test_deque_iterator() {
  printf("--- Testing Deque Iterator ---\n");

  Deque *d = create_deque(10, sizeof(int));

  // Push to front and back to test iteration order
  int v1 = 20, v2 = 10, v3 = 30;
  push_back_deque(d, &v1);
  push_front_deque(d, &v2);
  push_back_deque(d, &v3);

  DequeIterator *iter = create_deque_iterator(d);
  void *value_ptr;
  int expected[] = {10, 20, 30};
  int count = 0;

  while (has_next_deque(iter)) {
    assert(next_deque(iter, &value_ptr) == CDSA_OK);
    int val = *(int *)value_ptr;
    printf("  Deque Element: %d (Expected: %d)\n", val, expected[count]);
    assert(val == expected[count]);
    count++;
  }
  assert(count == 3);

  // Mutation Guard Test
  pop_front_deque(d);
  assert(has_next_deque(iter) == false);
  assert(next_deque(iter, &value_ptr) == CDSA_ERR_ITER_INVALIDATED);

  free_deque_iterator(iter);
  free_deque(d);
  printf("Deque Iterator: PASS\n\n");
}

// --- 3. Priority Queue Test ---
// Updated to match PriorityCompareFn which returns bool[cite: 7]
bool compare_ints_max(void *a, void *b) {
  int int_a = *(int *)a;
  int int_b = *(int *)b;
  return int_a > int_b;
}

void test_priority_queue_iterator() {
  printf("--- Testing Priority Queue Iterator ---\n");

  // Updated to use create_pq and its 2-argument signature[cite: 7]
  PriorityQueue *pq = create_pq(sizeof(int), compare_ints_max);

  int vals[] = {15, 30, 5, 20};
  for (int i = 0; i < 4; i++) {
    push_pq(pq, &vals[i]); // Updated to use push_pq[cite: 7]
  }

  PriorityQueueIterator *iter = create_priority_queue_iterator(pq);
  void *value_ptr;
  int count = 0;

  printf("  Iterating underlying heap array level-order:\n");
  while (has_next_priority_queue(iter)) {
    assert(next_priority_queue(iter, &value_ptr) == CDSA_OK);
    int val = *(int *)value_ptr;
    printf("  PQ Element: %d\n", val);
    count++;
  }
  assert(count == 4);

  // Mutation Guard Test
  int extra = 50;
  push_pq(pq, &extra); // Updated to use push_pq[cite: 7]
  assert(has_next_priority_queue(iter) == false);
  assert(next_priority_queue(iter, &value_ptr) == CDSA_ERR_ITER_INVALIDATED);

  free_priority_queue_iterator(iter);
  free_pq(pq); // Updated to use free_pq[cite: 7]
  printf("Priority Queue Iterator: PASS\n\n");
}

int main(void) {
  printf("\n=== RUNNING WRAPPER STRUCT ITERATOR TESTS ===\n\n");

  test_queue_iterator();
  test_deque_iterator();
  test_priority_queue_iterator();

  printf("=== ALL WRAPPER TESTS PASSED ===\n\n");
  return 0;
}

#include "CDSA/deque.h"
#include "CDSA/error.h"
#include "CDSA/priority_queue.h"
#include "CDSA/queue.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

// --- 1. cdsa_queue Test ---
void test_queue_iterator() {
  printf("--- Testing cdsa_queue Iterator ---\n");

  cdsa_queue *q = cdsa_create_queue(10, sizeof(int));

  for (int i = 1; i <= 3; i++) {
    cdsa_enqueue(q, &i);
  }

  cdsa_queue_iterator *iter = cdsa_create_queue_iterator(q);
  void *value_ptr;
  int expected[] = {1, 2, 3};
  int count = 0;

  while (cdsa_has_next_queue(iter)) {
    int status = cdsa_next_queue(iter, &value_ptr);
    assert(status == CDSA_OK);
    int val = *(int *)value_ptr;
    printf("  cdsa_queue Element: %d (Expected: %d)\n", val, expected[count]);
    assert(val == expected[count]);
    count++;
  }
  assert(count == 3);

  // Mutation Guard Test
  int extra = 99;
  cdsa_enqueue(q, &extra);
  bool has_next = cdsa_has_next_queue(iter);
  assert(has_next == false);
  int mut_status = cdsa_next_queue(iter, &value_ptr);
  assert(mut_status == CDSA_ERR_ITER_INVALIDATED);

  cdsa_free_queue_iterator(iter);
  cdsa_free_queue(q);
  printf("cdsa_queue Iterator: PASS\n\n");
}

// --- 2. cdsa_deque Test ---
void test_deque_iterator() {
  printf("--- Testing cdsa_deque Iterator ---\n");

  cdsa_deque *d = cdsa_create_deque(10, sizeof(int));

  int v1 = 20, v2 = 10, v3 = 30;
  cdsa_push_back_deque(d, &v1);
  cdsa_push_front_deque(d, &v2);
  cdsa_push_back_deque(d, &v3);

  cdsa_deque_iterator *iter = cdsa_create_deque_iterator(d);
  void *value_ptr;
  int expected[] = {10, 20, 30};
  int count = 0;

  while (cdsa_has_next_deque(iter)) {
    int status = cdsa_next_deque(iter, &value_ptr);
    assert(status == CDSA_OK);
    int val = *(int *)value_ptr;
    printf("  cdsa_deque Element: %d (Expected: %d)\n", val, expected[count]);
    assert(val == expected[count]);
    count++;
  }
  assert(count == 3);

  // Mutation Guard Test
  cdsa_pop_front_deque(d);
  bool has_next = cdsa_has_next_deque(iter);
  assert(has_next == false);
  int mut_status = cdsa_next_deque(iter, &value_ptr);
  assert(mut_status == CDSA_ERR_ITER_INVALIDATED);

  cdsa_free_deque_iterator(iter);
  cdsa_free_deque(d);
  printf("cdsa_deque Iterator: PASS\n\n");
}

// --- 3. Priority cdsa_queue Test ---
bool compare_ints_max(void *a, void *b) {
  int int_a = *(int *)a;
  int int_b = *(int *)b;
  return int_a > int_b;
}

void test_priority_queue_iterator() {
  printf("--- Testing Priority cdsa_queue Iterator ---\n");

  cdsa_priority_queue *pq = cdsa_create_pq(sizeof(int), compare_ints_max);

  int vals[] = {15, 30, 5, 20};
  for (int i = 0; i < 4; i++) {
    cdsa_push_pq(pq, &vals[i]);
  }

  cdsa_priority_queue_iterator *iter = cdsa_create_priority_queue_iterator(pq);
  void *value_ptr;
  int count = 0;

  printf("  Iterating underlying heap array level-order:\n");
  while (cdsa_has_next_priority_queue(iter)) {
    int status = cdsa_next_priority_queue(iter, &value_ptr);
    assert(status == CDSA_OK);
    int val = *(int *)value_ptr;
    printf("  PQ Element: %d\n", val);
    count++;
  }
  assert(count == 4);

  // Mutation Guard Test
  int extra = 50;
  cdsa_push_pq(pq, &extra);
  bool has_next = cdsa_has_next_priority_queue(iter);
  assert(has_next == false);
  int mut_status = cdsa_next_priority_queue(iter, &value_ptr);
  assert(mut_status == CDSA_ERR_ITER_INVALIDATED);

  cdsa_free_priority_queue_iterator(iter);
  cdsa_free_pq(pq);
  printf("Priority cdsa_queue Iterator: PASS\n\n");
}

int main(void) {
  printf("\n=== RUNNING WRAPPER STRUCT ITERATOR TESTS ===\n\n");

  test_queue_iterator();
  test_deque_iterator();
  test_priority_queue_iterator();

  printf("=== ALL WRAPPER TESTS PASSED ===\n\n");
  return 0;
}

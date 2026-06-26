#include "CDSA/priority_queue.h"
#include <stdio.h>
#include <stdlib.h>

// --- Comparators ---

// MIN-HEAP: Returns true if a < b (Smallest numbers bubble to the top)
bool min_compare(void *a, void *b) {
  int val_a = *(int *)a;
  int val_b = *(int *)b;
  return val_a < val_b;
}

// MAX-HEAP: Returns true if a > b (Largest numbers bubble to the top)
bool max_compare(void *a, void *b) {
  int val_a = *(int *)a;
  int val_b = *(int *)b;
  return val_a > val_b;
}

int main() {
  printf("==========================================\n");
  printf("⛰️ PRIORITY QUEUE TEST ⛰️\n");
  printf("==========================================\n\n");

  int nums[] = {45, 12, 85, 32, 89, 39, 69, 44, 42, 1, 58, 9};
  int num_count = sizeof(nums) / sizeof(nums[0]);

  // --- Test 1: Min-Heap ---
  printf("--- Phase 1: Min-Heap ---\n");
  // Notice we pass sizeof(int) so the queue knows how big our data blocks are!
  PriorityQueue *min_pq = create_pq(sizeof(int), min_compare);

  for (int i = 0; i < num_count; i++) {
    push_pq(min_pq, &nums[i]);
  }

  printf("Extracted (should be ascending): ");
  int out_val;
  while (!is_empty_pq(min_pq)) {
    // Pop safely copies the value into our out_val variable
    if (pop_pq(min_pq, &out_val)) {
      printf("%d ", out_val);
    }
  }
  printf("\n\n");
  free_pq(min_pq);

  // --- Test 2: Max-Heap ---
  printf("--- Phase 2: Max-Heap ---\n");
  PriorityQueue *max_pq = create_pq(sizeof(int), max_compare);

  for (int i = 0; i < num_count; i++) {
    push_pq(max_pq, &nums[i]);
  }

  printf("Extracted (should be descending): ");
  while (!is_empty_pq(max_pq)) {
    if (pop_pq(max_pq, &out_val)) {
      printf("%d ", out_val);
    }
  }
  printf("\n\n");
  free_pq(max_pq);

  printf("✅ Tests complete. Run Valgrind to verify!\n");
  return 0;
}

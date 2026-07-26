#include "CDSA/error.h"
#include "CDSA/ringbuffer.h"
#include <assert.h>
#include <stdio.h>

void test_ringbuffer_iterator() {
  printf("\n=== Testing cdsa_ringbuffer Iterator ===\n");

  // 1. Create a ringbuffer with a capacity of 4
  cdsa_ringbuffer *rb = cdsa_create_ringbuffer(10, sizeof(int));

  // 2. Fill it, then pop some to shift the head (causing a future wrap-around)
  int val;
  for (int i = 1; i <= 3; i++) {
    cdsa_push_back_ringbuffer(rb, &i); // Adds 1, 2, 3
  }

  // Dequeue one element (1 is gone, head is now at index 1)
  cdsa_pop_front_ringbuffer(rb);

  // Add more elements to trigger the wrap-around!
  int extra1 = 4;
  int extra2 = 5;
  cdsa_push_back_ringbuffer(rb, &extra1);
  cdsa_push_back_ringbuffer(rb, &extra2); // Data is now circularly wrapped: [5, _,
                                     // 2, 3, 4] (conceptually)

  // 3. Iterate
  cdsa_ringbuffer_iterator *iter = cdsa_create_ringbuffer_iterator(rb);
  void *value_ptr;
  int expected[] = {2, 3, 4, 5};
  int count = 0;

  printf("Iterating wrapped cdsa_ringbuffer contents:\n");
  while (cdsa_has_next_ringbuffer(iter)) {
    CDSA_STATUS status = cdsa_next_ringbuffer(iter, &value_ptr);
    assert(status == CDSA_OK);

    int val = *(int *)value_ptr;
    printf("  -> Element: %d (Expected: %d)\n", val, expected[count]);
    assert(val == expected[count]);
    count++;
  }
  assert(count == 4);

  // 4. Test Mutation Guard
  printf("Simulating cdsa_enqueue mid-iteration to trigger mutation guard...\n");
  int extra3 = 6;
  cdsa_push_back_ringbuffer(rb, &extra3); // Bump version!

  // The iterator must now fail-fast
  assert(cdsa_has_next_ringbuffer(iter) == false);
  assert(cdsa_next_ringbuffer(iter, &value_ptr) == CDSA_ERR_ITER_INVALIDATED);

  // Cleanup
  cdsa_free_ringbuffer_iterator(iter);
  cdsa_free_ringbuffer(rb);

  printf("=== cdsa_ringbuffer Iterator Test Complete ===\n\n");
}

int main(void) {
  test_ringbuffer_iterator();
  return 0;
}

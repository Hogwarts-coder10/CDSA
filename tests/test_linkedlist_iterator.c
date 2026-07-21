#include "CDSA/error.h"
#include "CDSA/linkedlist.h"
#include <assert.h>
#include <stdio.h>

void test_linkedlist_iterator() {
  printf("\n=== Testing LinkedList Iterator ===\n");

  // 1. Create the list
  LinkedList *list = create_linkedlist(sizeof(int));

  // 2. Populate it using push_front.
  // Pushing 1, 2, 3 means the list will be: 3 -> 2 -> 1
  int vals[] = {1, 2, 3};
  for (int i = 0; i < 3; i++) {
    push_front_linkedlist(list, &vals[i]);
  }

  // 3. Iterate
  LinkedListIterator *iter = create_linkedlist_iterator(list);
  void *value_ptr;
  int expected[] = {3, 2, 1};
  int count = 0;

  printf("Iterating LinkedList contents:\n");
  while (has_next_linkedlist(iter)) {
    CDSA_STATUS status = next_linkedlist(iter, &value_ptr);
    assert(status == CDSA_OK);

    int val = *(int *)value_ptr;
    printf("  -> Element: %d (Expected: %d)\n", val, expected[count]);
    assert(val == expected[count]);
    count++;
  }
  assert(count == 3);

  // 4. Test Mutation Guard
  printf("Simulating push mid-iteration to trigger mutation guard...\n");
  int extra = 99;
  push_front_linkedlist(list, &extra); // This must bump list->version!

  // The iterator must now fail-fast
  assert(has_next_linkedlist(iter) == false);
  assert(next_linkedlist(iter, &value_ptr) == CDSA_ERR_ITER_INVALIDATED);

  // Cleanup
  free_linkedlist_iterator(iter);
  free_linkedlist(list);

  printf("=== LinkedList Iterator Test Complete ===\n\n");
}

int main(void) {
  test_linkedlist_iterator();
  return 0;
}

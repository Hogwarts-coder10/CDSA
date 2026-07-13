#include "CDSA/error.h"
#include "CDSA/skiplist.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void test_skiplist_iterator() {
  printf("\n=== Testing SkipList Iterator ===\n");

  // Create the SkipList (adjust arguments if your create_skiplist takes
  // max_level/p)
  SkipList *sl = create_skiplist();

  // 1. Insert data OUT OF ORDER
  insert_skiplist(sl, 42.5, "Douglas");
  insert_skiplist(sl, 10.0, "Alice");
  insert_skiplist(sl, 99.9, "Zebra");
  insert_skiplist(sl, 25.0, "Bob");

  // 2. Read the data using the Iterator
  SkipListIterator *iter = create_skiplist_iterator(sl);
  double score;
  const char *value;
  int count = 0;

  // We'll use this to prove the iterator walks Level 0 in perfectly sorted
  // order
  double last_score = -1.0;

  printf("Iterating SkipList contents (Should be sorted by score):\n");

  while (has_next_skiplist(iter)) {
    CDSA_STATUS status = next_skiplist(iter, &score, &value);
    assert(status == CDSA_OK);

    printf("  -> [%.1f] : %s\n", score, value);

    // Prove the SkipList is doing its job!
    assert(score >= last_score);
    last_score = score;
    count++;
  }

  printf("Total elements seen: %d (Expected: 4)\n", count);
  assert(count == 4);

  free_skiplist_iterator(iter);

  // 3. Free the SkipList
  // (No second cleanup pass needed here since SkipList owns the duplicated
  // strings!)
  free_skiplist(sl);

  printf("=== SkipList Iterator Test Complete ===\n\n");
}

int main(void) {
  test_skiplist_iterator();
  return 0;
}

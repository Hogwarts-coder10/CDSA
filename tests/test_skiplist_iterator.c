#include "CDSA/error.h"
#include "CDSA/skiplist.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void test_skiplist_iterator() {
  printf("\n=== Testing cdsa_skiplist Iterator ===\n");

  // Create the cdsa_skiplist (adjust arguments if your cdsa_create_skiplist takes
  // max_level/p)
  cdsa_skiplist *sl = cdsa_create_skiplist();

  // 1. Insert data OUT OF ORDER
  insert_skiplist(sl, 42.5, "Douglas");
  insert_skiplist(sl, 10.0, "Alice");
  insert_skiplist(sl, 99.9, "Zebra");
  insert_skiplist(sl, 25.0, "Bob");

  // 2. Read the data using the Iterator
  cdsa_skiplist_iterator *iter = cdsa_create_skiplist_iterator(sl);
  double score;
  const char *value;
  int count = 0;

  // We'll use this to prove the iterator walks Level 0 in perfectly sorted
  // order
  double last_score = -1.0;

  printf("Iterating cdsa_skiplist contents (Should be sorted by score):\n");

  while (cdsa_has_next_skiplist(iter)) {
    CDSA_STATUS status = cdsa_next_skiplist(iter, &score, &value);
    assert(status == CDSA_OK);

    printf("  -> [%.1f] : %s\n", score, value);

    // Prove the cdsa_skiplist is doing its job!
    assert(score >= last_score);
    last_score = score;
    count++;
  }

  printf("Total elements seen: %d (Expected: 4)\n", count);
  assert(count == 4);

  cdsa_free_skiplist_iterator(iter);

  // 3. Free the cdsa_skiplist
  // (No second cleanup pass needed here since cdsa_skiplist owns the duplicated
  // strings!)
  cdsa_free_skiplist(sl);

  printf("=== cdsa_skiplist Iterator Test Complete ===\n\n");
}

int main(void) {
  test_skiplist_iterator();
  return 0;
}

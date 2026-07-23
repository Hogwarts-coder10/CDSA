#include "CDSA/art.h"
#include "CDSA/error.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

void test_art_iterator() {
  printf("\n=== Testing ART Iterator ===\n");

  // 1. Create the tree
  ArtTree *tree = create_art();
  assert(tree != NULL);

  // 2. Populate it with string keys and integer values
  char *keys[] = {"apple", "banana", "cherry", "date"};
  int vals[] = {10, 20, 30, 40};

  for (int i = 0; i < 4; i++) {
    assert(insert_art(tree, keys[i], &vals[i]) == CDSA_OK);
  }

  // 3. Iterate over the tree
  ArtIterator *iter = create_art_iterator(tree);
  assert(iter != NULL);

  void *value_ptr;
  int count = 0;

  printf("Iterating ART contents (DFS traversal):\n");
  while (has_next_art(iter)) {
    CDSA_STATUS status = next_art(iter, &value_ptr);
    assert(status == CDSA_OK);

    int val = *(int *)value_ptr;
    printf("  -> Yielded Value: %d\n", val);
    count++;
  }

  // We inserted 4 elements, so the iterator must have yielded exactly 4 times
  assert(count == 4);
  printf("Successfully yielded all %d elements.\n", count);

  // 4. Test Mutation Guard
  printf("Simulating insertion mid-iteration to trigger mutation guard...\n");
  int extra_val = 99;
  insert_art(tree, "elderberry", &extra_val); // This must bump tree->version!

  // The iterator must now fail-fast
  assert(has_next_art(iter) == false);
  assert(next_art(iter, &value_ptr) == CDSA_ERR_ITER_INVALIDATED);
  printf("Mutation guard successfully caught structural modification.\n");

  // Cleanup
  free_art_iterator(iter);
  free_art(tree);

  printf("=== ART Iterator Test Complete ===\n\n");
}

int main(void) {
  test_art_iterator();
  return 0;
}

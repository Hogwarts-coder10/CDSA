#include "CDSA/allocator.h"
#include "CDSA/hashmap.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void test_hashmap_iterator() {
  printf("\n=== Testing cdsa_hashmap Iterator ===\n");

  cdsa_hashmap *map = cdsa_create_hashmap(16);

  // 1. Allocate some values (Remember: The caller owns these!)
  int *val1 = CDSA_MALLOC(sizeof(int));
  *val1 = 100;
  int *val2 = CDSA_MALLOC(sizeof(int));
  *val2 = 200;
  int *val3 = CDSA_MALLOC(sizeof(int));
  *val3 = 300;

  insert_hashmap(map, "Alpha", val1);
  insert_hashmap(map, "Bravo", val2);
  insert_hashmap(map, "Charlie", val3);

  // 2. Pass One: Read the data using the Iterator
  cdsa_hashmap_iterator *iter = cdsa_create_hashmap_iterator(map);
  const char *key;
  void *value;
  int count = 0;

  printf("Iterating map contents:\n");
  while (cdsa_has_next_hashmap(iter)) {
    CDSA_STATUS status = cdsa_next_hashmap(iter, &key, &value);
    assert(status == CDSA_OK);

    printf("  -> %s : %d\n", key, *(int *)value);
    count++;
  }
  printf("Total elements seen: %d (Expected: 3)\n", count);
  assert(count == 3);

  cdsa_free_hashmap_iterator(iter);

  // 3. Pass Two: Use a NEW iterator to safely free the caller-owned values!
  // This proves why the iterator is so powerful for Kedis-C.
  cdsa_hashmap_iterator *cleanup_iter = cdsa_create_hashmap_iterator(map);

  while (cdsa_has_next_hashmap(cleanup_iter)) {
    cdsa_next_hashmap(cleanup_iter, &key, &value);
    CDSA_FREE(value); // Freeing the integer pointers we malloc'd at the start
  }

  cdsa_free_hashmap_iterator(cleanup_iter);

  // 4. Finally, free the map itself
  cdsa_free_hashmap(map);

  printf("=== Iterator Test Complete ===\n\n");
}

void test_iterator_mutation_guard() {
  printf("\n=== Testing Iterator Mutation Guard ===\n");

  // 1. Create a map with a deliberately TINY capacity (e.g., 4)
  // so it's very easy to trigger a resize.
  cdsa_hashmap *map = cdsa_create_hashmap(4);

  // 2. Insert just enough to fill it without resizing
  int *v1 = CDSA_MALLOC(sizeof(int));
  *v1 = 1;
  int *v2 = CDSA_MALLOC(sizeof(int));
  *v2 = 2;
  insert_hashmap(map, "Key1", v1);
  insert_hashmap(map, "Key2", v2);

  // 3. Start iterating
  cdsa_hashmap_iterator *iter = cdsa_create_hashmap_iterator(map);
  const char *key;
  void *value;

  // Read the first element safely
  CDSA_STATUS status = cdsa_next_hashmap(iter, &key, &value);
  assert(status == CDSA_OK);
  printf("Read first key safely: %s\n", key);

  // 4. THE LANDMINE: Simulate another thread or subsystem blasting data in
  printf("Simulating concurrent inserts to force a resize...\n");
  int *v3 = CDSA_MALLOC(sizeof(int));
  *v3 = 3;
  int *v4 = CDSA_MALLOC(sizeof(int));
  *v4 = 4;
  int *v5 = CDSA_MALLOC(sizeof(int));
  *v5 = 5; // This one pushes it over the edge!

  insert_hashmap(map, "Key3", v3);
  insert_hashmap(map, "Key4", v4);
  insert_hashmap(map, "Key5", v5);

  // 5. THE TEST: Try to use the old iterator again
  printf("Attempting to use the old iterator...\n");
  status = cdsa_next_hashmap(iter, &key, &value);

  if (status == CDSA_ERR_ITER_INVALIDATED) {
    printf("[SUCCESS] Iterator correctly detected structural mutation and "
           "aborted!\n");
  } else {
    printf("[FAIL] Iterator did not detect mutation! Status: %d\n", status);
    assert(false); // Fail the test immediately
  }

  // Also check cdsa_has_next_hashmap
  assert(cdsa_has_next_hashmap(iter) == false);

  // 6. Cleanup
  cdsa_free_hashmap_iterator(iter);

  // We must create a fresh iterator to clean up the memory,
  // because the old one is permanently locked out!
  cdsa_hashmap_iterator *cleanup_iter = cdsa_create_hashmap_iterator(map);
  while (cdsa_has_next_hashmap(cleanup_iter)) {
    cdsa_next_hashmap(cleanup_iter, &key, &value);
    CDSA_FREE(value);
  }
  cdsa_free_hashmap_iterator(cleanup_iter);
  cdsa_free_hashmap(map);

  printf("=== Mutation Guard Test Complete ===\n\n");
}

int main() {
  test_hashmap_iterator();
  test_iterator_mutation_guard();
  return 0;
}

#include "CDSA/allocator.h"
#include "CDSA/hashmap.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void test_hashmap_iterator() {
  printf("\n=== Testing HashMap Iterator ===\n");

  HashMap *map = create_hashmap(16);

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
  HashMapIterator *iter = create_hashmap_iterator(map);
  const char *key;
  void *value;
  int count = 0;

  printf("Iterating map contents:\n");
  while (has_next_hashmap(iter)) {
    CDSA_STATUS status = next_hashmap(iter, &key, &value);
    assert(status == CDSA_OK);

    printf("  -> %s : %d\n", key, *(int *)value);
    count++;
  }
  printf("Total elements seen: %d (Expected: 3)\n", count);
  assert(count == 3);

  free_hashmap_iterator(iter);

  // 3. Pass Two: Use a NEW iterator to safely free the caller-owned values!
  // This proves why the iterator is so powerful for Kedis-C.
  HashMapIterator *cleanup_iter = create_hashmap_iterator(map);

  while (has_next_hashmap(cleanup_iter)) {
    next_hashmap(cleanup_iter, &key, &value);
    CDSA_FREE(value); // Freeing the integer pointers we malloc'd at the start
  }

  free_hashmap_iterator(cleanup_iter);

  // 4. Finally, free the map itself
  free_hashmap(map);

  printf("=== Iterator Test Complete ===\n\n");
}

int main() {
  test_hashmap_iterator();
  return 0;
}

#include "CDSA/error.h"
#include "CDSA/vector.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void test_vector_iterator() {
  printf("\n=== Testing cdsa_vector Iterator ===\n");

  // 1. Create a cdsa_vector with a tiny initial capacity (e.g., 2) to easily force a
  // resize
  cdsa_vector *vec = cdsa_create_vector(sizeof(int));
  // Note: if your cdsa_create_vector doesn't take capacity, you might need to push
  // until you know it resizes based on your internal growth factor.

  int v1 = 10;
  int v2 = 20;
  cdsa_push_vector(vec, &v1);
  cdsa_push_vector(vec, &v2);

  // 2. Read the data using the Iterator
  cdsa_vector_iterator *iter = cdsa_create_vector_iterator(vec);
  void *value_ptr;
  int count = 0;

  printf("Iterating cdsa_vector contents:\n");
  while (cdsa_has_next_vector(iter)) {
    CDSA_STATUS status = cdsa_next_vector(iter, &value_ptr);
    assert(status == CDSA_OK);

    // Dereference the yielded pointer!
    printf("  -> Index %d : %d\n", count, *(int *)value_ptr);
    count++;
  }

  printf("Total elements seen: %d (Expected: 2)\n", count);
  assert(count == 2);

  // 3. THE LANDMINE: Push enough elements to force the cdsa_vector to reallocate
  printf("Simulating concurrent pushes to force an array reallocation...\n");
  int v3 = 30;
  int v4 = 40;
  int v5 = 50;
  cdsa_push_vector(vec, &v3);
  cdsa_push_vector(vec, &v4);
  cdsa_push_vector(vec, &v5);

  // 4. THE TEST: Try to use the old iterator again
  printf("Attempting to use the old iterator after reallocation...\n");
  CDSA_STATUS status = cdsa_next_vector(iter, &value_ptr);

  if (status == CDSA_ERR_ITER_INVALIDATED) {
    printf("[SUCCESS] Iterator correctly detected vector reallocation and "
           "aborted!\n");
  } else {
    printf("[FAIL] Iterator did not detect mutation! Status: %d\n", status);
    assert(false);
  }

  // 5. Cleanup
  cdsa_free_vector_iterator(iter);
  cdsa_free_vector(vec);

  printf("=== cdsa_vector Iterator Test Complete ===\n\n");
}

int main(void) {
  test_vector_iterator();
  return 0;
}

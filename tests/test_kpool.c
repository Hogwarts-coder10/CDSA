#include "CDSA/allocator.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static void test_kpool_creation_and_destruction() {
  printf("[*] test_kpool_creation_and_destruction\n");

  // Create a pool for 10 blocks of 32 bytes each
  kpool *pool = kpool_create(32, 10);
  assert(pool != NULL);

  // Clean up
  kpool_destroy(pool);
  printf("    PASS\n");
}

static void test_kpool_exhaustion() {
  printf("[*] test_kpool_exhaustion\n");

  size_t capacity = 5;
  kpool *pool = kpool_create(64, capacity);
  assert(pool != NULL);

  void *blocks[5];

  // Exhaust the pool
  for (size_t i = 0; i < capacity; i++) {
    blocks[i] = kpool_alloc(pool);
    assert(blocks[i] != NULL);
  }

  // The next allocation should fail and return NULL (OOM guard)
  void *overflow = kpool_alloc(pool);
  assert(overflow == NULL);

  kpool_destroy(pool);
  printf("    PASS\n");
}

static void test_kpool_reuse() {
  printf("[*] test_kpool_reuse\n");

  kpool *pool = kpool_create(128, 2);
  assert(pool != NULL);

  void *block1 = kpool_alloc(pool);
  void *block2 = kpool_alloc(pool);
  assert(block1 != NULL && block2 != NULL);

  // Free the first block, returning it to the free-list
  kpool_free(pool, block1);

  // Reallocate and verify it reuses the exact same memory address
  void *block3 = kpool_alloc(pool);
  assert(block3 != NULL);
  assert(block3 == block1);

  kpool_destroy(pool);
  printf("    PASS\n");
}

int main() {
  printf("======================================\n");
  printf(" RUNNING KPOOL SLAB ALLOCATOR TESTS\n");
  printf("======================================\n");

  test_kpool_creation_and_destruction();
  test_kpool_exhaustion();
  test_kpool_reuse();

  printf("======================================\n");
  printf(" ALL KPOOL TESTS PASSED\n");
  printf("======================================\n");

  return 0;
}

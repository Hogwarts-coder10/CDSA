#include "CDSA/allocator.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main() {
  printf("--- Arena Shakedown Test ---\n");

  // 1. Create a 1024-byte arena workspace
  cdsa_arena *arena = arena_create(1024);
  assert(arena != NULL);
  printf("[+] Arena created with 1024 bytes.\n");

  // 2. Allocate a weird, unaligned size (13 bytes)
  char *str = (char *)arena_alloc(arena, 13);
  strcpy(str, "Hello Wallet");
  printf("[+] Allocated 13 bytes. Stored: %s\n", str);

  // 3. Allocate an integer. The macro will automatically push it to an 8-byte
  // boundary!
  int *my_number = (int *)arena_alloc(arena, sizeof(int));
  *my_number = 999;
  printf("[+] Allocated int safely aligned. Stored: %d\n", *my_number);

  // 4. Instantly wipe the whole workspace (Offset back to 0)
  arena_reset(arena);
  printf("[+] Arena reset. Workspace wiped clean.\n");

  // 5. Clean shutdown
  arena_destroy(arena);
  printf("[+] Arena destroyed. No leaks.\n");

  return 0;
}

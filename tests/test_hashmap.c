#include "CDSA/hashmap.h"
#include <stdio.h>

int main() {
  printf("🚀 Starting HashMap Tests...\n\n");

  HashMap *map = create_hashmap(16);

  // 1. Basic Insertion & Retrieval
  printf("[*] Testing Insertion & Retrieval...\n");
  int a = 10, b = 20;
  insert_hashmap(map, "apple", &a);
  insert_hashmap(map, "banana", &b);

  printf("apple -> %d (Expected: 10)\n", *(int *)get_hashmap(map, "apple"));
  printf("banana -> %d (Expected: 20)\n\n", *(int *)get_hashmap(map, "banana"));

  // 2. Contains Check
  printf("[*] Testing Contains Logic...\n");
  printf("Contains 'apple'? %s (Expected: Yes)\n",
         contains_hashmap(map, "apple") ? "Yes" : "No");
  printf("Contains 'grape'? %s (Expected: No)\n\n",
         contains_hashmap(map, "grape") ? "Yes" : "No");

  // 3. Duplicate Key Overwrite
  printf("[*] Testing Key Overwrite...\n");
  int new_a = 99;
  insert_hashmap(map, "apple", &new_a); // Should overwrite 10 with 99

  printf("apple -> %d (Expected: 99)\n", *(int *)get_hashmap(map, "apple"));
  printf("HashMap Size: %zu (Expected: 2)\n\n", map->size);

  // 4. Load Factor Guard & Resizing
  printf("[*] Testing Load Factor Guard & Resizing...\n");
  int dummy = 1;
  const char *keys[] = {"1", "2", "3", "4",  "5", "6",
                        "7", "8", "9", "10", "11"};
  for (int i = 0; i < 11; i++) {
    insert_hashmap(map, keys[i], &dummy);
  }

  // 5. Deletion & Tombstones (THE NEW STUFF)
  printf("\n[*] Testing Deletion & Tombstones...\n");
  printf("Contains 'banana' before remove? %s (Expected: Yes)\n",
         contains_hashmap(map, "banana") ? "Yes" : "No");

  remove_hashmap(map, "banana");

  printf("Contains 'banana' after remove? %s (Expected: No)\n",
         contains_hashmap(map, "banana") ? "Yes" : "No");
  printf("get_hashmap('banana') == NULL? %s (Expected: Yes)\n",
         get_hashmap(map, "banana") == NULL ? "Yes" : "No");

  // 6. Tombstone Reuse (THE NEW STUFF)
  printf("\n[*] Testing Tombstone Reuse...\n");
  int new_val = 999;
  insert_hashmap(map, "new_banana", &new_val);
  printf("new_banana -> %d (Expected: 999)\n",
         *(int *)get_hashmap(map, "new_banana"));

  printf("\n--- Current Map State ---\n");
  print_hashmap(map);

  free_hashmap(map);
  printf("\n✅ Tests complete!\n");

  return 0;
}

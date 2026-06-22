#include "CDSA/skiplist.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
  printf("🏁 Starting Skip List Tests...\n\n");

  SkipList *sl = create_skiplist();

  // 1. Insert players into a leaderboard out of order
  printf("[*] Inserting Players...\n");
  insert_skiplist(sl, 1500.5, "Karthik");
  insert_skiplist(sl, 1200.0, "Alice");
  insert_skiplist(sl, 1800.2, "Bob");
  insert_skiplist(sl, 1500.5,
                  "Charlie"); // Tie score, should sort alphabetically!

  printf("Total Size: %zu (Expected: 4)\n", sl->size);
  printf("Max Level Reached: %d\n\n", sl->level);

  // 2. Print the Level 0 (Base) Linked List to verify sorting
  printf("[*] Leaderboard (Sorted Order):\n");
  SkipNode *current = sl->header->forward[0];
  while (current != NULL) {
    // We print the level it was randomly assigned too!
    printf(" -> [%.1f] %s (Level %d)\n", current->score, current->value,
           current->level);
    current = current->forward[0];
  }
  printf("\n");

  // 3. Test Deletion
  printf("[*] Testing Deletion...\n");
  printf("Deleting Alice: %s\n",
         remove_skiplist(sl, 1200.0, "Alice") ? "Success" : "Failed");
  printf("Deleting Ghost: %s (Expected: Failed)\n",
         remove_skiplist(sl, 9999.0, "Ghost") ? "Success" : "Failed");

  printf("New Size: %zu (Expected: 3)\n\n", sl->size);

  // 4. Test Range Queries (The Redis ZRANGE feature)
  printf("[*] Testing Range Query (Scores between 1300 and 1900)...\n");
  int count = 0;
  char **range_results = get_range_skiplist(sl, 1300.0, 1900.0, &count);

  printf("Found %d players in range:\n", count);
  for (int i = 0; i < count; i++) {
    printf("  -> %s\n", range_results[i]);
    free(range_results[i]); // Free the copied string
  }
  free(range_results); // Free the array of pointers
  printf("\n");

  // Clean up
  free_skiplist(sl);
  printf("✅ Skip List Tests completely freed!\n");

  return 0;
}

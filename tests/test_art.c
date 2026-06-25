#include "CDSA/art.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
  printf("🌲 Starting Final Boss Stress Test: Node256!\n\n");

  ArtTree *tree = create_art();

  printf("[*] Inserting 60 distinct keys to force Node256 upgrade...\n");

  // We need > 48 distinct first-characters to trigger Node256.
  // We will use a loop to generate strings like "A_Data", "B_Data", etc.
  // ASCII printable characters start at 33 ('!').

  for (int i = 0; i < 60; i++) {
    char key[16];
    // Generate a unique string for each iteration
    sprintf(key, "%c_Data", (char)(33 + i));

    insert_art(tree, key, "Payload");

    // Print milestones so we can watch the engine shift gears
    if (i == 3)
      printf("   -> Inserted 4 items  (Node4 full)\n");
    if (i == 15)
      printf("   -> Inserted 16 items (Node16 full)\n");
    if (i == 47)
      printf("   -> Inserted 48 items (Node48 full - UPGRADING TO NODE256!)\n");
  }

  printf("\n✅ Insertions complete. Root node should now be a NODE256! (Tree "
         "Size: %zu)\n\n",
         tree->size);

  // 2. Test O(1) Absolute Direct Access Searching
  printf("[*] Testing Node256 O(1) Direct Lookups...\n");

  char *res1 = (char *)search_art(tree, "A_Data"); // 'A' is ASCII 65
  printf("Search 'A_Data': %s (Expected: Payload)\n", res1 ? res1 : "NULL");

  char *res2 = (char *)search_art(tree, "Z_Data"); // 'Z' is ASCII 90
  printf("Search 'Z_Data': %s (Expected: Payload)\n", res2 ? res2 : "NULL");

  char *res3 = (char *)search_art(tree, "Ghost"); // Doesn't exist
  printf("Search 'Ghost':  %s (Expected: NULL)\n\n", res3 ? res3 : "NULL");

  // 3. Test Teardown (Will hit our new NODE256 free loop)
  printf("[*] Freeing Tree (Testing Node256 Teardown)...\n");
  free_art(tree);

  printf("✅ Tree completely dismantled!\n");

  return 0;
}

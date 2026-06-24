#include "CDSA/art.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
  printf("🌲 Starting Node48 Stress Test...\n\n");

  ArtTree *tree = create_art();

  // We need to insert 17 words that start with DIFFERENT letters.
  // This will force the root node to hold 17 children,
  // triggering the upgrades: Node4 -> Node16 -> Node48!

  char *words[] = {
      "Apple", "Banana",    "Cherry", "Date",      "Elderberry", "Fig",
      "Grape", "Honeydew",  "Ice",    "Jackfruit", "Kiwi",       "Lemon",
      "Mango", "Nectarine", "Orange", "Papaya",    "Quince" // <-- The 17th
                                                            // word! BOOM!
  };

  printf("[*] Inserting 17 distinct keys to force Node48 upgrade...\n");
  for (int i = 0; i < 17; i++) {
    insert_art(tree, words[i], "Data");
    printf("Inserted: %-12s | Tree Size: %zu\n", words[i], tree->size);
  }

  printf("\n✅ Insertions complete. Root node should now be a NODE48!\n\n");

  // 2. Test O(1) Hashmap Searching
  printf("[*] Testing Node48 O(1) Hashmap Lookups...\n");

  char *res1 = (char *)search_art(tree, "Kiwi");
  printf("Search 'Kiwi':   %s (Expected: Data)\n", res1 ? res1 : "NULL");

  char *res2 = (char *)search_art(tree, "Apple");
  printf("Search 'Apple':  %s (Expected: Data)\n", res2 ? res2 : "NULL");

  char *res3 = (char *)search_art(tree, "Quince");
  printf("Search 'Quince': %s (Expected: Data)\n", res3 ? res3 : "NULL");

  char *res4 = (char *)search_art(tree, "Zebra");
  printf("Search 'Zebra':  %s (Expected: NULL)\n\n", res4 ? res4 : "NULL");

  // 3. Test Teardown (Will hit our new NODE48 free loop)
  printf("[*] Freeing Tree (Testing Node48 Teardown)...\n");
  free_art(tree);
  printf("✅ Tree completely dismantled!\n");

  return 0;
}

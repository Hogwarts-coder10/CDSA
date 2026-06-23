#include "CDSA/art.h"
#include <stdio.h>

int main() {
  printf("🌲 Starting Adaptive Radix Tree Tests...\n\n");

  ArtTree *tree = create_art();

  // 1. Insert 5 keys that share the exact same first letter 'a'
  // This will force the root node (which starts as Node4) to upgrade to Node16!
  printf("[*] Inserting Data (Forcing Node4 -> Node16 Upgrade)...\n");
  insert_art(tree, "apple", "Data A");
  insert_art(tree, "ant", "Data B");
  insert_art(tree, "axe", "Data C");
  insert_art(tree, "arc", "Data D");

  // --> AT THIS EXACT MOMENT, THE ROOT IS FULL (4 items) <--

  insert_art(tree, "ape", "Data E"); // Boom! Upgrades to Node16!

  printf("Total Keys in Database: %zu (Expected: 5)\n\n", tree->size);

  // 2. Test Searching
  printf("[*] Testing Search...\n");
  char *result = (char *)search_art(tree, "axe");
  printf("Search 'axe': %s (Expected: Data C)\n", result ? result : "NULL");

  result = (char *)search_art(tree, "ape");
  printf("Search 'ape': %s (Expected: Data E)\n", result ? result : "NULL");

  result = (char *)search_art(tree, "alien");
  printf("Search 'alien': %s (Expected: NULL)\n\n", result ? result : "NULL");

  // 3. Dismantle the Tree
  printf("[*] Freeing Tree...\n");
  free_art(tree);
  printf("✅ Tree completely dismantled!\n");

  return 0;
}

#include "CDSA/art.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
  printf("🌲 Starting Final Boss Stress Test: Path Compression!\n\n");

  ArtTree *tree = create_art();

  // 1. Trigger the Leaf Splitter (Creates the initial compressed node)
  printf("[*] Inserting 'understanding'...\n");
  insert_art(tree, "understanding", "Payload A");

  // Shares "understand", splits at 'i' vs 'a'
  printf("[*] Inserting 'understandable' (Splits Leaf)...\n");
  insert_art(tree, "understandable", "Payload B");

  // 2. Trigger the Internal Node Splitter (Snaps compressed nodes in half)
  // Shares "under", breaks the "understand" prefix at 's' vs 'd'
  printf("[*] Inserting 'underdog' (Snaps 'understand' -> 'under')...\n");
  insert_art(tree, "underdog", "Payload C");

  // Shares "u", breaks the "under" prefix at 'n' vs 'm'
  printf("[*] Inserting 'umbrella' (Snaps 'under' -> 'u')...\n");
  insert_art(tree, "umbrella", "Payload D");

  printf("\n✅ Insertions complete. Tree Size: %zu\n\n", tree->size);

  // 3. Test the Reader (Fast-forwarding depth through prefixes)
  printf("[*] Testing Prefix Reader Lookups...\n");

  char *res1 = (char *)search_art(tree, "understanding");
  printf("Search 'understanding':  %s (Expected: Payload A)\n",
         res1 ? res1 : "NULL");

  char *res2 = (char *)search_art(tree, "understandable");
  printf("Search 'understandable': %s (Expected: Payload B)\n",
         res2 ? res2 : "NULL");

  char *res3 = (char *)search_art(tree, "underdog");
  printf("Search 'underdog':       %s (Expected: Payload C)\n",
         res3 ? res3 : "NULL");

  char *res4 = (char *)search_art(tree, "umbrella");
  printf("Search 'umbrella':       %s (Expected: Payload D)\n",
         res4 ? res4 : "NULL");

  // "under" is a prefix in our tree, but it was never inserted as a leaf!
  char *res5 = (char *)search_art(tree, "under");
  printf("Search 'under':          %s (Expected: NULL)\n\n",
         res5 ? res5 : "NULL");

  // 4. Test Teardown
  print_art(tree);
  printf("[*] Freeing Tree (Testing Fragmented Teardown)...\n");
  free_art(tree);

  printf("✅ Tree completely dismantled!\n");

  return 0;
}

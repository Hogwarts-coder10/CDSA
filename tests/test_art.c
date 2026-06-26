#include "CDSA/art.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper to quickly verify searches
void verify(ArtTree *tree, const char *key, const char *expected) {
  char *result = (char *)search_art(tree, key);
  if (result == NULL && expected == NULL) {
    printf("  [PASS] Key '%s' correctly not found.\n", key);
  } else if (result != NULL && expected != NULL &&
             strcmp(result, expected) == 0) {
    printf("  [PASS] Key '%s' found -> %s\n", key, result);
  } else {
    printf("  [FAIL] Key '%s'. Expected: %s, Got: %s\n", key,
           expected ? expected : "NULL", result ? result : "NULL");
  }
}

int main() {
  printf("==========================================\n");
  printf("🌲 ART ENGINE: COMPLETE STRESS TEST 🌲\n");
  printf("==========================================\n\n");

  ArtTree *tree = create_art();

  // ---------------------------------------------------------
  printf("--- PHASE 1: PATH COMPRESSION & SPLITTING ---\n");
  // ---------------------------------------------------------
  insert_art(tree, "understanding", "Payload A");
  insert_art(tree, "understandable", "Payload B"); // Splits leaf
  insert_art(tree, "underdog", "Payload C"); // Splits internal compressed path
  insert_art(tree, "umbrella", "Payload D"); // Splits early prefix

  verify(tree, "understanding", "Payload A");
  verify(tree, "umbrella", "Payload D");
  verify(tree, "under", NULL); // Prefix exists, but not as a leaf
  printf("Tree Size: %zu\n\n", tree->size);

  // ---------------------------------------------------------
  printf("--- PHASE 2: WIDTH UPGRADES (NODE4 -> NODE256) ---\n");
  // ---------------------------------------------------------
  // We will insert keys starting with "Z" followed by chars 0-255.
  // This forces the node under "Z" to upgrade through all 4 sizes.
  printf("[*] Hammering tree with 256 keys to force Node morphing...\n");

  char key_buffer[5];

  for (int i = 0; i < 256; i++) {
    // Create strings like "Z\x00", "Z\x01", ... "Z\xFF"
    sprintf(key_buffer, "Z%c", (char)i);
    char *payload = malloc(20); // DYNAMIC ALLOCATION
    sprintf(payload, "Data %d", i);
    insert_art(tree, key_buffer, payload);
  }

  printf("[*] Upgrades complete. Verifying...\n");
  verify(tree, "Z\x0A", "Data 10");
  verify(tree, "Z\x80", "Data 128");
  verify(tree, "Z\xFF", "Data 255");
  printf("Tree Size: %zu\n\n", tree->size);

  // ---------------------------------------------------------
  printf("--- PHASE 3: DELETION & SHRINKAGE (NODE256 -> NODE4) ---\n");
  // ---------------------------------------------------------
  printf("[*] Deleting keys to force node collapse...\n");

  // Delete enough to drop below 48 (Triggers Node256 -> Node48)
  for (int i = 255; i >= 48; i--) {
    sprintf(key_buffer, "Z%c", (char)i);
    void *payload = search_art(tree, key_buffer); // FETCH FIRST
    if (payload)
      free(payload);              // FREE FIRST
    delete_art(tree, key_buffer); // THEN DELETE NODE
  }
  printf("[*] Shrunk to Node48. Deleting more...\n");

  // Delete enough to drop below 16 (Triggers Node48 -> Node16)
  for (int i = 47; i >= 16; i--) {
    sprintf(key_buffer, "Z%c", (char)i);
    void *payload = search_art(tree, key_buffer);
    if (payload)
      free(payload);
    delete_art(tree, key_buffer);
  }
  printf("[*] Shrunk to Node16. Deleting more...\n");

  // Delete enough to drop to 4 (Triggers Node16 -> Node4)
  for (int i = 15; i >= 4; i--) {
    sprintf(key_buffer, "Z%c", (char)i);
    void *payload = search_art(tree, key_buffer);
    if (payload)
      free(payload);
    delete_art(tree, key_buffer);
  }

  printf("[*] Verifying remaining keys in shrunk node...\n");
  verify(tree, "Z\x03", "Data 3");
  verify(tree, "Z\xFF", NULL); // Proves it was deleted
  printf("Tree Size: %zu\n\n", tree->size);

  // ---------------------------------------------------------
  printf("--- PHASE 4: PATH MERGING (PREFIX GLUING) ---\n");
  // ---------------------------------------------------------
  // "underdog" and "understanding" share "under".
  // If we delete "underdog", the Node4 intersection should die,
  // and "understand" should glue back together.
  printf("[*] Deleting 'underdog' to trigger Path Merging...\n");
  delete_art(tree, "underdog");

  printf("[*] Verifying structure survived the merge...\n");
  verify(tree, "understanding", "Payload A");
  verify(tree, "understandable", "Payload B");
  verify(tree, "underdog", NULL);

  printf("\n--- FINAL TREE VISUALIZATION ---\n");
  print_art(tree);

  // ---------------------------------------------------------
  printf("--- PHASE 5: MEMORY TEARDOWN ---\n");
  // ---------------------------------------------------------
  // Free the remaining manually allocated payloads from Phase 2 (Indices 0, 1,
  // 2, 3)
  for (int i = 0; i < 4; i++) {
    sprintf(key_buffer, "Z%c", (char)i);
    void *payload = search_art(tree, key_buffer);
    if (payload)
      free(payload);
  }

  free_art(tree);
  printf("\n✅ Tree completely dismantled. Run Valgrind to verify 0 leaks!\n");
  printf("==========================================\n\n");

  return 0;
}

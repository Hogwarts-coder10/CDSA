#define _POSIX_C_SOURCE 200809L
#include "CDSA/skiplist.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ── helpers ─────────────────────────────────────────────────────────────────

static void free_range(char **results, int count) {
  for (int i = 0; i < count; i++)
    free(results[i]);
  free(results);
}

static int range_contains(char **results, int count, const char *val) {
  for (int i = 0; i < count; i++)
    if (strcmp(results[i], val) == 0)
      return 1;
  return 0;
}

// ── test sections ────────────────────────────────────────────────────────────

static void test_create_empty() {
  printf("[*] test_create_empty\n");
  SkipList *sl = create_skiplist();
  assert(sl != NULL);
  assert(size_skiplist(sl) == 0);
  assert(level_skiplist(sl) >= 1);
  free_skiplist(sl);
  printf("    PASS\n");
}

static void test_basic_insert_and_size() {
  printf("[*] test_basic_insert_and_size\n");
  SkipList *sl = create_skiplist();

  assert(insert_skiplist(sl, 1.0, "alpha") == true);
  assert(size_skiplist(sl) == 1);

  assert(insert_skiplist(sl, 2.0, "beta") == true);
  assert(size_skiplist(sl) == 2);

  assert(insert_skiplist(sl, 3.0, "gamma") == true);
  assert(size_skiplist(sl) == 3);

  free_skiplist(sl);
  printf("    PASS\n");
}

static void test_delete_existing() {
  printf("[*] test_delete_existing\n");
  SkipList *sl = create_skiplist();

  insert_skiplist(sl, 10.0, "alice");
  insert_skiplist(sl, 20.0, "bob");
  insert_skiplist(sl, 30.0, "charlie");

  assert(remove_skiplist(sl, 20.0, "bob") == true);
  assert(size_skiplist(sl) == 2);

  // bob should no longer appear in a full range
  int count = 0;
  char **r = get_range_skiplist(sl, 0.0, 100.0, &count);
  assert(count == 2);
  assert(range_contains(r, count, "alice") == 1);
  assert(range_contains(r, count, "charlie") == 1);
  assert(range_contains(r, count, "bob") == 0);
  free_range(r, count);

  free_skiplist(sl);
  printf("    PASS\n");
}

static void test_delete_nonexistent() {
  printf("[*] test_delete_nonexistent\n");
  SkipList *sl = create_skiplist();

  insert_skiplist(sl, 5.0, "x");
  assert(remove_skiplist(sl, 9999.0, "ghost") == false);
  assert(remove_skiplist(sl, 5.0, "wrong_value") == false);
  assert(size_skiplist(sl) == 1); // nothing was actually removed

  free_skiplist(sl);
  printf("    PASS\n");
}

static void test_delete_then_reinsert() {
  printf("[*] test_delete_then_reinsert\n");
  SkipList *sl = create_skiplist();

  insert_skiplist(sl, 1.0, "ping");
  assert(remove_skiplist(sl, 1.0, "ping") == true);
  assert(size_skiplist(sl) == 0);

  // reinsert same key — should work fine
  assert(insert_skiplist(sl, 1.0, "ping") == true);
  assert(size_skiplist(sl) == 1);

  int count = 0;
  char **r = get_range_skiplist(sl, 0.0, 5.0, &count);
  assert(count == 1);
  assert(strcmp(r[0], "ping") == 0);
  free_range(r, count);

  free_skiplist(sl);
  printf("    PASS\n");
}

static void test_tied_scores_alphabetical_order() {
  printf("[*] test_tied_scores_alphabetical_order\n");
  SkipList *sl = create_skiplist();

  // All same score — skiplist must fall back to alphabetical (like Redis ZADD)
  insert_skiplist(sl, 1500.0, "Karthik");
  insert_skiplist(sl, 1500.0, "Alice");
  insert_skiplist(sl, 1500.0, "Charlie");
  insert_skiplist(sl, 1500.0, "Bob");

  assert(size_skiplist(sl) == 4);

  int count = 0;
  char **r = get_range_skiplist(sl, 1500.0, 1500.0, &count);
  assert(count == 4);
  // results should be in alphabetical order: Alice, Bob, Charlie, Karthik
  assert(strcmp(r[0], "Alice") == 0);
  assert(strcmp(r[1], "Bob") == 0);
  assert(strcmp(r[2], "Charlie") == 0);
  assert(strcmp(r[3], "Karthik") == 0);
  free_range(r, count);

  free_skiplist(sl);
  printf("    PASS\n");
}

static void test_range_query_basic() {
  printf("[*] test_range_query_basic\n");
  SkipList *sl = create_skiplist();

  insert_skiplist(sl, 100.0, "low");
  insert_skiplist(sl, 500.0, "mid");
  insert_skiplist(sl, 900.0, "high");

  int count = 0;
  char **r = get_range_skiplist(sl, 200.0, 800.0, &count);
  assert(count == 1);
  assert(strcmp(r[0], "mid") == 0);
  free_range(r, count);

  free_skiplist(sl);
  printf("    PASS\n");
}

static void test_range_query_inclusive_bounds() {
  printf("[*] test_range_query_inclusive_bounds\n");
  SkipList *sl = create_skiplist();

  insert_skiplist(sl, 1.0, "a");
  insert_skiplist(sl, 2.0, "b");
  insert_skiplist(sl, 3.0, "c");

  // exact boundary hits
  int count = 0;
  char **r = get_range_skiplist(sl, 1.0, 3.0, &count);
  assert(count == 3);
  free_range(r, count);

  // single point
  r = get_range_skiplist(sl, 2.0, 2.0, &count);
  assert(count == 1);
  assert(strcmp(r[0], "b") == 0);
  free_range(r, count);

  free_skiplist(sl);
  printf("    PASS\n");
}

static void test_range_query_empty_result() {
  printf("[*] test_range_query_empty_result\n");
  SkipList *sl = create_skiplist();

  insert_skiplist(sl, 1.0, "a");
  insert_skiplist(sl, 2.0, "b");

  int count = 999; // sentinel
  char **r = get_range_skiplist(sl, 50.0, 100.0, &count);
  assert(r == NULL);
  assert(count == 0);

  free_skiplist(sl);
  printf("    PASS\n");
}

static void test_range_query_all() {
  printf("[*] test_range_query_all\n");
  SkipList *sl = create_skiplist();

  insert_skiplist(sl, 1.0, "a");
  insert_skiplist(sl, 2.0, "b");
  insert_skiplist(sl, 3.0, "c");
  insert_skiplist(sl, 4.0, "d");
  insert_skiplist(sl, 5.0, "e");

  int count = 0;
  char **r = get_range_skiplist(sl, 0.0, 999.0, &count);
  assert(count == 5);
  assert(strcmp(r[0], "a") == 0);
  assert(strcmp(r[4], "e") == 0);
  free_range(r, count);

  free_skiplist(sl);
  printf("    PASS\n");
}

static void test_level_shrinks_after_delete() {
  printf("[*] test_level_shrinks_after_delete\n");
  SkipList *sl = create_skiplist();

  // Insert many entries to ensure some tall nodes are created
  for (int i = 0; i < 50; i++) {
    char buf[16];
    snprintf(buf, sizeof(buf), "key%d", i);
    insert_skiplist(sl, (double)i, buf);
  }
  assert(size_skiplist(sl) == 50);

  // Delete all — level must never go below 1
  for (int i = 0; i < 50; i++) {
    char buf[16];
    snprintf(buf, sizeof(buf), "key%d", i);
    remove_skiplist(sl, (double)i, buf);
  }
  assert(size_skiplist(sl) == 0);
  assert(level_skiplist(sl) >= 1);

  free_skiplist(sl);
  printf("    PASS\n");
}

static void test_insert_delete_churn() {
  printf("[*] test_insert_delete_churn\n");
  SkipList *sl = create_skiplist();

  // Repeated insert/delete of the same key — verifies no leak or corruption
  for (int i = 0; i < 500; i++) {
    assert(insert_skiplist(sl, 1.0, "churn") == true);
    assert(size_skiplist(sl) == 1);
    assert(remove_skiplist(sl, 1.0, "churn") == true);
    assert(size_skiplist(sl) == 0);
  }

  free_skiplist(sl);
  printf("    PASS\n");
}

static void test_free_null() {
  printf("[*] test_free_null\n");
  free_skiplist(NULL); // must not crash
  printf("    PASS\n");
}

static void test_range_caller_owns_strings() {
  printf("[*] test_range_caller_owns_strings\n");
  SkipList *sl = create_skiplist();

  insert_skiplist(sl, 1.0, "original");

  int count = 0;
  char **r = get_range_skiplist(sl, 0.0, 10.0, &count);
  assert(count == 1);

  // Mutate the returned string — must not corrupt the skiplist
  r[0][0] = 'X';

  // The node inside the skiplist still has "original" so deletion still works
  assert(remove_skiplist(sl, 1.0, "original") == true);

  free_range(r, count);
  free_skiplist(sl);
  printf("    PASS\n");
}

static void test_kedis_style_namespaced_keys() {
  printf("[*] test_kedis_style_namespaced_keys\n");
  SkipList *sl = create_skiplist();

  insert_skiplist(sl, 1.0, "session:user:12345:auth_token");
  insert_skiplist(sl, 2.0, "session:user:12345:refresh_token");
  insert_skiplist(sl, 3.0, "session:user:99999:auth_token");

  assert(size_skiplist(sl) == 3);

  int count = 0;
  char **r = get_range_skiplist(sl, 1.0, 2.0, &count);
  assert(count == 2);
  assert(range_contains(r, count, "session:user:12345:auth_token") == 1);
  assert(range_contains(r, count, "session:user:12345:refresh_token") == 1);
  free_range(r, count);

  assert(remove_skiplist(sl, 1.0, "session:user:12345:auth_token") == true);
  assert(size_skiplist(sl) == 2);

  free_skiplist(sl);
  printf("    PASS\n");
}

// ── main ─────────────────────────────────────────────────────────────────────

int main() {
  printf("==========================================\n");
  printf("  SKIPLIST TEST SUITE\n");
  printf("==========================================\n\n");

  test_create_empty();
  test_basic_insert_and_size();
  test_delete_existing();
  test_delete_nonexistent();
  test_delete_then_reinsert();
  test_tied_scores_alphabetical_order();
  test_range_query_basic();
  test_range_query_inclusive_bounds();
  test_range_query_empty_result();
  test_range_query_all();
  test_level_shrinks_after_delete();
  test_insert_delete_churn();
  test_free_null();
  test_range_caller_owns_strings();
  test_kedis_style_namespaced_keys();

  printf("\n==========================================\n");
  printf("  ALL TESTS PASSED\n");
  printf("==========================================\n");
  return 0;
}

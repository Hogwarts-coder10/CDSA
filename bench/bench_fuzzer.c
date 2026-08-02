#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Core CDSA Headers */
#include "CDSA/allocator.h"
#include "CDSA/art.h"
#include "CDSA/deque.h"
#include "CDSA/hashmap.h"
#include "CDSA/kstring.h"
#include "CDSA/priority_queue.h"
#include "CDSA/skiplist.h"
#include "CDSA/vector.h"

#define FUZZ_OPS 1000000
#define KEY_POOL_SIZE 50000
#define MAX_KEY_LEN 64

static char key_pool[KEY_POOL_SIZE][MAX_KEY_LEN];

/* Priority Queue Comparator */
static bool int_max_cmp(void *a, void *b) { return *(int *)a > *(int *)b; }

static void generate_chaos_keys(void) {
  for (int i = 0; i < KEY_POOL_SIZE; i++) {
    int len = (rand() % (MAX_KEY_LEN - 2)) + 1;
    for (int j = 0; j < len; j++) {
      key_pool[i][j] = (char)((rand() % 94) + 33); // Printable ASCII
    }
    key_pool[i][len] = '\0';
  }
}

int main(void) {
  printf("====================================================\n");
  printf(" FULL-SUITE CHAOS MONKEY FUZZER (%d Ops)\n", FUZZ_OPS);
  printf("====================================================\n\n");

  srand((unsigned int)time(NULL));

  printf("[System] Generating %d chaotic payloads...\n", KEY_POOL_SIZE);
  generate_chaos_keys();

  /* Initialize core data structures with correct signatures */
  cdsa_art_tree *art = cdsa_create_art();
  cdsa_hashmap *map = cdsa_create_hashmap(1024);
  cdsa_vector *vec = cdsa_create_vector(sizeof(int));
  cdsa_skiplist *list = cdsa_create_skiplist();
  cdsa_priority_queue *pq =
      cdsa_create_pq(sizeof(int), (PriorityCompareFn)int_max_cmp);
  cdsa_deque *dq = cdsa_create_deque(2048, sizeof(int));
  cdsa_kstring *str = cdsa_create_kstring();

  printf("[System] Unleashing Chaos Monkey across all structures...\n");

  for (int i = 0; i < FUZZ_OPS; i++) {
    int op = rand() % 100;
    int idx = rand() % KEY_POOL_SIZE;
    int val = rand();

    /* -------------------------------------------------------------
     * 1. Indexing Engines: ART, HashMap, SkipList
     * ------------------------------------------------------------- */
    if (op < 40) {
      /* Inserts */
      cdsa_insert_art(art, key_pool[idx], (void *)(intptr_t)val);
      insert_hashmap(map, key_pool[idx], (void *)(intptr_t)val);
      insert_skiplist(list, (double)val, key_pool[idx]);
    } else if (op < 70) {
      /* Searches */
      cdsa_search_art(art, key_pool[idx]);
      get_hashmap(map, key_pool[idx]);
    } else {
      /* Deletions */
      cdsa_delete_art(art, key_pool[idx]);
      remove_hashmap(map, key_pool[idx]);
      remove_skiplist(list, (double)val, key_pool[idx]);
    }

    /* -------------------------------------------------------------
     * 2. Linear & Heap Containers: Vector, Deque, Priority Queue
     * ------------------------------------------------------------- */
    if (op % 2 == 0) {
      cdsa_push_vector(vec, &val);
      cdsa_push_back_deque(dq, &val);
      cdsa_push_pq(pq, &val);
    } else {
      if (cdsa_size_vector(vec) > 0) {
        cdsa_pop_vector(vec);
      }
      if (cdsa_size_deque(dq) > 0) {
        cdsa_pop_front_deque(dq);
      }
      if (cdsa_size_pq(pq) > 0) {
        int popped;
        cdsa_pop_pq(pq, &popped);
      }
    }

    /* -------------------------------------------------------------
     * 3. Dynamic String Operations (KString)
     * ------------------------------------------------------------- */
    if (op % 5 == 0) {
      append_kstring(str, key_pool[idx % 100]);
      if (cdsa_size_kstring(str) > 1000) {
        cdsa_clear_kstring(str);
      }
    }
  }

  printf("\n[Fuzzer] Survived %d chaotic operations without a crash!\n",
         FUZZ_OPS);

  /* Clean Teardown */
  cdsa_free_art(art);
  cdsa_free_hashmap(map);
  cdsa_free_vector(vec);
  cdsa_free_skiplist(list);
  cdsa_free_pq(pq);
  cdsa_free_deque(dq);
  cdsa_free_kstring(str);

  printf("[System] Post-Teardown Global Remaining Mem: %.2f MB\n",
         cdsa_global_memory_used / (1024.0 * 1024.0));

  if (cdsa_global_memory_used == 0) {
    printf("[SUCCESS] Entire CDSA suite is 100%% memory-tight!\n");
  } else {
    printf("[WARNING] Memory imbalance detected during teardown.\n");
  }

  return 0;
}

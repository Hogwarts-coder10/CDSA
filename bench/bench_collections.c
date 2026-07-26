#define _POSIX_C_SOURCE 199309L
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// CDSA Headers
#include "CDSA/art.h"
#include "CDSA/hashmap.h"
#include "CDSA/linkedlist.h"
#include "CDSA/priority_queue.h"
#include "CDSA/skiplist.h"
#include "CDSA/vector.h"

#define NUM_OPS 1000000

// --- Utilities ---
double get_time_sec(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec + ts.tv_nsec / 1e9;
}

bool pq_cmp_max(void *a, void *b) {
  return (*(intptr_t *)a) > (*(intptr_t *)b);
}

// ==========================================
// 1. ART BENCHMARKS
// ==========================================
cdsa_art_tree *bench_art_insert(void) {
  cdsa_art_tree *tree = cdsa_create_art();
  char key[32];

  double start = get_time_sec();
  for (int i = 0; i < NUM_OPS; i++) {
    snprintf(key, sizeof(key), "key:%d", i);
    cdsa_insert_art(tree, key, (void *)(intptr_t)i);
  }
  double duration = get_time_sec() - start;

  printf("[ART] Insert:        %.4f seconds (%8.0f ops/sec)\n", duration,
         NUM_OPS / duration);
  return tree;
}

void bench_art_search(cdsa_art_tree *tree) {
  char key[32];
  volatile void *val;

  double start = get_time_sec();
  for (int i = 0; i < NUM_OPS; i++) {
    snprintf(key, sizeof(key), "key:%d", i);
    val = cdsa_search_art(tree, key);
  }
  double duration = get_time_sec() - start;

  printf("[ART] Search:        %.4f seconds (%8.0f ops/sec)\n", duration,
         NUM_OPS / duration);
}

// ==========================================
// 2. HASHMAP BENCHMARKS
// ==========================================
cdsa_hashmap *bench_hashmap_insert(void) {
  cdsa_hashmap *map = cdsa_create_hashmap(1024);
  char key[32];

  double start = get_time_sec();
  for (int i = 0; i < NUM_OPS; i++) {
    snprintf(key, sizeof(key), "key:%d", i);
    insert_hashmap(map, key, (void *)(intptr_t)i);
  }
  double duration = get_time_sec() - start;

  printf("[HashMap] Insert:    %.4f seconds (%8.0f ops/sec)\n", duration,
         NUM_OPS / duration);
  return map;
}

void bench_hashmap_search(cdsa_hashmap *map) {
  char key[32];
  volatile void *val;

  double start = get_time_sec();
  for (int i = 0; i < NUM_OPS; i++) {
    snprintf(key, sizeof(key), "key:%d", i);
    val = get_hashmap(map, key);
  }
  double duration = get_time_sec() - start;

  printf("[HashMap] Search:    %.4f seconds (%8.0f ops/sec)\n", duration,
         NUM_OPS / duration);
}

// ==========================================
// 3. SKIPLIST BENCHMARKS
// ==========================================
void bench_skiplist_insert(void) {
  cdsa_skiplist *sl = cdsa_create_skiplist();
  char key[32];

  double start = get_time_sec();
  for (int i = 0; i < NUM_OPS; i++) {
    snprintf(key, sizeof(key), "key:%d", i);
    // Casting i to double for the score
    insert_skiplist(sl, (double)i, key);
  }
  double duration = get_time_sec() - start;

  printf("[SkipList] Insert:   %.4f seconds (%8.0f ops/sec)\n", duration,
         NUM_OPS / duration);
  cdsa_free_skiplist(sl);
}

// ==========================================
// 4. SEQUENTIAL BENCHMARKS
// ==========================================
void bench_vector_push(void) {
  cdsa_vector *vec = cdsa_create_vector(sizeof(intptr_t));

  double start = get_time_sec();
  for (intptr_t i = 0; i < NUM_OPS; i++) {
    cdsa_push_vector(vec, &i);
  }
  double duration = get_time_sec() - start;

  printf("[Vector] Push Back:  %.4f seconds (%8.0f ops/sec)\n", duration,
         NUM_OPS / duration);
  cdsa_free_vector(vec);
}

void bench_linkedlist_push(void) {
  cdsa_linkedlist *list = cdsa_create_linkedlist(sizeof(intptr_t));

  double start = get_time_sec();
  for (intptr_t i = 0; i < NUM_OPS; i++) {
    cdsa_push_front_linkedlist(list, &i);
  }
  double duration = get_time_sec() - start;

  printf("[LinkedList] Push:   %.4f seconds (%8.0f ops/sec)\n", duration,
         NUM_OPS / duration);
  cdsa_free_linkedlist(list);
}

void bench_pq_push(void) {
  cdsa_priority_queue *pq = cdsa_create_pq(sizeof(intptr_t), pq_cmp_max);

  // Seed random so we force the heap to constantly sift up and rebalance
  srand(42);

  double start = get_time_sec();
  for (intptr_t i = 0; i < NUM_OPS; i++) {
    intptr_t val = rand() % NUM_OPS;
    cdsa_push_pq(pq, &val);
  }
  double duration = get_time_sec() - start;

  printf("[Pri-Queue] Push:    %.4f seconds (%8.0f ops/sec)\n", duration,
         NUM_OPS / duration);
  cdsa_free_pq(pq);
}

void bench_hashmap_load_factors(void) {
  // 1. Lock the capacity at 2 Million to prevent auto-resizing
  size_t fixed_capacity = 2000000;
  cdsa_hashmap *map = cdsa_create_hashmap(fixed_capacity);

  // 2. Define our stress-test checkpoints
  double load_factors[] = {0.10, 0.30, 0.50, 0.70, 0.74};
  int num_factors = 5;
  int current_elements = 0;
  char key[32];
  volatile void *val;

  // 3. Pre-allocate memory for 500,000 random queries
  int search_ops = 500000;
  char **queries = malloc(search_ops * sizeof(char *));
  for (int s = 0; s < search_ops; s++) {
    queries[s] = malloc(32);
  }

  printf("\n====================================================\n");
  printf(" HASHMAP LOAD FACTOR STRESS TEST (Linear Probing)\n");
  printf("====================================================\n\n");

  for (int i = 0; i < num_factors; i++) {
    int target_elements = (int)(fixed_capacity * load_factors[i]);

    // A. Fill the map up to the exact target load factor
    for (; current_elements < target_elements; current_elements++) {
      snprintf(key, sizeof(key), "key:%d", current_elements);
      insert_hashmap(map, key, (void *)(intptr_t)current_elements);
    }

    // B. Pre-generate the random search strings to remove snprintf overhead
    for (int s = 0; s < search_ops; s++) {
      int random_target = rand() % current_elements;
      snprintf(queries[s], 32, "key:%d", random_target);
    }

    // C. Start the high-precision timer
    double start = get_time_sec();

    for (int s = 0; s < search_ops; s++) {
      val = get_hashmap(map, queries[s]);
    }

    double duration = get_time_sec() - start;
    double ops_sec = search_ops / duration;

    printf("Load Factor %2.0f%% | Size: %7d | Search: %.4fs (%8.0f ops/sec)\n",
           load_factors[i] * 100, current_elements, duration, ops_sec);
  }

  // Cleanup
  for (int s = 0; s < search_ops; s++) {
    free(queries[s]);
  }
  free(queries);
  cdsa_free_hashmap(map);
}

// ==========================================
// MAIN RUNNER
// ==========================================
int main(void) {
  printf("====================================================\n");
  printf(" CDSA PERFORMANCE BENCHMARKS (%d Operations)\n", NUM_OPS);
  printf("====================================================\n\n");

  // 1. Complex Structures

  // cdsa_art_tree *art_tree = bench_art_insert();
  // bench_art_search(art_tree);

  // cdsa_hashmap *h_map = bench_hashmap_insert();
  //  bench_hashmap_search(h_map);

  // bench_skiplist_insert();

  // printf("\n----------------------------------------------------\n\n");

  // 2. Linear & Heap Structures
  // bench_vector_push();
  // bench_linkedlist_push();
  // bench_pq_push();

  printf("\n----------------------------------------------------\n\n");

  bench_hashmap_load_factors();

  printf("\n====================================================\n");

  // Cleanup
  // cdsa_free_art(art_tree);
  // cdsa_free_hashmap(h_map);

  return 0;
}

#define _POSIX_C_SOURCE 199309L
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "CDSA/allocator.h"
#include "CDSA/art.h"
#include "CDSA/hashmap.h"

#define NUM_OPS 1000000

double get_time_sec(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec + ts.tv_nsec / 1e9;
}

// Generates chaotic keys to stress path compression and hashing
void generate_random_keys(char **keys, int count) {
  srand(42);
  for (int i = 0; i < count; i++) {
    keys[i] = malloc(32);
    snprintf(keys[i], 32, "uuid-%08x-%04x", rand(), rand() % 10000);
  }
}

void run_advanced_telemetry() {
  printf("====================================================\n");
  printf(" ADVANCED WORKLOAD SIMULATOR (%d Ops)\n", NUM_OPS);
  printf("====================================================\n\n");

  // Pre-allocate keys to isolate raw engine speed
  char **keys = malloc(NUM_OPS * sizeof(char *));
  generate_random_keys(keys, NUM_OPS);

  // --- 1. MEMORY & RANDOM INSERT TEST ---
  cdsa_art_tree *tree = cdsa_create_art();

  size_t mem_before = cdsa_global_memory_used;
  double start = get_time_sec();
  for (int i = 0; i < NUM_OPS; i++) {
    cdsa_insert_art(tree, keys[i], (void *)(intptr_t)i);
  }
  double insert_time = get_time_sec() - start;
  size_t mem_after = cdsa_global_memory_used;

  printf("[ART] Random Insert:  %.4fs (%8.0f ops/sec)\n", insert_time,
         NUM_OPS / insert_time);
  printf("[ART] Heap Footprint: %.2f MB\n",
         (mem_after - mem_before) / (1024.0 * 1024.0));

  // --- 2. MIXED WORKLOAD (50% Insert, 50% Search) ---
  cdsa_hashmap *map = cdsa_create_hashmap(1024);
  volatile void *val;
  int search_hits = 0;

  start = get_time_sec();
  for (int i = 0; i < NUM_OPS; i++) {
    if (rand() % 2 == 0) {
      insert_hashmap(map, keys[i], (void *)(intptr_t)i);
    } else {
      // Attempt to search a key that may or may not exist yet
      val = get_hashmap(map, keys[i]);
      if (val)
        search_hits++;
    }
  }
  double mixed_time = get_time_sec() - start;

  printf("\n[HashMap] Mixed R/W:  %.4fs (%8.0f ops/sec)\n", mixed_time,
         NUM_OPS / mixed_time);

  // --- 3. TEARDOWN (DELETE) BENCHMARK ---
  start = get_time_sec();
  for (int i = 0; i < NUM_OPS; i++) {
    cdsa_delete_art(tree, keys[i]);
  }
  double delete_time = get_time_sec() - start;

  printf("\n[ART] Teardown:       %.4fs (%8.0f ops/sec)\n", delete_time,
         NUM_OPS / delete_time);

  // Validate we didn't leak memory during deletion
  printf("[System] Remaining Mem:  %.2f MB\n",
         cdsa_global_memory_used / (1024.0 * 1024.0));

  // Cleanup
  cdsa_free_art(tree);
  cdsa_free_hashmap(map);
  for (int i = 0; i < NUM_OPS; i++)
    free(keys[i]);
  free(keys);
}

int main(void) {
  run_advanced_telemetry();
  return 0;
}

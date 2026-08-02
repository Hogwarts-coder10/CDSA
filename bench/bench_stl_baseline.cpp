#include <chrono>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

extern "C" {
#include "CDSA/art.h"
#include "CDSA/hashmap.h"
}

#define NUM_OPS 1000000

double get_time_sec(
    std::chrono::time_point<std::chrono::high_resolution_clock> start) {
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> diff = end - start;
  return diff.count();
}

void generate_random_keys(std::vector<std::string> &keys, int count) {
  srand(42);
  for (int i = 0; i < count; i++) {
    char buf[32];
    snprintf(buf, sizeof(buf), "uuid-%08x-%04x", rand(), rand() % 10000);
    keys.push_back(std::string(buf));
  }
}

int main() {
  std::cout << "====================================================\n";
  std::cout << " THE RIVAL BASELINE: CDSA vs C++ STL (" << NUM_OPS << " Ops)\n";
  std::cout << "====================================================\n\n";

  std::vector<std::string> keys;
  generate_random_keys(keys, NUM_OPS);

  // ==========================================
  // HASHMAP vs STD::UNORDERED_MAP
  // ==========================================
  std::cout << "--- Hash Table Drag Race ---\n";

  // 1. C++ std::unordered_map
  std::unordered_map<std::string, int> stl_map;
  stl_map.reserve(NUM_OPS);
  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < NUM_OPS; i++) {
    stl_map[keys[i]] = i;
  }
  double stl_hash_time = get_time_sec(start);
  std::cout << "[C++ STL] std::unordered_map: " << stl_hash_time << "s ("
            << (NUM_OPS / stl_hash_time) << " ops/sec)\n";

  // 2. CDSA HashMap
  cdsa_hashmap *c_map = cdsa_create_hashmap(2000000);
  start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < NUM_OPS; i++) {
    insert_hashmap(c_map, keys[i].c_str(), (void *)(intptr_t)i);
  }
  double cdsa_hash_time = get_time_sec(start);
  std::cout << "[CDSA C]  cdsa_hashmap:       " << cdsa_hash_time << "s ("
            << (NUM_OPS / cdsa_hash_time) << " ops/sec)\n\n";

  // ==========================================
  // TREES: ART vs STD::MAP (Red-Black Tree)
  // ==========================================
  std::cout << "--- Tree/Index Drag Race ---\n";

  // 1. C++ std::map
  std::map<std::string, int> stl_tree;
  start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < NUM_OPS; i++) {
    stl_tree[keys[i]] = i;
  }
  double stl_tree_time = get_time_sec(start);
  std::cout << "[C++ STL] std::map (RB Tree): " << stl_tree_time << "s ("
            << (NUM_OPS / stl_tree_time) << " ops/sec)\n";

  // 2. CDSA ART Engine
  cdsa_art_tree *c_art = cdsa_create_art();
  start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < NUM_OPS; i++) {
    cdsa_insert_art(c_art, keys[i].c_str(), (void *)(intptr_t)i);
  }
  double cdsa_art_time = get_time_sec(start);
  std::cout << "[CDSA C]  cdsa_art_tree:      " << cdsa_art_time << "s ("
            << (NUM_OPS / cdsa_art_time) << " ops/sec)\n";

  // Cleanup
  cdsa_free_hashmap(c_map);
  cdsa_free_art(c_art);

  return 0;
}

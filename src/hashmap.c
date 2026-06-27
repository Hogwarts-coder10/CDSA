#include "CDSA/hashmap.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOMBSTONE ((char *)-1)

HashMap *create_hashmap(size_t capacity) {
  HashMap *map = malloc(sizeof(HashMap));
  map->size = 0;

  map->capacity = capacity;

  map->entries = calloc(capacity, sizeof(HashEntry));

  return map;
}

void free_hashmap(HashMap *map) {
  free(map->entries);
  free(map);
}

size_t hash_function(const char *key, size_t capacity) {
  size_t hash = 0;
  while (*key != '\0') {
    hash = (hash * 31) + *key;
    key++;
  }
  return hash % capacity;
}

void insert_hashmap(HashMap *map, const char *key, void *value) {
  /* Stop infinite probing loops before we have resize function
   * We trigger this when hashmap is 75% full */

  if (map->size >= (map->capacity * 3) / 4) {
    resize_hashmap(map);
    insert_hashmap(map, key, value);
    return;
  }

  size_t index = hash_function(key, map->capacity);

  // Look for empty slot or the exact same slot
  while (map->entries[index].key != NULL) {
    // If the key already exists, UPDATE the value and exit.
    if (map->entries[index].key != TOMBSTONE &&
        strcmp(map->entries[index].key, key) == 0) {
      map->entries[index].value = value;
      return; // Do NOT increment size!
    }

    printf("probing index = %zu\n", index);
    index = (index + 1) % map->capacity;
  }

  // we found a empty slot a.k.a we insert now
  map->entries[index].key = (char *)key;
  map->entries[index].value = value;
  map->size++;
}

void print_hashmap(HashMap *map) {
  for (size_t i = 0; i < map->capacity; i++) {
    // THE SHIELD: Only print if it's not NULL and not a TOMBSTONE
    if (map->entries[i].key != NULL && map->entries[i].key != TOMBSTONE) {

      printf("[%zu] %s -> %d\n", i, map->entries[i].key,
             *(int *)map->entries[i].value);
    }
  }
}

void *get_hashmap(HashMap *map, const char *key) {
  size_t index = hash_function(key, map->capacity);

  // Probe until we hit an empty slot
  while (map->entries[index].key != NULL) {
    // THE SHIELD: We MUST check that the key is NOT a tombstone
    // BEFORE we allow strcmp to run!

    if (map->entries[index].key != TOMBSTONE &&
        strcmp(map->entries[index].key, key) == 0) {
      return map->entries[index].value;
    }

    // Otherwise, keep probing
    index = (index + 1) % map->capacity;
  }

  // If we hit a NULL key, the item doesn't exist
  return NULL;
}

bool contains_hashmap(HashMap *map, const char *key) {
  // A simple wrapper: if get_hashmap returns anything other than NULL, it
  // exists.
  return get_hashmap(map, key) != NULL;
}

void resize_hashmap(HashMap *map) {
  size_t old_capacity = map->capacity;
  HashEntry *old_entries = map->entries;

  map->capacity *= 2;
  map->entries = calloc(map->capacity, sizeof(HashEntry));
  map->size = 0;

  // 4. Rehash all VALID entries into the new array
  for (size_t i = 0; i < old_capacity; i++) {

    // THE SHIELD: Do not re-hash NULLs or TOMBSTONES
    if (old_entries[i].key != NULL && old_entries[i].key != TOMBSTONE) {
      insert_hashmap(map, old_entries[i].key, old_entries[i].value);
    }
  }

  free(old_entries);
  printf("[System] HashMap resized to capacity: %zu\n", map->capacity);
}

void remove_hashmap(HashMap *map, const char *key) {
  size_t index = hash_function(key, map->capacity);

  // Probe until we hit a completely empty slot
  while (map->entries[index].key != NULL) {

    // We must ensure the current slot is NOT a tombstone before checking strcmp
    if (map->entries[index].key != TOMBSTONE &&
        strcmp(map->entries[index].key, key) == 0) {

      // Found it! Mark it as a tombstone.
      map->entries[index].key = TOMBSTONE;
      map->entries[index].value = NULL;
      map->size--;
      return;
    }

    index = (index + 1) % map->capacity;
  }
}

#include "CDSA/hashmap.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOMBSTONE ((char *)-1)

struct HashEntry {
  char *key;
  void *value;
};

struct HashMap {
  size_t capacity;
  size_t size;
  size_t occupied;
  struct HashEntry *entries;
};

HashMap *create_hashmap(size_t capacity) {
  HashMap *map = malloc(sizeof(HashMap));

  if (map == NULL) {
    return NULL;
  }

  if (capacity == 0) {
    capacity = 1;
  }

  map->size = 0;
  map->occupied = 0;
  map->capacity = capacity;

  map->entries = calloc(capacity, sizeof(HashEntry));

  if (map->entries == NULL) {
    free(map);
    return NULL;
  }

  return map;
}

void free_hashmap(HashMap *map) {
  if (map == NULL)
    return;
  free(map->entries);
  free(map);
}

size_t size_hashmap(HashMap *map) { return map->size; }

size_t hash_function(const char *key, size_t capacity) {
  size_t hash = 0;
  while (*key != '\0') {
    hash = (hash * 31) + *key;
    key++;
  }
  return hash % capacity;
}

bool insert_hashmap(HashMap *map, const char *key, void *value) {
  if (map->occupied >= (map->capacity * 3) / 4) {
    if (!resize_hashmap(map)) {
      // OOM and couldn't grow. Don't recurse - that's how you get a
      // stack overflow under sustained memory pressure. Drop the insert
      // and let the caller know via a return value if you add one later.
      return false;
    }
  }

  size_t index = hash_function(key, map->capacity);
  size_t first_tombstone = (size_t)-1; // no tombstone seen yet

  while (map->entries[index].key != NULL) {
    if (map->entries[index].key == TOMBSTONE) {
      if (first_tombstone == (size_t)-1)
        first_tombstone = index; // remember first reusable slot
    } else if (strcmp(map->entries[index].key, key) == 0) {
      map->entries[index].value = value; // update existing key
      return true;
    }
    index = (index + 1) % map->capacity;
  }

  // Prefer reusing a tombstone slot over consuming a fresh NULL slot
  if (first_tombstone != (size_t)-1) {
    index = first_tombstone;
    // occupied unchanged — tombstone was already counted
  } else {
    map->occupied++; // claiming a genuinely fresh NULL slot
  }

  map->entries[index].key = (char *)key;
  map->entries[index].value = value;
  map->size++;
  return true;
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

bool resize_hashmap(HashMap *map) {
  size_t old_capacity = map->capacity;
  HashEntry *old_entries = map->entries;

  size_t new_capacity = map->capacity * 2;
  HashEntry *new_entries = calloc(new_capacity, sizeof(HashEntry));

  if (new_entries == NULL) {
    printf("[System] Warning: HashMap resize failed due to OOM.\n");
    return false;
  }

  // Swap the new array in BEFORE rehashing, or insert_hashmap below
  // will see the old capacity/size and immediately re-trigger a resize.
  map->entries = new_entries;
  map->capacity = new_capacity;
  map->size = 0;

  for (size_t i = 0; i < old_capacity; i++) {
    if (old_entries[i].key != NULL && old_entries[i].key != TOMBSTONE) {
      insert_hashmap(map, old_entries[i].key, old_entries[i].value);
    }
  }

  map->occupied = map->size; // tombstones are gone after rebuild

  free(old_entries);
  printf("[System] HashMap resized to capacity: %zu\n", map->capacity);
  return true;
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

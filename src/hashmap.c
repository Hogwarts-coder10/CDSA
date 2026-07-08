#include "CDSA/hashmap.h"
#include "CDSA/allocator.h"
#include "CDSA/error.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOMBSTONE ((char *)-1)

typedef struct {
  char *key;
  void *value;
} HashEntry;

struct HashMap {
  size_t capacity;
  size_t size;
  size_t occupied;
  HashEntry *entries;
};

HashMap *create_hashmap(size_t capacity) {
  HashMap *map = CDSA_MALLOC(sizeof(HashMap));

  if (map == NULL) {
    return NULL;
  }

  if (capacity == 0) {
    capacity = 1;
  }

  map->size = 0;
  map->occupied = 0;
  map->capacity = capacity;

  map->entries = CDSA_CALLOC(capacity, sizeof(HashEntry));

  if (map->entries == NULL) {
    CDSA_FREE(map); // Fixed here
    return NULL;
  }

  return map;
}

void free_hashmap(HashMap *map) {
  if (map == NULL)
    return;
  CDSA_FREE(map->entries);
  CDSA_FREE(map);
}

size_t size_hashmap(HashMap *map) {
  if (map == NULL)
    return 0;
  return map->size;
}

static size_t hash_function(const char *key, size_t capacity) {
  size_t hash = 0;
  while (*key != '\0') {
    hash = (hash * 31) + *key;
    key++;
  }
  return hash % capacity;
}

CDSA_STATUS insert_hashmap(HashMap *map, const char *key, void *value) {
  if (map == NULL || key == NULL) {
    return CDSA_ERR_INVALID;
  }

  if (map->occupied >= (map->capacity * 3) / 4) {
    CDSA_STATUS status = resize_hashmap(map);
    if (status != CDSA_OK) {
      // OOM and couldn't grow. Propagate the error explicitly.
      return status;
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
      return CDSA_OK;
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

  return CDSA_OK;
}

void print_hashmap(HashMap *map) {
  if (map == NULL)
    return;

  for (size_t i = 0; i < map->capacity; i++) {
    // THE SHIELD: Only print if it's not NULL and not a TOMBSTONE
    if (map->entries[i].key != NULL && map->entries[i].key != TOMBSTONE) {
      printf("[%zu] %s -> %d\n", i, map->entries[i].key,
             *(int *)map->entries[i].value);
    }
  }
}

void *get_hashmap(HashMap *map, const char *key) {
  if (map == NULL || key == NULL)
    return NULL;

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

CDSA_STATUS resize_hashmap(HashMap *map) {
  if (map == NULL)
    return CDSA_ERR_INVALID;

  size_t old_capacity = map->capacity;
  HashEntry *old_entries = map->entries;

  size_t new_capacity = map->capacity * 2;
  HashEntry *new_entries = CDSA_CALLOC(new_capacity, sizeof(HashEntry));

  if (new_entries == NULL) {
    printf("[System] Warning: HashMap resize failed due to OOM.\n");
    return CDSA_ERR_OOM;
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

  CDSA_FREE(old_entries); // Fixed here
  printf("[System] HashMap resized to capacity: %zu\n", map->capacity);
  return CDSA_OK;
}

CDSA_STATUS remove_hashmap(HashMap *map, const char *key) {
  if (map == NULL || key == NULL)
    return CDSA_ERR_INVALID;

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
      return CDSA_OK;
    }

    index = (index + 1) % map->capacity;
  }

  return CDSA_ERR_NOT_FOUND;
}

// --- Iterator Implementation ---

struct HashMapIterator {
  HashMap *map;
  size_t current_index;
};

HashMapIterator *create_hashmap_iterator(HashMap *map) {
  if (map == NULL)
    return NULL;

  HashMapIterator *iter = CDSA_MALLOC(sizeof(HashMapIterator));
  if (iter == NULL)
    return NULL;

  iter->map = map;
  iter->current_index = 0;
  return iter;
}

bool has_next_hashmap(HashMapIterator *iter) {
  if (iter == NULL || iter->map == NULL)
    return false;

  // Peek ahead to find the next slot that isn't NULL and isn't a TOMBSTONE
  while (iter->current_index < iter->map->capacity) {
    char *key = iter->map->entries[iter->current_index].key;
    if (key != NULL && key != TOMBSTONE) {
      return true; // Found valid data!
    }
    iter->current_index++;
  }

  return false; // Reached the end of the capacity
}

CDSA_STATUS next_hashmap(HashMapIterator *iter, const char **out_key,
                         void **out_value) {
  if (iter == NULL || out_key == NULL)
    return CDSA_ERR_INVALID;

  // has_next automatically advances current_index to the next valid slot
  if (!has_next_hashmap(iter)) {
    return CDSA_ERR_NOT_FOUND;
  }

  // Extract the data
  *out_key = iter->map->entries[iter->current_index].key;
  if (out_value != NULL) {
    *out_value = iter->map->entries[iter->current_index].value;
  }

  // Advance the index so the next call doesn't read the same element
  iter->current_index++;

  return CDSA_OK;
}

void free_hashmap_iterator(HashMapIterator *iter) {
  if (iter == NULL)
    return;
  CDSA_FREE(iter);
}

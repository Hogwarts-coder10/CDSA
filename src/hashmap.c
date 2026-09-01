#include "CDSA/hashmap.h"
#include "CDSA/allocator.h"
#include "CDSA/error.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char *key;
  void *value;
  size_t
      hash; // Catches the original hash to avoid recalculating during evictions
  size_t psl; // Probe Sequence Length (PSL) ("wealth indicator")
} HashEntry;

struct cdsa_hashmap {
  size_t capacity;
  size_t size;
  size_t max_psl; // Cheat code: the highest PSL in the map for early exit on
                  // lookups
  size_t version;
  HashEntry *entries;
};

cdsa_hashmap *cdsa_create_hashmap(size_t capacity) {
  cdsa_hashmap *map = CDSA_MALLOC(sizeof(cdsa_hashmap));

  if (map == NULL) {
    return NULL;
  }

  if (capacity == 0) {
    capacity = 1;
  }

  map->size = 0;
  map->max_psl = 0;
  map->capacity = capacity;
  map->version = 0;

  map->entries = CDSA_CALLOC(capacity, sizeof(HashEntry));

  if (map->entries == NULL) {
    CDSA_FREE(map); // Fixed here
    return NULL;
  }

  return map;
}

void cdsa_free_hashmap(cdsa_hashmap *map) {
  if (map == NULL)
    return;
  CDSA_FREE(map->entries);
  CDSA_FREE(map);
}

size_t cdsa_size_hashmap(const cdsa_hashmap *map) {
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

CDSA_STATUS insert_hashmap(cdsa_hashmap *map, const char *key, void *value) {
  if (map == NULL || key == NULL) {
    return CDSA_ERR_INVALID;
  }

  // We only check against map->size now
  if (map->size >= (map->capacity * 3) / 4) {
    CDSA_STATUS status = resize_hashmap(map);
    if (status != CDSA_OK) {
      return status;
    }
  }

  size_t current_hash = hash_function(key, map->capacity);
  size_t index = current_hash;

  // Create our incoming entry with a starting PSL of 0
  HashEntry incoming = {(char *)key, value, current_hash, 0};

  while (true) {
    // Found an empty slot
    if (map->entries[index].key == NULL) {
      map->entries[index] = incoming;
      map->size++;

      // Update the global max_psl tracker
      if (incoming.psl > map->max_psl) {
        map->max_psl = incoming.psl;
      }
      return CDSA_OK;
    }

    // Update existing key
    if (strcmp(map->entries[index].key, incoming.key) == 0) {
      map->entries[index].value = incoming.value;
      return CDSA_OK;
    }

    // ROBIN HOOD SWAP: Take from the rich, give to the poor
    // If the incoming key has traveled further than the resident key, steal the
    // slot
    if (incoming.psl > map->entries[index].psl) {
      HashEntry temp = map->entries[index];
      map->entries[index] = incoming;
      incoming = temp; // The evicted resident becomes the new incoming key
    }

    // Move to the next slot and increase the probe sequence length
    index = (index + 1) % map->capacity;
    incoming.psl++;
  }
}

void print_hashmap(cdsa_hashmap *map) {
  if (map == NULL)
    return;

  for (size_t i = 0; i < map->capacity; i++) {
    // THE SHIELD: Only print if it's not NULL and not a TOMBSTONE
    if (map->entries[i].key != NULL) {
      printf("[%zu] %s -> %d\n", i, map->entries[i].key,
             *(int *)map->entries[i].value);
    }
  }
}

void *get_hashmap(cdsa_hashmap *map, const char *key) {
  if (map == NULL || key == NULL) {
    return NULL;
  }

  size_t index = hash_function(key, map->capacity);
  size_t probe_distance = 0;

  // THE CHEAT CODE: Stop searching if we've probed further than the map's
  // max_psl
  while (map->entries[index].key != NULL && probe_distance <= map->max_psl) {

    if (strcmp(map->entries[index].key, key) == 0) {
      return map->entries[index].value;
    }

    index = (index + 1) % map->capacity;
    probe_distance++;
  }

  // If we hit a NULL or exceeded max_psl, the key definitively does not exist
  return NULL;
}

bool contains_hashmap(cdsa_hashmap *map, const char *key) {
  // A simple wrapper: if get_hashmap returns anything other than NULL, it
  // exists.
  return get_hashmap(map, key) != NULL;
}

CDSA_STATUS resize_hashmap(cdsa_hashmap *map) {
  if (map == NULL)
    return CDSA_ERR_INVALID;

  size_t old_capacity = map->capacity;
  HashEntry *old_entries = map->entries;

  size_t new_capacity = map->capacity * 2;
  HashEntry *new_entries = CDSA_CALLOC(new_capacity, sizeof(HashEntry));

  if (new_entries == NULL) {
    printf("[System] Warning: cdsa_hashmap resize failed due to OOM.\n");
    return CDSA_ERR_OOM;
  }

  // Swap the new array in BEFORE rehashing, or insert_hashmap below
  // will see the old capacity/size and immediately re-trigger a resize.
  map->entries = new_entries;
  map->capacity = new_capacity;
  map->size = 0;

  for (size_t i = 0; i < old_capacity; i++) {
    if (old_entries[i].key != NULL) {
      insert_hashmap(map, old_entries[i].key, old_entries[i].value);
    }
  }

  CDSA_FREE(old_entries);
  map->version++; // resizing invalidates all active Iterators
  printf("[System] cdsa_hashmap resized to capacity: %zu\n", map->capacity);
  return CDSA_OK;
}

CDSA_STATUS remove_hashmap(cdsa_hashmap *map, const char *key) {
  if (map == NULL || key == NULL) {
    return CDSA_ERR_INVALID;
  }

  size_t index = hash_function(key, map->capacity);
  size_t probe_distance = 0;

  // Search for the key, stopping early if we probe beyond the maximum known PSL
  while (map->entries[index].key != NULL && probe_distance <= map->max_psl) {

    if (strcmp(map->entries[index].key, key) == 0) {
      // Key found! Decrement size immediately.
      map->size--;

      size_t curr = index;
      size_t next = (curr + 1) % map->capacity;

      // SHIFT BACKWARDS: Pull elements back to fill the void
      // We only shift if the next element exists AND has been pushed from its
      // ideal slot (psl > 0)
      while (map->entries[next].key != NULL && map->entries[next].psl > 0) {
        map->entries[curr] = map->entries[next];
        map->entries[curr].psl--; // Distance to ideal slot decreased by 1

        curr = next;
        next = (curr + 1) % map->capacity;
      }

      // Clear the final vacated slot completely
      map->entries[curr].key = NULL;
      map->entries[curr].value = NULL;
      map->entries[curr].hash = 0;
      map->entries[curr].psl = 0;

      return CDSA_OK;
    }

    index = (index + 1) % map->capacity;
    probe_distance++;
  }

  return CDSA_ERR_NOT_FOUND;
}

// --- Iterator Implementation ---

struct cdsa_hashmap_iterator {
  const cdsa_hashmap *map;
  size_t current_index;
  size_t snapshot_version;
};

cdsa_hashmap_iterator *cdsa_create_hashmap_iterator(const cdsa_hashmap *map) {
  if (map == NULL)
    return NULL;

  cdsa_hashmap_iterator *iter = CDSA_MALLOC(sizeof(cdsa_hashmap_iterator));
  if (iter == NULL)
    return NULL;

  iter->map = map;
  iter->current_index = 0;
  iter->snapshot_version = map->version;
  return iter;
}

bool cdsa_has_next_hashmap(cdsa_hashmap_iterator *iter) {
  if (iter == NULL || iter->map == NULL)
    return false;

  if (iter->map->version != iter->snapshot_version) {
    return false;
  }

  // WARNING: This function has side effects! It advances current_index
  // past empty space or tombstones to find the next valid element.
  // Peek ahead to find the next slot that isn't NULL and isn't a TOMBSTONE
  while (iter->current_index < iter->map->capacity) {
    char *key = iter->map->entries[iter->current_index].key;
    if (key != NULL) {
      return true; // Found valid data!
    }
    iter->current_index++;
  }

  return false; // Reached the end of the capacity
}

CDSA_STATUS cdsa_next_hashmap(cdsa_hashmap_iterator *iter, const char **out_key,
                              void **out_value) {
  if (iter == NULL || out_key == NULL)
    return CDSA_ERR_INVALID;

  if (iter->map->version != iter->snapshot_version) {
    return CDSA_ERR_ITER_INVALIDATED;
  }
  // has_next automatically advances current_index to the next valid slot
  if (!cdsa_has_next_hashmap(iter)) {
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

void cdsa_free_hashmap_iterator(cdsa_hashmap_iterator *iter) {
  if (iter == NULL)
    return;
  CDSA_FREE(iter);
}

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
  size_t hash; // Now stores raw integer hash to completely bypass strcmp on
               // collisions and rehashing on resizes
  size_t psl;  // Probe Sequence Length (PSL) ("wealth indicator")
} HashEntry;

struct cdsa_hashmap {
  size_t capacity;
  size_t size;
  size_t max_psl; // Cheat code: the highest PSL in the map for early exit on
                  // lookups
  size_t version;
  HashEntry *entries;
};

// Forward declarations for cross-dependencies
CDSA_STATUS resize_hashmap(cdsa_hashmap *map);
static CDSA_STATUS internal_insert(cdsa_hashmap *map, const char *key,
                                   void *value, size_t raw_hash);

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
    CDSA_FREE(map);
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

// ⚡ Generates the pure hash without the modulo penalty
static size_t get_raw_hash(const char *key) {
  size_t hash = 0;
  while (*key != '\0') {
    hash = (hash * 31) + *key;
    key++;
  }
  return hash;
}

// ⚡ Fast-path insertion that accepts a pre-computed raw hash
static CDSA_STATUS internal_insert(cdsa_hashmap *map, const char *key,
                                   void *value, size_t raw_hash) {
  size_t index = raw_hash % map->capacity;
  HashEntry incoming = {(char *)key, value, raw_hash, 0};

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

    // ⚡ FAST PATH: Integer check before string comparison
    if (map->entries[index].hash == incoming.hash &&
        strcmp(map->entries[index].key, incoming.key) == 0) {
      map->entries[index].value = incoming.value;
      return CDSA_OK;
    }

    // ROBIN HOOD SWAP
    if (incoming.psl > map->entries[index].psl) {
      HashEntry temp = map->entries[index];
      map->entries[index] = incoming;
      incoming = temp;
    }

    index = (index + 1) % map->capacity;
    incoming.psl++;
  }
}

CDSA_STATUS insert_hashmap(cdsa_hashmap *map, const char *key, void *value) {
  if (map == NULL || key == NULL) {
    return CDSA_ERR_INVALID;
  }

  if (map->size >= (map->capacity * 3) / 4) {
    CDSA_STATUS status = resize_hashmap(map);
    if (status != CDSA_OK) {
      return status;
    }
  }

  size_t raw_hash = get_raw_hash(key);
  return internal_insert(map, key, value, raw_hash);
}

void print_hashmap(cdsa_hashmap *map) {
  if (map == NULL)
    return;

  for (size_t i = 0; i < map->capacity; i++) {
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

  size_t raw_hash = get_raw_hash(key);
  size_t index = raw_hash % map->capacity;
  size_t probe_distance = 0;

  while (map->entries[index].key != NULL && probe_distance <= map->max_psl) {
    // ⚡ FAST PATH: Only run strcmp if the integer hashes collide
    if (map->entries[index].hash == raw_hash &&
        strcmp(map->entries[index].key, key) == 0) {
      return map->entries[index].value;
    }

    index = (index + 1) % map->capacity;
    probe_distance++;
  }

  return NULL;
}

bool contains_hashmap(cdsa_hashmap *map, const char *key) {
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

  map->entries = new_entries;
  map->capacity = new_capacity;
  map->size = 0;

  for (size_t i = 0; i < old_capacity; i++) {
    if (old_entries[i].key != NULL) {
      // ⚡ FAST PATH: Feed cached hash directly to avoid rehashing strings
      internal_insert(map, old_entries[i].key, old_entries[i].value,
                      old_entries[i].hash);
    }
  }

  CDSA_FREE(old_entries);
  map->version++;
  printf("[System] cdsa_hashmap resized to capacity: %zu\n", map->capacity);
  return CDSA_OK;
}

CDSA_STATUS remove_hashmap(cdsa_hashmap *map, const char *key) {
  if (map == NULL || key == NULL) {
    return CDSA_ERR_INVALID;
  }

  size_t raw_hash = get_raw_hash(key);
  size_t index = raw_hash % map->capacity;
  size_t probe_distance = 0;

  while (map->entries[index].key != NULL && probe_distance <= map->max_psl) {
    // ⚡ FAST PATH: Integer check before string comparison
    if (map->entries[index].hash == raw_hash &&
        strcmp(map->entries[index].key, key) == 0) {
      map->size--;

      size_t curr = index;
      size_t next = (curr + 1) % map->capacity;

      while (map->entries[next].key != NULL && map->entries[next].psl > 0) {
        map->entries[curr] = map->entries[next];
        map->entries[curr].psl--;

        curr = next;
        next = (curr + 1) % map->capacity;
      }

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

  while (iter->current_index < iter->map->capacity) {
    char *key = iter->map->entries[iter->current_index].key;
    if (key != NULL) {
      return true;
    }
    iter->current_index++;
  }

  return false;
}

CDSA_STATUS cdsa_next_hashmap(cdsa_hashmap_iterator *iter, const char **out_key,
                              void **out_value) {
  if (iter == NULL || out_key == NULL)
    return CDSA_ERR_INVALID;

  if (iter->map->version != iter->snapshot_version) {
    return CDSA_ERR_ITER_INVALIDATED;
  }

  if (!cdsa_has_next_hashmap(iter)) {
    return CDSA_ERR_NOT_FOUND;
  }

  *out_key = iter->map->entries[iter->current_index].key;
  if (out_value != NULL) {
    *out_value = iter->map->entries[iter->current_index].value;
  }

  iter->current_index++;
  return CDSA_OK;
}

void cdsa_free_hashmap_iterator(cdsa_hashmap_iterator *iter) {
  if (iter == NULL)
    return;
  CDSA_FREE(iter);
}

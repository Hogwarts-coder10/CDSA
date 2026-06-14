#include "CDSA/hashmap.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

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
  size_t index = hash_function(key, map->capacity);

  while (map->entries[index].key != NULL) {
    printf("probing index = %zu\n", index);
    index = (index + 1) % map->capacity;
  }

  map->entries[index].key = (char *)key;
  map->entries[index].value = value;
  map->size++;
}

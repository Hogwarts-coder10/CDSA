#include "CDSA/hashmap.h"
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

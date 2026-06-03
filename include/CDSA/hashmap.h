#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>

typedef struct {
  char *key;
  void *value;
} HashEntry;

typedef struct {
  HashEntry *entries;
  size_t size;
  size_t capacity;
} HashMap;

HashMap *create_hashmap(size_t capacity);
void free_hashmap(HashMap *hp);

#endif

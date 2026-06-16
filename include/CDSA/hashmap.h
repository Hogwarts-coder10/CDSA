#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdbool.h>
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
void free_hashmap(HashMap *map);
size_t hash_function(const char *key, size_t capacity);
void insert_hashmap(HashMap *map, const char *key, void *value);
void *get_hashmap(HashMap *map, const char *key);
bool contains_hashmap(HashMap *map, const char *key);
void remove_hashmap(HashMap *map, const char *key);
void print_hashmap(HashMap *map);
#endif

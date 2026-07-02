#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdbool.h>
#include <stddef.h>

typedef struct HashEntry HashEntry;
typedef struct HashMap HashMap;

HashMap *create_hashmap(size_t capacity);
void free_hashmap(HashMap *map);
size_t hash_function(const char *key, size_t capacity);
bool insert_hashmap(HashMap *map, const char *key, void *value);
void *get_hashmap(HashMap *map, const char *key);
bool contains_hashmap(HashMap *map, const char *key);
void remove_hashmap(HashMap *map, const char *key);
void print_hashmap(HashMap *map);
bool resize_hashmap(HashMap *map);
void remove_hashmap(HashMap *map, const char *key);
size_t size_hashmap(HashMap *map);

#endif

#ifndef HASHMAP_H
#define HASHMAP_H

#include "CDSA/error.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct HashMap HashMap;

HashMap *create_hashmap(size_t capacity);
void free_hashmap(HashMap *map);

/**
 * @brief Inserts a key-value pair.
 *
 * @ownership
 * - KEY: Caller retains ownership. The string must outlive the hashmap.
 * - VALUE: Caller retains ownership. The hashmap only stores the pointer.
 * - MEMORY: The caller is responsible for freeing the key and value after
 *   removing them from the map or freeing the map itself.
 */
CDSA_STATUS insert_hashmap(HashMap *map, const char *key, void *value);

void *get_hashmap(HashMap *map, const char *key);
bool contains_hashmap(HashMap *map, const char *key);
CDSA_STATUS remove_hashmap(HashMap *map, const char *key);
void print_hashmap(HashMap *map);
CDSA_STATUS resize_hashmap(HashMap *map);
size_t size_hashmap(HashMap *map);

#endif

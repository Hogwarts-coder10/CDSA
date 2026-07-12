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

// --- Iterator API ---

// Opaque iterator type
typedef struct HashMapIterator HashMapIterator;

/**
 * @brief Creates a new iterator for the hashmap.
 * @warning The caller must free the iterator using free_hashmap_iterator.
 */
HashMapIterator *create_hashmap_iterator(HashMap *map);

/**
 * @brief Checks if there are more valid entries to read.
 * @note WARNING: This function modifies the internal state of the iterator
 * by advancing its index forward over tombstones and empty slots to find data.
 * @warning Returns false if the underlying map has been structuraly modified
 * (resized) since the iterator was created.
 */
bool has_next_hashmap(HashMapIterator *iter);

/**
 * @brief Advances the iterator and retrieves the next key-value pair.
 * @return CDSA_OK on success, or CDSA_ERR_ITER_INVALIDATED if a concurrent
 * mutation (resize) happened during the walk pass.
 */
CDSA_STATUS next_hashmap(HashMapIterator *iter, const char **out_key,
                         void **out_value);

/**
 * @brief Frees the iterator memory.
 */
void free_hashmap_iterator(HashMapIterator *iter);

#endif

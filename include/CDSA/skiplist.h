#ifndef SKIPLIST_H
#define SKIPLIST_H

#include "CDSA/error.h"
#include <stdbool.h>
#include <stddef.h>

#define SKIPLIST_MAX_LEVEL 16

typedef struct SkipList SkipList;

// LifeCycle
SkipList *cdsa_create_skiplist();
void cdsa_free_skiplist(SkipList *sl);

// Core operations

size_t cdsa_size_skiplist(SkipList *sl);
int level_skiplist(SkipList *sl);

// Returns true if inserted, false if it failed (like out of memory)
/**
 * @brief Inserts a score and value.
 *
 * @ownership
 * - VALUE: The library takes ownership by creating an internal deep copy
 * (strdup).
 * - MEMORY: The caller may safely free or modify their original string after
 *   insertion. The library will automatically free the internal copy upon
 * removal.
 */
CDSA_STATUS insert_skiplist(SkipList *sl, double score, const char *value);

// Returns true if found and deleted, false if it didn't exist
CDSA_STATUS remove_skiplist(SkipList *sl, double score, const char *value);

char **get_range_skiplist(SkipList *sl, double min_score, double max_score,
                          int *out_score);

// -- Iterartor API --
// Opaque Iterartor type
typedef struct SkipListIterator SkipListIterator;

/**
 * @brief creates a new Iterartor for SkipList
 * @warning the caller must free the Iterartor using cdsa_free_skiplist_iterator
 */

SkipListIterator *cdsa_create_skiplist_iterator(SkipList *sl);

/**
 * @brief Checks if there are more nodes to read.
 */
bool cdsa_has_next_skiplist(SkipListIterator *iter);

/**
 * @brief Advances the iterator and retrieves the next score and value.
 * @param out_score Pointer to store the score (optional, can be NULL).
 * @param out_value Pointer to store the value string (optional, can be NULL).
 */
CDSA_STATUS cdsa_next_skiplist(SkipListIterator *iter, double *out_score,
                          const char **out_value);

/**
 * @brief Frees the iterator memory.
 */
void cdsa_free_skiplist_iterator(SkipListIterator *iter);

#endif

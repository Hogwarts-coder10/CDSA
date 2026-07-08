#ifndef SKIPLIST_H
#define SKIPLIST_H

#include "CDSA/error.h"
#include <stdbool.h>
#include <stddef.h>

#define SKIPLIST_MAX_LEVEL 16

typedef struct SkipList SkipList;

// LifeCycle
SkipList *create_skiplist();
void free_skiplist(SkipList *sl);

// Core operations

size_t size_skiplist(SkipList *sl);
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

#endif

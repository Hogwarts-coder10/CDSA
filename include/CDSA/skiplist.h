#ifndef SKIPLIST_H
#define SKIPLIST_H

#include "CDSA/error.h"
#include <stdbool.h>
#include <stddef.h>

#define SKIPLIST_MAX_LEVEL 16

typedef struct cdsa_skiplist cdsa_skiplist;

// LifeCycle
cdsa_skiplist *cdsa_create_skiplist();
void cdsa_free_skiplist(cdsa_skiplist *sl);

// Core operations

size_t cdsa_size_skiplist(const cdsa_skiplist *sl);
int level_skiplist(cdsa_skiplist *sl);

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
CDSA_STATUS insert_skiplist(cdsa_skiplist *sl, double score, const char *value);

// Returns true if found and deleted, false if it didn't exist
CDSA_STATUS remove_skiplist(cdsa_skiplist *sl, double score, const char *value);

char **get_range_skiplist(cdsa_skiplist *sl, double min_score, double max_score,
                          int *out_score);

// -- Iterartor API --
// Opaque Iterartor type
typedef struct cdsa_skiplist_iterator cdsa_skiplist_iterator;

/**
 * @brief creates a new Iterartor for cdsa_skiplist
 * @warning the caller must free the Iterartor using cdsa_free_skiplist_iterator
 */

cdsa_skiplist_iterator *cdsa_create_skiplist_iterator(const cdsa_skiplist *sl);

/**
 * @brief Checks if there are more nodes to read.
 */
bool cdsa_has_next_skiplist(cdsa_skiplist_iterator *iter);

/**
 * @brief Advances the iterator and retrieves the next score and value.
 * @param out_score Pointer to store the score (optional, can be NULL).
 * @param out_value Pointer to store the value string (optional, can be NULL).
 */
CDSA_STATUS cdsa_next_skiplist(cdsa_skiplist_iterator *iter, double *out_score,
                          const char **out_value);

/**
 * @brief Frees the iterator memory.
 */
void cdsa_free_skiplist_iterator(cdsa_skiplist_iterator *iter);

#endif

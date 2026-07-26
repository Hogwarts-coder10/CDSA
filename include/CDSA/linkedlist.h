#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "CDSA/error.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct cdsa_linkedlist cdsa_linkedlist;

cdsa_linkedlist *cdsa_create_linkedlist(cdsa_size_t elem_size);
void cdsa_free_linkedlist(cdsa_linkedlist *list);
/**
 * @brief Pushes an element onto the front of the linked list.
 * * @ownership
 * - VALUE: The library creates a shallow, byte-for-byte copy using memcpy
 * based on the list's configured elem_size.
 * - MEMORY: If the element being copied is a struct containing pointers to
 * dynamically allocated memory, the caller retains ownership of that underlying
 * memory and must free it before destroying the list.
 */
CDSA_STATUS cdsa_push_front_linkedlist(cdsa_linkedlist *list, void *value);
cdsa_size_t cdsa_size_linkedlist(cdsa_linkedlist *list);
bool cdsa_is_empty_linkedlist(cdsa_linkedlist *list);
void *cdsa_front_linkedlist(cdsa_linkedlist *list);
CDSA_STATUS cdsa_pop_front_linkedlist(cdsa_linkedlist *list);
void cdsa_clear_linkedlist(cdsa_linkedlist *list);
void print_linkedlist(cdsa_linkedlist *list, void (*print_fn)(void *));

// --- Iterator API ---

typedef struct cdsa_linkedlist_iterator cdsa_linkedlist_iterator;

/**
 * @brief Creates a new iterator for the cdsa_linkedlist.
 * @warning The caller must free the iterator using cdsa_free_linkedlist_iterator.
 */
cdsa_linkedlist_iterator *cdsa_create_linkedlist_iterator(cdsa_linkedlist *list);

/**
 * @brief Checks if there are more nodes to read.
 */
bool cdsa_has_next_linkedlist(cdsa_linkedlist_iterator *iter);

/**
 * @brief Advances the iterator and retrieves a pointer to the next element.
 * * @ownership
 * - YIELD: Returns a temporary pointer directly to the data payload of the
 * current node.
 * - WARNING: Do NOT free this pointer. It is invalidated if the list is
 * modified.
 * * @return CDSA_OK on success, or CDSA_ERR_ITER_INVALIDATED if a concurrent
 * mutation occurred.
 */
CDSA_STATUS cdsa_next_linkedlist(cdsa_linkedlist_iterator *iter, void **out_value);

/**
 * @brief Frees the iterator memory.
 */
void cdsa_free_linkedlist_iterator(cdsa_linkedlist_iterator *iter);

#endif

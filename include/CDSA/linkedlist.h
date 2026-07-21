#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "CDSA/error.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct LinkedList LinkedList;

LinkedList *create_linkedlist(size_t elem_size);
void free_linkedlist(LinkedList *list);
/**
 * @brief Pushes an element onto the front of the linked list.
 * * @ownership
 * - VALUE: The library creates a shallow, byte-for-byte copy using memcpy
 * based on the list's configured elem_size.
 * - MEMORY: If the element being copied is a struct containing pointers to
 * dynamically allocated memory, the caller retains ownership of that underlying
 * memory and must free it before destroying the list.
 */
CDSA_STATUS push_front_linkedlist(LinkedList *list, void *value);
size_t size_linkedlist(LinkedList *list);
bool is_empty_linkedlist(LinkedList *list);
void *front_linkedlist(LinkedList *list);
CDSA_STATUS pop_front_linkedlist(LinkedList *list);
void clear_linkedlist(LinkedList *list);
void print_linkedlist(LinkedList *list, void (*print_fn)(void *));

// --- Iterator API ---

typedef struct LinkedListIterator LinkedListIterator;

/**
 * @brief Creates a new iterator for the LinkedList.
 * @warning The caller must free the iterator using free_linkedlist_iterator.
 */
LinkedListIterator *create_linkedlist_iterator(LinkedList *list);

/**
 * @brief Checks if there are more nodes to read.
 */
bool has_next_linkedlist(LinkedListIterator *iter);

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
CDSA_STATUS next_linkedlist(LinkedListIterator *iter, void **out_value);

/**
 * @brief Frees the iterator memory.
 */
void free_linkedlist_iterator(LinkedListIterator *iter);

#endif

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

#endif

#include "CDSA/linkedlist.h"
#include "CDSA/allocator.h"
#include "CDSA/error.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Included the self-referential struct fix from earlier
typedef struct Node {
  void *data;
  struct Node *next;
} Node;

struct LinkedList {
  Node *head;
  size_t size;
  size_t elem_size;
};

LinkedList *create_linkedlist(size_t elem_size) {
  LinkedList *list = CDSA_MALLOC(sizeof(LinkedList));
  if (list == NULL)
    return NULL;
  list->head = NULL;
  list->size = 0;
  list->elem_size = elem_size;
  return list;
}

size_t size_linkedlist(LinkedList *list) {
  if (list == NULL)
    return 0;
  return list->size;
}

bool is_empty_linkedlist(LinkedList *list) {
  if (list == NULL)
    return true;
  return list->size == 0;
}

void free_linkedlist(LinkedList *list) {
  if (list == NULL)
    return;
  Node *current = list->head;
  Node *next;

  while (current != NULL) {
    next = current->next;
    if (current->data != NULL)
      CDSA_FREE(current->data);
    CDSA_FREE(current);
    current = next;
  }
  CDSA_FREE(list);
}

CDSA_STATUS push_front_linkedlist(LinkedList *list, void *value) {
  if (list == NULL || value == NULL)
    return CDSA_ERR_INVALID;

  Node *new_node = CDSA_MALLOC(sizeof(Node));
  if (new_node == NULL)
    return CDSA_ERR_OOM; // Explicit memory failure

  new_node->data = CDSA_MALLOC(list->elem_size);
  if (new_node->data == NULL) {
    CDSA_FREE(new_node); // Clean up the dangling node before returning
    return CDSA_ERR_OOM;
  }

  memcpy(new_node->data, value, list->elem_size);
  new_node->next = list->head;
  list->head = new_node;
  list->size++;

  return CDSA_OK;
}

CDSA_STATUS pop_front_linkedlist(LinkedList *list) {
  if (list == NULL)
    return CDSA_ERR_INVALID;

  if (list->head == NULL)
    return CDSA_ERR_EMPTY; // Prevent popping from an empty list

  Node *old_head = list->head;
  list->head = list->head->next;

  CDSA_FREE(old_head->data);
  CDSA_FREE(old_head);
  list->size--;

  return CDSA_OK;
}

void clear_linkedlist(LinkedList *list) {
  if (list == NULL)
    return;
  Node *current = list->head;
  Node *next;

  while (current != NULL) {
    next = current->next;
    if (current->data != NULL)
      CDSA_FREE(current->data);
    CDSA_FREE(current);
    current = next;
  }
  list->head = NULL;
  list->size = 0;
}

void *front_linkedlist(LinkedList *list) {
  if (list == NULL || list->head == NULL)
    return NULL;
  return list->head->data;
}

void print_linkedlist(LinkedList *list, void (*print_fn)(void *)) {
  if (list == NULL || print_fn == NULL)
    return;
  Node *current = list->head;

  while (current != NULL) {
    print_fn(current->data);
    printf(" -> ");
    current = current->next;
  }
  printf("NULL\n");
}

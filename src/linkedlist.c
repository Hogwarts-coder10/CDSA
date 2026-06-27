#include "CDSA/linkedlist.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

LinkedList *create_linkedlist(size_t elem_size) {
  LinkedList *list = malloc(sizeof(LinkedList));
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
      free(current->data);
    free(current);
    current = next;
  }
  free(list);
}

void push_front_linkedlist(LinkedList *list, void *value) {
  if (list == NULL || value == NULL)
    return;
  Node *new_node = malloc(sizeof(Node));
  if (new_node == NULL)
    return;

  new_node->data = malloc(list->elem_size);
  if (new_node->data == NULL) {
    free(new_node);
    return;
  }

  memcpy(new_node->data, value, list->elem_size);
  new_node->next = list->head;
  list->head = new_node;
  list->size++;
}

void pop_front_linkedlist(LinkedList *list) {
  if (list == NULL || list->head == NULL)
    return;
  Node *old_head = list->head;
  list->head = list->head->next;

  free(old_head->data);
  free(old_head);
  list->size--;
}

void clear_linkedlist(LinkedList *list) {
  if (list == NULL)
    return;
  Node *current = list->head;
  Node *next;

  while (current != NULL) {
    next = current->next;
    if (current->data != NULL)
      free(current->data);
    free(current);
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

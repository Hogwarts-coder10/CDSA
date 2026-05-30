#include "CDSA/linkedlist.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

LinkedList *create_linkedlist(void) {
  LinkedList *list = malloc(sizeof(LinkedList));
  list->head = NULL;
  list->size = 0;

  return list;
}

size_t size_linkedlist(LinkedList *list) { return list->size; }

bool is_empty_linkedlist(LinkedList *list) { return list->size == 0; }

void free_linkedlist(LinkedList *list) {
  Node *current = list->head;
  Node *next;

  while (current != NULL) {
    next = current->next;
    free(current);
    current = next;
  }

  free(list);
}

void push_front_linkedlist(LinkedList *list, int value) {
  Node *new_node = malloc(sizeof(Node));
  new_node->data = value;
  new_node->next = list->head;
  list->head = new_node;
  list->size++;
}

void pop_front_linkedlist(LinkedList *list) {
  if (list->head == NULL) {
    return;
  }

  Node *new_node = list->head;
  list->head = list->head->next;
  free(new_node);
  list->size--;
}

void clear_linkedlist(LinkedList *list) {
  Node *current = list->head;
  Node *next;

  while (current != NULL) {
    next = current->next;
    free(current);
    current = next;
  }

  list->head = NULL;
  list->size = 0;
}

int front_linkedlist(LinkedList *list) { return list->head->data; }

void print_linkedlist(LinkedList *list) {
  Node *current = list->head;

  while (current != NULL) {
    printf("%d -> ", current->data);
    current = current->next;
  }

  printf("NULL\n");
}

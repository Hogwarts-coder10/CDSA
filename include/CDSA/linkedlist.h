#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdbool.h>
#include <stddef.h>

typedef struct Node {
  void *data;
  struct Node *next;
} Node;

typedef struct {
  Node *head;
  size_t size;
  size_t elem_size;
} LinkedList;

LinkedList *create_linkedlist(size_t elem_size);
void free_linkedlist(LinkedList *list);
void push_front_linkedlist(LinkedList *list, void *value);
size_t size_linkedlist(LinkedList *list);
bool is_empty_linkedlist(LinkedList *list);
void *front_linkedlist(LinkedList *list);
void pop_front_linkedlist(LinkedList *list);
void clear_linkedlist(LinkedList *list);
void print_linkedlist(LinkedList *list, void (*print_fn)(void *));

#endif

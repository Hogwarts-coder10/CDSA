#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdbool.h>
#include <stddef.h>

typedef struct Node {
  int data;
  struct Node *next;
} Node;

typedef struct {
  Node *head;
  size_t size;
} LinkedList;

LinkedList *create_linkedlist(void);

void free_linkedlist(LinkedList *list);

void push_front(LinkedList *list, int value);

size_t size_linkedlist(LinkedList *list);

bool is_empty_linkedlist(LinkedList *list);

#endif

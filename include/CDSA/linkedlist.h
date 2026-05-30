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

void push_front_linkedlist(LinkedList *list, int value);

size_t size_linkedlist(LinkedList *list);

bool is_empty_linkedlist(LinkedList *list);

int front_linkedlist(LinkedList *list);

void pop_front_linkedlist(LinkedList *list);

void clear_linkedlist(LinkedList *list);

void print_linkedlist(LinkedList *list);
#endif

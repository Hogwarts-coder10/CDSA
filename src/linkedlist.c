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

void free_linkedlist(LinkedList *list) { free(list); }

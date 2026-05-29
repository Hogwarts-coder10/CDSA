#include "CDSA/linkedlist.h"
#include <stdbool.h>
#include <stdio.h>

int main() {
  LinkedList *list = create_linkedlist();
  printf("%zu\n", size_linkedlist(list));
  printf("%d\n", is_empty_linkedlist(list));
  free_linkedlist(list);

  return 0;
}

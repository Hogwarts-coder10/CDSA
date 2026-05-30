#include "CDSA/linkedlist.h"
#include <stdbool.h>
#include <stdio.h>

int main() {
  LinkedList *list = create_linkedlist();
  printf("%zu\n", size_linkedlist(list));
  printf("%d\n", is_empty_linkedlist(list));
  push_front_linkedlist(list, 10);
  push_front_linkedlist(list, 20);
  push_front_linkedlist(list, 30);
  pop_front_linkedlist(list);
  printf("%d\n", front_linkedlist(list));
  printf("%zu\n", size_linkedlist(list));
  print_linkedlist(list);

  clear_linkedlist(list);

  printf("%zu\n", size_linkedlist(list));
  printf("%d\n", is_empty_linkedlist(list));

  free_linkedlist(list);

  return 0;
}

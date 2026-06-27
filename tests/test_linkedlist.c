#include "CDSA/linkedlist.h"
#include <stdbool.h>
#include <stdio.h>

// Helper to print integers
void print_int(void *data) { printf("%d", *(int *)data); }

int main() {
  // 1. Pass the size of the elements we are storing
  LinkedList *list = create_linkedlist(sizeof(int));

  printf("Size initially: %zu\n", size_linkedlist(list));
  printf("Is empty? %d\n", is_empty_linkedlist(list));

  // 2. Pass variables by reference
  int val1 = 10, val2 = 20, val3 = 30;
  push_front_linkedlist(list, &val1);
  push_front_linkedlist(list, &val2);
  push_front_linkedlist(list, &val3);

  pop_front_linkedlist(list); // Removes 30

  // 3. Cast and dereference the returned void pointer
  int *front_val = (int *)front_linkedlist(list);
  if (front_val) {
    printf("Front element: %d\n", *front_val);
  }

  printf("Size after push/pop: %zu\n", size_linkedlist(list));

  // 4. Pass the print callback function
  print_linkedlist(list, print_int);

  clear_linkedlist(list);

  printf("Size after clear: %zu\n", size_linkedlist(list));
  printf("Is empty? %d\n", is_empty_linkedlist(list));

  free_linkedlist(list);

  return 0;
}

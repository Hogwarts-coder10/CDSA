#include "CDSA/linkedlist.h"
#include <stdbool.h>
#include <stdio.h>

// Helper to print integers
void print_int(void *data) { printf("%d", *(int *)data); }

int main() {
  // 1. Pass the size of the elements we are storing
  cdsa_linkedlist *list = cdsa_create_linkedlist(sizeof(int));

  printf("Size initially: %zu\n", cdsa_size_linkedlist(list));
  printf("Is empty? %d\n", cdsa_is_empty_linkedlist(list));

  // 2. Pass variables by reference
  int val1 = 10, val2 = 20, val3 = 30;
  cdsa_push_front_linkedlist(list, &val1);
  cdsa_push_front_linkedlist(list, &val2);
  cdsa_push_front_linkedlist(list, &val3);

  cdsa_pop_front_linkedlist(list); // Removes 30

  // 3. Cast and dereference the returned void pointer
  int *cdsa_front_val = (int *)cdsa_front_linkedlist(list);
  if (cdsa_front_val) {
    printf("Front element: %d\n", *cdsa_front_val);
  }

  printf("Size after push/pop: %zu\n", cdsa_size_linkedlist(list));

  // 4. Pass the print callback function
  print_linkedlist(list, print_int);

  cdsa_clear_linkedlist(list);

  printf("Size after clear: %zu\n", cdsa_size_linkedlist(list));
  printf("Is empty? %d\n", cdsa_is_empty_linkedlist(list));

  cdsa_free_linkedlist(list);

  return 0;
}

#include "CDSA/vector.h"
#include <stdio.h>

int main() {
  cdsa_vector *vec = cdsa_create_vector(sizeof(int));

  for (int i = 0; i < 10; i++) {
    cdsa_push_vector(vec, &i);
  }

  for (int i = 0; i < 10; i++) {
    printf("%d\n", *(int *)get_vector(vec, i));
  }

  printf("%d\n", *(int *)cdsa_front_vector(vec));
  printf("%d\n", *(int *)cdsa_back_vector(vec));

  printf("Size: %zu\n", cdsa_size_vector(vec));
  printf("Capacity: %zu\n", capacity_vector(vec));
  cdsa_pop_vector(vec);

  int *x = get_vector(vec, 999);

  if (x == NULL) {
    printf("Out of bounds!\n");
  }

  int val = 42;
  set_vector(vec, 2, &val);
  printf("%d\n", *(int *)get_vector(vec, 2));

  printf("Before: %zu\n", cdsa_size_vector(vec));
  cdsa_clear_vector(vec);
  printf("After: %zu\n", cdsa_size_vector(vec));
  printf("Capacity: %zu\n", capacity_vector(vec));

  cdsa_free_vector(vec);
  return 0;
}

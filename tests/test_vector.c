#include "CDSA/vector.h"
#include <stdio.h>

int main() {
  Vector *vec = create_vector(sizeof(int));

  for (int i = 0; i < 10; i++) {
    push_vector(vec, &i);
  }

  for (int i = 0; i < 10; i++) {
    printf("%d\n", *(int *)get_vector(vec, i));
  }

  printf("%d\n", *(int *)front_vector(vec));
  printf("%d\n", *(int *)back_vector(vec));

  printf("Size: %zu\n", size_vector(vec));
  printf("Capacity: %zu\n", capacity_vector(vec));
  pop_vector(vec);

  int *x = get_vector(vec, 999);

  if (x == NULL) {
    printf("Out of bounds!\n");
  }

  int val = 42;
  set_vector(vec, 2, &val);
  printf("%d\n", *(int *)get_vector(vec, 2));

  printf("Before: %zu\n", size_vector(vec));
  clear_vector(vec);
  printf("After: %zu\n", size_vector(vec));
  printf("Capacity: %zu\n", capacity_vector(vec));

  free_vector(vec);
  return 0;
}

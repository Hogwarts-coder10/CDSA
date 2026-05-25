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

  free_vector(vec);

  return 0;
}

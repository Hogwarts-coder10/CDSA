#include "CDSA/stack.h"
#include <stdbool.h>
#include <stdio.h>

int main() {
  Stack *stack = create_stack(sizeof(int));
  int a = 10;
  int b = 20;
  int c = 12;

  push_stack(stack, &a);
  push_stack(stack, &b);
  push_stack(stack, &c);

  printf("Size = %zu\n", size_stack(stack));

  int *top = top_stack(stack);
  printf("Top = %d\n", *top);
  free_stack(stack);

  return 0;
}

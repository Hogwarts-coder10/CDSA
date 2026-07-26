#include "CDSA/stack.h"
#include <stdbool.h>
#include <stdio.h>

int main() {
  cdsa_stack *stack = cdsa_create_stack(sizeof(int));
  int a = 10;
  int b = 20;
  int c = 12;

  cdsa_push_stack(stack, &a);
  cdsa_push_stack(stack, &b);
  cdsa_push_stack(stack, &c);

  printf("Size = %zu\n", cdsa_size_stack(stack));

  int *top = top_stack(stack);
  printf("Top = %d\n", *top);
  cdsa_free_stack(stack);

  return 0;
}

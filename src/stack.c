#include "CDSA/stack.h"
#include "CDSA/vector.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

Stack *create_stack(size_t elem_size) {
  Stack *stack = malloc(sizeof(Stack));
  stack->vec = create_vector(elem_size);
  return stack;
}

void push_stack(Stack *stack, void *value) { push_vector(stack->vec, value); }

void free_stack(Stack *stack) {
  free_vector(stack->vec);
  free(stack);
}

void pop_stack(Stack *stack) { pop_vector(stack->vec); }

void *top_stack(Stack *stack) { return back_vector(stack->vec); }

size_t size_stack(Stack *stack) { return size_vector(stack->vec); }

bool is_empty_stack(Stack *stack) { return is_empty_vector(stack->vec); }

void clear_stack(Stack *stack) { clear_vector(stack->vec); }

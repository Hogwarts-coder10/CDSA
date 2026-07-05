#include "CDSA/stack.h"
#include "CDSA/allocator.h"
#include "CDSA/error.h"
#include "CDSA/vector.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

struct Stack {
  Vector *vec;
};

Stack *create_stack(size_t elem_size) {
  Stack *stack = CDSA_MALLOC(sizeof(Stack));

  if (stack == NULL) {
    return NULL;
  }
  stack->vec = create_vector(elem_size);

  if (stack->vec == NULL) {
    CDSA_FREE(stack);
    return NULL;
  }

  return stack;
}

CDSA_STATUS push_stack(Stack *stack, void *value) {
  if (stack == NULL || value == NULL) {
    return CDSA_ERR_INVALID;
  }
  // Vector handles the OOM checks and returns the proper CDSA_STATUS
  return push_vector(stack->vec, value);
}

void free_stack(Stack *stack) {
  if (stack == NULL)
    return;
  free_vector(stack->vec);
  CDSA_FREE(stack);
}

CDSA_STATUS pop_stack(Stack *stack) {
  if (stack == NULL) {
    return CDSA_ERR_INVALID;
  }
  // Vector handles the empty checks and returns CDSA_ERR_NOT_FOUND or OK
  return pop_vector(stack->vec);
}

void *top_stack(Stack *stack) {
  if (stack == NULL)
    return NULL;
  return back_vector(stack->vec);
}

size_t size_stack(Stack *stack) {
  if (stack == NULL)
    return 0;
  return size_vector(stack->vec);
}

bool is_empty_stack(Stack *stack) {
  if (stack == NULL)
    return true;
  return is_empty_vector(stack->vec);
}

void clear_stack(Stack *stack) {
  if (stack == NULL)
    return;
  clear_vector(stack->vec);
}

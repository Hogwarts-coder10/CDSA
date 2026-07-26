#include "CDSA/stack.h"
#include "CDSA/allocator.h"
#include "CDSA/error.h"
#include "CDSA/vector.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

struct Stack {
  cdsa_vector *vec;
};

Stack *cdsa_create_stack(size_t elem_size) {
  Stack *stack = CDSA_MALLOC(sizeof(Stack));

  if (stack == NULL) {
    return NULL;
  }
  stack->vec = cdsa_create_vector(elem_size);

  if (stack->vec == NULL) {
    CDSA_FREE(stack);
    return NULL;
  }

  return stack;
}

CDSA_STATUS cdsa_push_stack(Stack *stack, void *value) {
  if (stack == NULL || value == NULL) {
    return CDSA_ERR_INVALID;
  }
  // cdsa_vector handles the OOM checks and returns the proper CDSA_STATUS
  return cdsa_push_vector(stack->vec, value);
}

void cdsa_free_stack(Stack *stack) {
  if (stack == NULL)
    return;
  cdsa_free_vector(stack->vec);
  CDSA_FREE(stack);
}

CDSA_STATUS cdsa_pop_stack(Stack *stack) {
  if (stack == NULL) {
    return CDSA_ERR_INVALID;
  }
  // cdsa_vector handles the empty checks and returns CDSA_ERR_NOT_FOUND or OK
  return cdsa_pop_vector(stack->vec);
}

void *top_stack(Stack *stack) {
  if (stack == NULL)
    return NULL;
  return cdsa_back_vector(stack->vec);
}

size_t cdsa_size_stack(Stack *stack) {
  if (stack == NULL)
    return 0;
  return cdsa_size_vector(stack->vec);
}

bool cdsa_is_empty_stack(Stack *stack) {
  if (stack == NULL)
    return true;
  return cdsa_is_empty_vector(stack->vec);
}

void cdsa_clear_stack(Stack *stack) {
  if (stack == NULL)
    return;
  cdsa_clear_vector(stack->vec);
}

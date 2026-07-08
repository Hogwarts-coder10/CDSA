#ifndef STACK_H
#define STACK_H

#include "CDSA/error.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct Stack Stack;

Stack *create_stack(size_t elem_size);
void free_stack(Stack *stack);
/**
 * @brief Pushes an element into the collection.
 *
 * @ownership
 * - VALUE: The library creates a shallow, byte-for-byte copy using memcpy
 *   based on the collection's configured elem_size.
 * - MEMORY: If the element is a pointer to dynamically allocated memory,
 *   the caller retains ownership of that underlying memory and must free it.
 */
CDSA_STATUS push_stack(Stack *stack, void *value);
CDSA_STATUS pop_stack(Stack *stack);
void *top_stack(Stack *stack);
size_t size_stack(Stack *stack);
bool is_empty_stack(Stack *stack);
void clear_stack(Stack *stack);

#endif

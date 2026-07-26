#ifndef STACK_H
#define STACK_H

#include "CDSA/error.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct cdsa_stack cdsa_stack;

cdsa_stack *cdsa_create_stack(size_t elem_size);
void cdsa_free_stack(cdsa_stack *stack);
/**
 * @brief Pushes an element into the collection.
 *
 * @ownership
 * - VALUE: The library creates a shallow, byte-for-byte copy using memcpy
 *   based on the collection's configured elem_size.
 * - MEMORY: If the element is a pointer to dynamically allocated memory,
 *   the caller retains ownership of that underlying memory and must free it.
 */
CDSA_STATUS cdsa_push_stack(cdsa_stack *stack, void *value);
CDSA_STATUS cdsa_pop_stack(cdsa_stack *stack);
void *top_stack(cdsa_stack *stack);
size_t cdsa_size_stack(const cdsa_stack *stack);
bool cdsa_is_empty_stack(const cdsa_stack *stack);
void cdsa_clear_stack(cdsa_stack *stack);

#endif

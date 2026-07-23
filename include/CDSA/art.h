#ifndef ART_H
#define ART_H

#include "CDSA/error.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct NodeHeader NodeHeader;

// Adaptive Nodes
typedef struct Node4 Node4;
typedef struct Node16 Node16;
typedef struct Node48 Node48;
typedef struct Node256 Node256;

// LEAF_NODE (ART = Adaptive Radix Tree (Trie))

typedef struct ArtLeaf ArtLeaf;
typedef struct ArtTree ArtTree;

// LifeCycle

ArtTree *create_art();
void free_art(ArtTree *tree);

// Core Operations

/**
 * @brief Inserts a key-value pair into the Adaptive Radix Tree.
 * * @ownership
 * - KEY: The library takes ownership by creating an internal deep copy
 * (strdup). The caller can safely free their original key string immediately
 * after insertion.
 * - VALUE: The caller retains ownership. The tree only stores the raw void*
 * pointer.
 * - MEMORY: The caller is responsible for freeing the memory pointed to by
 * 'value' before destroying the tree or removing the key, to prevent memory
 * leaks.
 */
CDSA_STATUS insert_art(ArtTree *tree, const char *key, void *value);

/**
 * @brief Searches for a value by its key.
 * * @ownership
 * - RETURN: Returns the raw pointer to the value. The caller retains ownership
 * of this memory and should not free it unless they are actively removing it
 * from the tree.
 */
void *search_art(ArtTree *tree, const char *key);

void print_art(ArtTree *tree);

/**
 * @brief Removes a key-value pair from the tree.
 * * @warning Because the tree does not own the value pointers, calling this
 * function will permanently lose the reference to the value. The caller MUST
 * retrieve and free the value (using search_art) before calling delete_art if
 * the value was dynamically allocated.
 */
CDSA_STATUS delete_art(ArtTree *tree, const char *key);
size_t size_art(ArtTree *tree);

// --- Iterator API ---

typedef struct ArtIterator ArtIterator;

/**
 * @brief Creates a new iterator for the Adaptive Radix Tree.
 * @warning The caller must free the iterator using free_art_iterator.
 */
ArtIterator *create_art_iterator(ArtTree *tree);

/**
 * @brief Checks if there are more key-value pairs to read.
 */
bool has_next_art(ArtIterator *iter);

/**
 * @brief Advances the iterator and retrieves a pointer to the next value.
 * * @ownership
 * - YIELD: Returns a temporary pointer to the value inside the leaf.
 * - WARNING: Do NOT free this pointer. It is invalidated if the tree is
 * modified.
 * * @return CDSA_OK on success, or CDSA_ERR_ITER_INVALIDATED if a concurrent
 * mutation occurred.
 */
CDSA_STATUS next_art(ArtIterator *iter, void **out_value);

/**
 * @brief Frees the iterator memory.
 */
void free_art_iterator(ArtIterator *iter);
#endif

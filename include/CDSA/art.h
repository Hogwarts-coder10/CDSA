#ifndef ART_H
#define ART_H

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
bool insert_art(ArtTree *tree, const char *key, void *value);
void *search_art(ArtTree *tree, const char *key);
void print_art(ArtTree *tree);
bool delete_art(ArtTree *tree, const char *key);
#endif

#ifndef ART_H
#define ART_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum { NODE4, NODE16, LEAF_NODE } NodeType;

typedef struct {
  NodeType type;
  uint32_t prefix_len;
  uint8_t prefix[10];
} NodeHeader;

// Adaptive Nodes

typedef struct {
  NodeHeader header;
  uint8_t num_children;
  uint8_t keys[4];
  void *children[4];
} Node4;

typedef struct {
  NodeHeader header;
  uint8_t num_children;
  uint8_t keys[16];
  void *children[16];
} Node16;

// LEAF_NODE (ART = Adaptive Radix Tree (Trie))

typedef struct {
  NodeHeader header;
  char *key;
  void *value;
} ArtLeaf;

typedef struct {
  void *root;
  size_t size;
} ArtTree;

// LifeCycle

ArtTree *create_art();
void free_art(ArtTree *tree);

// Core Operations
bool insert_art(ArtTree *tree, const char *key, void *value);
void *search_art(ArtTree *tree, const char *key);

#endif

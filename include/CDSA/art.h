#ifndef ART_H
#define ART_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum { NODE4, NODE16, NODE48, NODE256, LEAF_NODE } NodeType;

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

typedef struct {
  NodeHeader header;

  /* The 256-byte Lookup Map.
   Every index corresponds to an ASCII character.
   Valued at 255 if empty, or 0-47 pointing to the slot in the children array.
  */

  uint8_t child_index[256];

  // Compressed array of actual pointers (saving upto 1.6KB per Node)
  void *children[48];

  uint8_t num_children;
} Node48;

typedef struct {
  NodeHeader header;

  void *children[256];

  /* if we use uint8_t, after 255 overflow occurs and the MSB is ignored */
  uint16_t num_children;
} Node256;

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

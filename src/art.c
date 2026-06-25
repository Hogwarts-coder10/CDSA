#include "CDSA/art.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Node Allocators

static Node4 *alloc_node4() {
  Node4 *node = calloc(1, sizeof(Node4));
  node->header.type = NODE4;
  node->num_children = 0;
  return node;
}

static Node16 *alloc_node16() {
  Node16 *node = calloc(1, sizeof(Node16));
  node->header.type = NODE16;
  node->num_children = 0;
  return node;
}

static Node48 *alloc_node48() {
  Node48 *node = calloc(1, sizeof(Node48));
  node->header.type = NODE48;

  // Setting all indices to '255'(empty)
  memset(node->child_index, 255, sizeof(node->child_index));

  node->num_children = 0;
  return node;
}

static Node256 *alloc_node256() {
  Node256 *node = calloc(1, sizeof(Node256));
  node->header.type = NODE256;
  node->num_children = 0;
  return node;
}

static char *safe_strdup(const char *s) {
  size_t len = strlen(s) + 1;
  char *dup = malloc(len);
  if (dup)
    memcpy(dup, s, len);
  return dup;
}

static ArtLeaf *alloc_leaf(const char *key, void *value) {
  ArtLeaf *leaf = calloc(1, sizeof(ArtLeaf));
  leaf->header.type = LEAF_NODE;
  leaf->key = safe_strdup(key);
  leaf->value = value;
  return leaf;
}

// The Adaptive Morphing Engine

static Node16 *upgrade_node4_to_node16(Node4 *old_node) {
  Node16 *new_node = alloc_node16();

  // 1. Copy the header (preserves the Path Compression prefixes!)
  new_node->header = old_node->header;
  new_node->header.type = NODE16; // Make sure we update the tag!

  // 2. Copy the children and their routing keys
  new_node->num_children = old_node->num_children;
  for (int i = 0; i < old_node->num_children; i++) {
    new_node->keys[i] = old_node->keys[i];
    new_node->children[i] = old_node->children[i];
  }

  // 3. Free the old, small node to prevent memory leaks
  free(old_node);

  return new_node;
}

static Node48 *upgrade_node16_to_node48(Node16 *old_node) {
  Node48 *new_node = alloc_node48();

  // Copy the header
  new_node->header = old_node->header;
  new_node->header.type = NODE48;

  // Copy the children and their routing keys
  new_node->num_children = old_node->num_children;
  for (int i = 0; i < old_node->num_children; i++) {
    uint8_t key_char = old_node->keys[i];

    // Put the pointer in the condensed array
    new_node->children[i] = old_node->children[i];

    // Tell the 256-byte map where to find it!
    new_node->child_index[key_char] = i;
  }

  free(old_node);
  return new_node;
}

static Node256 *upgrade_node48_to_node256(Node48 *old_node) {
  Node256 *new_node = alloc_node256();

  // Copy the header
  new_node->header = old_node->header;
  new_node->header.type = NODE256;
  new_node->num_children = old_node->num_children;

  // Unpack the hashmap to an array
  for (int i = 0; i < 256; i++) {
    uint8_t new_index = old_node->child_index[i];

    if (new_index != 255) { // if the hashmap is not empty
      new_node->children[i] = old_node->children[new_index];
    }
  }

  free(old_node);

  return new_node;
}

// Internal Helper: Finding a child pointer in Node4
static void **find_child_node4(Node4 *n, uint8_t c) {
  for (int i = 0; i < n->num_children; i++) {
    if (n->keys[i] == c) {
      return &n->children[i];
    }
  }

  return NULL;
}

// Internal Helper: Finding a child pointer in Node64
static void **find_child_node16(Node16 *n, uint8_t c) {
  for (int i = 0; i < n->num_children; i++) {
    if (n->keys[i] == c) {
      return &n->children[i];
    }
  }

  return NULL;
}

static void **find_child_node48(Node48 *n, uint8_t c) {
  uint8_t index = n->child_index[c];

  if (index == 255)
    return NULL;

  return &n->children[index];
}

static void **find_child_node256(Node256 *n, uint8_t c) {
  // direct memory access
  return &n->children[c];
}

static void free_node(void *node) {
  if (node == NULL)
    return;

  NodeHeader *header = (NodeHeader *)node;

  if (header->type == LEAF_NODE) {
    ArtLeaf *leaf = (ArtLeaf *)node;
    free(leaf->key); // we allocated this with strdup() during insertion
    free(leaf);
  }

  else if (header->type == NODE4) {
    Node4 *n = (Node4 *)node;

    for (int i = 0; i < n->num_children; i++) {
      free_node(n->children[i]);
    }

    free(n);
  }

  else if (header->type == NODE16) {
    Node16 *n = (Node16 *)node;

    for (int i = 0; i < n->num_children; i++) {
      free_node(n->children[i]);
    }

    free(n);
  }

  else if (header->type == NODE48) {
    Node48 *n = (Node48 *)node;

    for (int i = 0; i < n->num_children; i++) {
      free_node(n->children[i]);
    }

    free(n);
  }

  else if (header->type == NODE256) {
    Node256 *n = (Node256 *)node;

    for (int i = 0; i < 256; i++) {
      if (n->children[i] != NULL) {
        free_node(n->children[i]);
      }
    }

    free(n);
  }
}

// LifeCycle

ArtTree *create_art() {
  ArtTree *tree = malloc(sizeof(ArtTree));
  tree->root = NULL;
  tree->size = 0;
  return tree;
}

// Main Engine
bool insert_art(ArtTree *tree, const char *key, void *value) {
  // 1. If the tree is empty, just create a leaf and we are done!
  if (tree->root == NULL) {
    tree->root = alloc_leaf(key, value);
    tree->size++;
    return true;
  }

  void **current_ptr = &tree->root;
  uint8_t *key_bytes = (uint8_t *)key;
  int depth = 0;

  // 2. Traverse the tree
  while (*current_ptr != NULL) {
    NodeHeader *header = (NodeHeader *)*current_ptr;

    // If we hit a Leaf, we reached the end of a path.
    // (In a full production ART, we would check for key collisions here and
    // split the leaf. For Milestone 1, we will just assume keys don't perfectly
    // overlap yet to test the upgrades).
    if (header->type == LEAF_NODE) {
      ArtLeaf *leaf = (ArtLeaf *)*current_ptr;

      // 1. Find exactly where the existing string and the new string diverge
      int i = 0;
      while (leaf->key[i] != '\0' && key[i] != '\0' && leaf->key[i] == key[i]) {
        i++;
      }

      // 2. Are they exactly the same string
      if (leaf->key[i] == '\0' && key[i] == '\0') {
        // key exists so update the value
        leaf->value = value;
        return true;
      }

      // Since we found divergence, we need to make a new Node4 (intersection)
      Node4 *new_node4 = alloc_node4();

      // (Path Compression: In a full engine, we'd save the matched letters
      // here!)

      // Add old keys to the new Node4
      new_node4->keys[0] = (uint8_t)leaf->key[i];
      new_node4->children[0] = leaf;
      new_node4->num_children++;

      // 5. Add the NEW leaf to the new Node4
      ArtLeaf *new_leaf = alloc_leaf(key, value);
      new_node4->keys[1] = (uint8_t)key[i];
      new_node4->children[1] = new_leaf;
      new_node4->num_children++;

      // 6. The Double-Pointer Magic: Overwrite the parent's pointer!
      // Instead of pointing to the old leaf, the parent now points to our new
      // intersection.
      *current_ptr = new_node4;

      tree->size++;
      return true;
    }

    uint8_t c = key_bytes[depth];
    void **next_ptr = NULL;

    // 3. Route based on the Node Type
    if (header->type == NODE4) {
      Node4 *n = (Node4 *)*current_ptr;
      next_ptr = find_child_node4(n, c);

      // If the child doesn't exist, we need to add it!
      if (next_ptr == NULL) {
        // Do we have room in this Node4?
        if (n->num_children < 4) {
          n->keys[n->num_children] = c;
          n->children[n->num_children] = alloc_leaf(key, value);
          n->num_children++;
          tree->size++;
          return true;
        } else {
          // THE MAGIC HAPPENS HERE: Node4 is full!
          // Upgrade to Node16, and overwrite the parent's pointer to point to
          // the new node!
          Node16 *new_node = upgrade_node4_to_node16(n);
          *current_ptr = new_node;

          // Now insert into our brand new Node16
          new_node->keys[new_node->num_children] = c;
          new_node->children[new_node->num_children] = alloc_leaf(key, value);
          new_node->num_children++;
          tree->size++;
          return true;
        }
      }
    }

    else if (header->type == NODE16) {
      Node16 *n = (Node16 *)*current_ptr;
      next_ptr = find_child_node16(n, c);

      if (next_ptr == NULL) {
        if (n->num_children < 16) {
          n->keys[n->num_children] = c;
          n->children[n->num_children] = alloc_leaf(key, value);
          n->num_children++;
          tree->size++;
          return true;
        }
        // (Node16 -> Node48 upgrade logic will go here in Milestone 2!)
        else {
          Node48 *new_node = upgrade_node16_to_node48(n);
          *current_ptr = new_node;

          // insert our Node48
          uint8_t new_index = new_node->num_children;
          new_node->children[new_index] = alloc_leaf(key, value);
          new_node->child_index[c] = new_index;
          new_node->num_children++;

          tree->size++;
          return true;
        }
      }
    }

    else if (header->type == NODE48) {
      Node48 *n = (Node48 *)*current_ptr;
      next_ptr = find_child_node48(n, c);

      if (next_ptr == NULL) {
        if (n->num_children < 48) {
          // THE HASHMAP MAGIC:
          // 1. Get the next available slot in the compressed array
          uint8_t new_index = n->num_children;

          n->children[new_index] = alloc_leaf(key, value);

          n->child_index[c] = new_index;

          n->num_children++;
          tree->size++;
          return true;
        }

        else {
          Node256 *new_node = upgrade_node48_to_node256(n);
          *current_ptr = new_node;

          // Directly assign the 49th child using the ASCII byte as the index
          new_node->children[c] = alloc_leaf(key, value);
          new_node->num_children++;

          tree->size++;
          return true;
        }
      }

    }

    else if (header->type == NODE256) {
      Node256 *n = (Node256 *)*current_ptr;
      next_ptr = find_child_node256(n, c);

      if (*next_ptr == NULL) {
        *next_ptr = alloc_leaf(key, value);
        n->num_children++;
        tree->size++;
        return true;
      }
    }

    // Move down to the next level
    current_ptr = next_ptr;
    depth++;
  }

  return false;
}

void *search_art(ArtTree *tree, const char *key) {
  if (tree == NULL || tree->root == NULL)
    return NULL;

  void *current = tree->root;
  uint8_t *key_bytes = (uint8_t *)key;
  int depth = 0;

  // Walk down the tree until we hit a dead end or find the leaf
  while (current != NULL) {
    NodeHeader *header = (NodeHeader *)current;

    // 1. Did we reach the end of the path?
    if (header->type == LEAF_NODE) {
      ArtLeaf *leaf = (ArtLeaf *)current;
      // Verify it's the exact string (handles edge cases where paths look
      // similar)
      if (strcmp(leaf->key, key) == 0) {
        return leaf->value;
      }
      return NULL; // String didn't perfectly match
    }

    // 2. What character are we looking for at this depth?
    uint8_t c = key_bytes[depth];
    void *next = NULL;

    // 3. Scan the arrays based on the node type
    if (header->type == NODE4) {
      Node4 *n = (Node4 *)current;
      for (int i = 0; i < n->num_children; i++) {
        if (n->keys[i] == c) {
          next = n->children[i];
          break;
        }
      }
    }

    else if (header->type == NODE16) {
      Node16 *n = (Node16 *)current;
      for (int i = 0; i < n->num_children; i++) {
        if (n->keys[i] == c) {
          next = n->children[i];
          break;
        }
      }
    }

    else if (header->type == NODE48) {
      Node48 *n = (Node48 *)current;

      // O(1) direct look up in the 256 byte hashmap
      uint8_t index = n->child_index[c];

      if (index != 255) {
        next = n->children[index];
      }

      else {
        next = NULL;
      }
    }

    else if (header->type == NODE256) {
      Node256 *n = (Node256 *)current;
      next = n->children[c];
    }

    // 4. If we didn't find the character in the arrays, the key doesn't exist
    if (next == NULL)
      return NULL;

    // Move deeper into the tree
    current = next;
    depth++;
  }

  return NULL;
}

void free_art(ArtTree *tree) {
  if (tree == NULL)
    return;
  free_node(tree->root);
  free(tree);
}

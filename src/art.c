#include "CDSA/art.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Node Allocators ---

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

// --- Internal Helper: Check Prefix match ---
// Returns the number of characters that successfully matched the node's prefix
static int check_prefix(NodeHeader *header, const char *key, int depth) {
  // only checks for first 10 characters (or prefix Length)
  int max_cmp = (header->prefix_len < 10) ? header->prefix_len : 10;
  int idx;

  for (idx = 0; idx < max_cmp; idx++) {
    if (header->prefix[idx] != (uint8_t)key[depth + idx]) {
      return idx;
    }
  }

  return idx;
}

// --- The Adaptive Morphing Engine ---

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

// --- Find Child Helpers ---

static void **find_child_node4(Node4 *n, uint8_t c) {
  for (int i = 0; i < n->num_children; i++) {
    if (n->keys[i] == c) {
      return &n->children[i];
    }
  }
  return NULL;
}

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
  return &n->children[c];
}

// --- Teardown Helper ---

static void free_node(void *node) {
  if (node == NULL)
    return;

  NodeHeader *header = (NodeHeader *)node;

  if (header->type == LEAF_NODE) {
    ArtLeaf *leaf = (ArtLeaf *)node;
    free(leaf->key);
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

// --- Advanced ASCII Tree Printer ---
static void print_node_visual(void *node, char edge_char, bool is_last[],
                              int level) {
  if (node == NULL)
    return;

  // 1. Draw the vertical continuation lines for previous levels
  for (int i = 0; i < level - 1; i++) {
    if (is_last[i])
      printf("    ");
    else
      printf("│   ");
  }

  // 2. Draw the connector for the current node and its routing character
  if (level > 0) {
    if (is_last[level - 1])
      printf("└── ");
    else
      printf("├── ");
    printf("('%c') ", edge_char);
  }

  NodeHeader *header = (NodeHeader *)node;

  // 3. Base Case: Leaf
  if (header->type == LEAF_NODE) {
    ArtLeaf *leaf = (ArtLeaf *)node;
    printf("🍃 Leaf: \"%s\" -> %s\n", leaf->key, (char *)leaf->value);
    return;
  }

  // 4. Print the Node Type
  if (header->type == NODE4)
    printf("🔀 Node4");
  else if (header->type == NODE16)
    printf("🔀 Node16");
  else if (header->type == NODE48)
    printf("🔀 Node48");
  else if (header->type == NODE256)
    printf("🔀 Node256");

  // 5. Print the Compressed Path (Prefix) if it exists
  if (header->prefix_len > 0) {
    int p_len = (header->prefix_len < 10) ? header->prefix_len : 10;
    printf(" [Prefix: \"");
    for (int i = 0; i < p_len; i++)
      printf("%c", header->prefix[i]);
    if (header->prefix_len > 10)
      printf("...");
    printf("\"]");
  }
  printf("\n");

  // 6. Route to children (Tracking the last child for visual formatting)
  int child_count = 0;
  int total_children = 0;

  // Get total children so we know when to draw the '└──' elbow
  if (header->type == NODE4)
    total_children = ((Node4 *)node)->num_children;
  else if (header->type == NODE16)
    total_children = ((Node16 *)node)->num_children;
  else if (header->type == NODE48)
    total_children = ((Node48 *)node)->num_children;
  else if (header->type == NODE256)
    total_children = ((Node256 *)node)->num_children;

  if (header->type == NODE4) {
    Node4 *n = (Node4 *)node;
    for (int i = 0; i < n->num_children; i++) {
      is_last[level] = (child_count == total_children - 1);
      print_node_visual(n->children[i], n->keys[i], is_last, level + 1);
      child_count++;
    }
  } else if (header->type == NODE16) {
    Node16 *n = (Node16 *)node;
    for (int i = 0; i < n->num_children; i++) {
      is_last[level] = (child_count == total_children - 1);
      print_node_visual(n->children[i], n->keys[i], is_last, level + 1);
      child_count++;
    }
  } else if (header->type == NODE48) {
    Node48 *n = (Node48 *)node;
    for (int i = 0; i < 256; i++) {
      uint8_t idx = n->child_index[i];
      if (idx != 255) {
        is_last[level] = (child_count == total_children - 1);
        print_node_visual(n->children[idx], (char)i, is_last, level + 1);
        child_count++;
      }
    }
  } else if (header->type == NODE256) {
    Node256 *n = (Node256 *)node;
    for (int i = 0; i < 256; i++) {
      if (n->children[i] != NULL) {
        is_last[level] = (child_count == total_children - 1);
        print_node_visual(n->children[i], (char)i, is_last, level + 1);
        child_count++;
      }
    }
  }
}

// --- LifeCycle ---

ArtTree *create_art() {
  ArtTree *tree = malloc(sizeof(ArtTree));
  tree->root = NULL;
  tree->size = 0;
  return tree;
}

void free_art(ArtTree *tree) {
  if (tree == NULL)
    return;
  free_node(tree->root);
  free(tree);
}

void print_art(ArtTree *tree) {
  printf("\n=== 🌲 ART TREE DUMP (Total Keys: %zu) ===\n", tree->size);
  if (tree == NULL || tree->root == NULL) {
    printf("Tree is empty.\n");
  } else {
    // Array to track if a node at depth 'i' is the last child
    bool is_last[256] = {false};
    print_node_visual(tree->root, '\0', is_last, 0);
  }
  printf("==========================================\n\n");
}

// --- Main Engine: Insert ---
bool insert_art(ArtTree *tree, const char *key, void *value) {
  if (tree->root == NULL) {
    tree->root = alloc_leaf(key, value);
    tree->size++;
    return true;
  }

  void **current_ptr = &tree->root;
  uint8_t *key_bytes = (uint8_t *)key;
  int depth = 0;

  while (*current_ptr != NULL) {
    NodeHeader *header = (NodeHeader *)*current_ptr;

    if (header->type == LEAF_NODE) {
      ArtLeaf *leaf = (ArtLeaf *)*current_ptr;

      int i = 0;
      while (leaf->key[i] != '\0' && key[i] != '\0' && leaf->key[i] == key[i]) {
        i++;
      }

      if (leaf->key[i] == '\0' && key[i] == '\0') {
        leaf->value = value;
        return true;
      }

      Node4 *new_node4 = alloc_node4();

      // --- PATH COMPRESSION: THE CREATOR ---
      // Save the shared characters into the new node's prefix array
      int shared_len = i - depth;
      new_node4->header.prefix_len = shared_len;
      int prefix_save = (shared_len < 10) ? shared_len : 10;
      for (int p = 0; p < prefix_save; p++) {
        new_node4->header.prefix[p] = (uint8_t)key[depth + p];
      }

      new_node4->keys[0] = (uint8_t)leaf->key[i];
      new_node4->children[0] = leaf;
      new_node4->num_children++;

      ArtLeaf *new_leaf = alloc_leaf(key, value);
      new_node4->keys[1] = (uint8_t)key[i];
      new_node4->children[1] = new_leaf;
      new_node4->num_children++;

      *current_ptr = new_node4;
      tree->size++;
      return true;
    }

    // --- PATH COMPRESSION: THE SPLITTER ---
    if (header->prefix_len > 0) {
      int match_len = check_prefix(header, key, depth);
      int expected_match = (header->prefix_len < 10) ? header->prefix_len : 10;

      // Divergence! The strings split before the prefix finished.
      if (match_len < expected_match) {
        Node4 *new_node = alloc_node4();

        new_node->header.prefix_len = match_len;
        memcpy(new_node->header.prefix, header->prefix, match_len);

        uint8_t old_char = header->prefix[match_len];

        header->prefix_len -= (match_len + 1);
        int new_len = (header->prefix_len < 10) ? header->prefix_len : 10;
        for (int p = 0; p < new_len; p++) {
          header->prefix[p] = header->prefix[match_len + 1 + p];
        }

        new_node->keys[0] = old_char;
        new_node->children[0] = *current_ptr;
        new_node->num_children++;

        uint8_t new_char = (uint8_t)key[depth + match_len];
        new_node->keys[1] = new_char;
        new_node->children[1] = alloc_leaf(key, value);
        new_node->num_children++;

        *current_ptr = new_node;
        tree->size++;
        return true;
      }

      // Fast-forward depth if perfectly matched
      depth += header->prefix_len;
    }

    uint8_t c = key_bytes[depth];
    void **next_ptr = NULL;

    if (header->type == NODE4) {
      Node4 *n = (Node4 *)*current_ptr;
      next_ptr = find_child_node4(n, c);

      if (next_ptr == NULL) {
        if (n->num_children < 4) {
          n->keys[n->num_children] = c;
          n->children[n->num_children] = alloc_leaf(key, value);
          n->num_children++;
          tree->size++;
          return true;
        } else {
          Node16 *new_node = upgrade_node4_to_node16(n);
          *current_ptr = new_node;

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
        } else {
          Node48 *new_node = upgrade_node16_to_node48(n);
          *current_ptr = new_node;

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
          uint8_t new_index = n->num_children;
          n->children[new_index] = alloc_leaf(key, value);
          n->child_index[c] = new_index;
          n->num_children++;
          tree->size++;
          return true;
        } else {
          Node256 *new_node = upgrade_node48_to_node256(n);
          *current_ptr = new_node;

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

    current_ptr = next_ptr;
    depth++;
  }

  return false;
}

// --- Main Engine: Search ---
void *search_art(ArtTree *tree, const char *key) {
  if (tree == NULL || tree->root == NULL)
    return NULL;

  void *current = tree->root;
  uint8_t *key_bytes = (uint8_t *)key;
  int depth = 0;

  while (current != NULL) {
    NodeHeader *header = (NodeHeader *)current;

    if (header->type == LEAF_NODE) {
      ArtLeaf *leaf = (ArtLeaf *)current;
      if (strcmp(leaf->key, key) == 0) {
        return leaf->value;
      }
      return NULL;
    }

    // --- PATH COMPRESSION: THE READER ---
    if (header->prefix_len > 0) {
      int match_len = check_prefix(header, key, depth);
      int expected_match = (header->prefix_len < 10) ? header->prefix_len : 10;

      if (match_len != expected_match) {
        return NULL; // Mismatch in the compressed path, word isn't here
      }

      depth += header->prefix_len; // Fast-forward depth!
    }

    uint8_t c = key_bytes[depth];
    void *next = NULL;

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
      uint8_t index = n->child_index[c];
      if (index != 255) {
        next = n->children[index];
      }
    }

    else if (header->type == NODE256) {
      Node256 *n = (Node256 *)current;
      next = n->children[c];
    }

    if (next == NULL)
      return NULL;

    current = next;
    depth++;
  }

  return NULL;
}

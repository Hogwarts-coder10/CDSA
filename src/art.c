#include "CDSA/art.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Sentinel Value for empty slots in Node48
#define ART_EMPTY_SLOT 255

typedef enum { NODE4, NODE16, NODE48, NODE256, LEAF_NODE } NodeType;

struct NodeHeader {
  NodeType type;
  uint32_t prefix_len;
  uint8_t prefix[10];
};

struct Node4 {
  NodeHeader header;
  uint8_t num_children;
  uint8_t keys[4];
  void *children[4];
};

struct Node16 {
  NodeHeader header;
  uint8_t num_children;
  uint8_t keys[16];
  void *children[16];
};

struct Node48 {
  NodeHeader header;

  /* The 256-byte Lookup Map.
   Every index corresponds to an ASCII character.
   Valued at 255 if empty, or 0-47 pointing to the slot in the children array.
  */

  uint8_t child_index[256];

  // Compressed array of actual pointers (saving upto 1.6KB per Node)
  void *children[48];

  uint8_t num_children;
};

struct Node256 {
  NodeHeader header;
  void *children[256];
  /* if we use uint8_t, after 255 overflow occurs and the MSB is ignored */
  uint16_t num_children;
};

struct ArtLeaf {
  NodeHeader header;
  char *key;
  void *value;
};

struct ArtTree {
  void *root;
  size_t size;
};

// --- Node Allocators ---

static Node4 *alloc_node4() {
  Node4 *node = calloc(1, sizeof(Node4));
  if (node == NULL) {
    return NULL;
  }
  node->header.type = NODE4;
  node->num_children = 0;
  return node;
}

static Node16 *alloc_node16() {
  Node16 *node = calloc(1, sizeof(Node16));
  if (node == NULL) {
    return NULL;
  }
  node->header.type = NODE16;
  node->num_children = 0;
  return node;
}

static Node48 *alloc_node48() {
  Node48 *node = calloc(1, sizeof(Node48));
  if (node == NULL) {
    return NULL;
  }
  node->header.type = NODE48;

  // Setting all indices to '255'(empty)
  memset(node->child_index, ART_EMPTY_SLOT, sizeof(node->child_index));

  node->num_children = 0;
  return node;
}

static Node256 *alloc_node256() {
  Node256 *node = calloc(1, sizeof(Node256));
  if (node == NULL) {
    return NULL;
  }
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
  if (leaf == NULL) {
    return NULL;
  }

  leaf->header.type = LEAF_NODE;
  leaf->key = safe_strdup(key);
  if (leaf->key == NULL) {
    free(leaf);
    return NULL;
  }
  leaf->value = value;
  return leaf;
}

// --- Internal Helper: Check Prefix match ---
// Returns the number of characters that successfully matched the node's prefix
static int check_prefix(NodeHeader *header, const char *key, int depth) {
  // only checks for first 10 characters (or prefix Length)
  uint32_t max_cmp = (header->prefix_len < 10) ? header->prefix_len : 10;
  uint32_t idx;

  for (idx = 0; idx < max_cmp; idx++) {
    if (header->prefix[idx] != (uint8_t)key[depth + idx]) {
      return (int)idx;
    }
  }

  return (int)idx;
}

// --- The Adaptive Morphing Engine ---

static Node16 *upgrade_node4_to_node16(Node4 *old_node) {
  Node16 *new_node = alloc_node16();
  if (new_node == NULL) {
    return NULL;
  }
  // 1. Copy the header (preserves the Path Compression prefixes!)
  new_node->header = old_node->header;
  new_node->header.type = NODE16;

  // 2. Copy the children and their routing keys
  new_node->num_children = old_node->num_children;
  for (uint16_t i = 0; i < old_node->num_children; i++) {
    new_node->keys[i] = old_node->keys[i];
    new_node->children[i] = old_node->children[i];
  }

  // 3. Free the old, small node
  free(old_node);

  return new_node;
}

static Node48 *upgrade_node16_to_node48(Node16 *old_node) {
  Node48 *new_node = alloc_node48();

  if (new_node == NULL) {
    return NULL;
  }

  new_node->header = old_node->header;
  new_node->header.type = NODE48;

  new_node->num_children = old_node->num_children;
  for (uint16_t i = 0; i < old_node->num_children; i++) {
    uint8_t key_char = old_node->keys[i];
    new_node->children[i] = old_node->children[i];
    new_node->child_index[key_char] = (uint8_t)i;
  }

  free(old_node);
  return new_node;
}

static Node256 *upgrade_node48_to_node256(Node48 *old_node) {
  Node256 *new_node = alloc_node256();

  if (new_node == NULL) {
    return NULL;
  }
  new_node->header = old_node->header;
  new_node->header.type = NODE256;
  new_node->num_children = old_node->num_children;

  for (int i = 0; i < 256; i++) {
    uint8_t new_index = old_node->child_index[i];
    if (new_index != ART_EMPTY_SLOT) {
      new_node->children[i] = old_node->children[new_index];
    }
  }

  free(old_node);
  return new_node;
}

// --- Find Child Helpers ---

static void **find_child_node4(Node4 *n, uint8_t c) {
  for (uint16_t i = 0; i < n->num_children; i++) {
    if (n->keys[i] == c) {
      return &n->children[i];
    }
  }
  return NULL;
}

static void **find_child_node16(Node16 *n, uint8_t c) {
  for (uint16_t i = 0; i < n->num_children; i++) {
    if (n->keys[i] == c) {
      return &n->children[i];
    }
  }
  return NULL;
}

static void **find_child_node48(Node48 *n, uint8_t c) {
  uint8_t index = n->child_index[c];
  if (index == ART_EMPTY_SLOT)
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
    for (uint16_t i = 0; i < n->num_children; i++) {
      free_node(n->children[i]);
    }
    free(n);
  }

  else if (header->type == NODE16) {
    Node16 *n = (Node16 *)node;
    for (uint16_t i = 0; i < n->num_children; i++) {
      free_node(n->children[i]);
    }
    free(n);
  }

  else if (header->type == NODE48) {
    Node48 *n = (Node48 *)node;
    for (uint16_t i = 0; i < n->num_children; i++) {
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

  for (int i = 0; i < level - 1; i++) {
    if (is_last[i])
      printf("    ");
    else
      printf("│   ");
  }

  if (level > 0) {
    if (is_last[level - 1])
      printf("└── ");
    else
      printf("├── ");
    printf("('%c') ", edge_char);
  }

  NodeHeader *header = (NodeHeader *)node;

  if (header->type == LEAF_NODE) {
    ArtLeaf *leaf = (ArtLeaf *)node;
    printf("🍃 Leaf: \"%s\" -> %s\n", leaf->key, (char *)leaf->value);
    return;
  }

  if (header->type == NODE4)
    printf("🔀 Node4");
  else if (header->type == NODE16)
    printf("🔀 Node16");
  else if (header->type == NODE48)
    printf("🔀 Node48");
  else if (header->type == NODE256)
    printf("🔀 Node256");

  if (header->prefix_len > 0) {
    uint32_t p_len = (header->prefix_len < 10) ? header->prefix_len : 10;
    printf(" [Prefix: \"");
    for (uint32_t i = 0; i < p_len; i++)
      printf("%c", header->prefix[i]);
    if (header->prefix_len > 10)
      printf("...");
    printf("\"]");
  }
  printf("\n");

  uint16_t child_count = 0;
  uint16_t total_children = 0;

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
    for (uint16_t i = 0; i < n->num_children; i++) {
      is_last[level] = (child_count == total_children - 1);
      print_node_visual(n->children[i], n->keys[i], is_last, level + 1);
      child_count++;
    }
  } else if (header->type == NODE16) {
    Node16 *n = (Node16 *)node;
    for (uint16_t i = 0; i < n->num_children; i++) {
      is_last[level] = (child_count == total_children - 1);
      print_node_visual(n->children[i], n->keys[i], is_last, level + 1);
      child_count++;
    }
  } else if (header->type == NODE48) {
    Node48 *n = (Node48 *)node;
    for (int i = 0; i < 256; i++) {
      uint8_t idx = n->child_index[i];
      if (idx != ART_EMPTY_SLOT) {
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

// Downgrade Engines (Shrinkage)

static Node48 *downgrade_node256_to_node48(Node256 *old_node) {
  Node48 *new_node = alloc_node48();

  if (new_node == NULL) {
    return NULL;
  }
  new_node->header = old_node->header;
  new_node->header.type = NODE48;
  new_node->num_children = old_node->num_children;

  uint8_t current_idx = 0;
  for (int i = 0; i < 256; i++) {
    if (old_node->children[i] != NULL) {
      new_node->children[current_idx] = old_node->children[i];
      new_node->child_index[i] = current_idx;
      current_idx++;
    }
  }
  free(old_node);
  return new_node;
}

static Node16 *downgrade_node48_to_node16(Node48 *old_node) {
  Node16 *new_node = alloc_node16();

  if (new_node == NULL) {
    return NULL;
  }

  new_node->header = old_node->header;
  new_node->header.type = NODE16;
  new_node->num_children = old_node->num_children;

  uint8_t current_idx = 0;
  for (int i = 0; i < 256; i++) {
    uint8_t old_index = old_node->child_index[i];
    if (old_index != ART_EMPTY_SLOT) {
      new_node->keys[current_idx] = (uint8_t)i;
      new_node->children[current_idx] = old_node->children[old_index];
      current_idx++;
    }
  }
  free(old_node);
  return new_node;
}

static Node4 *downgrade_node16_to_node4(Node16 *old_node) {
  Node4 *new_node = alloc_node4();

  if (new_node == NULL) {
    return NULL;
  }

  new_node->header = old_node->header;
  new_node->header.type = NODE4;
  new_node->num_children = old_node->num_children;

  for (uint16_t i = 0; i < new_node->num_children; i++) {
    new_node->keys[i] = old_node->keys[i];
    new_node->children[i] = old_node->children[i];
  }
  free(old_node);
  return new_node;
}

// Remove child Helpers
static void remove_child_node4(Node4 *n, uint8_t c) {
  uint16_t pos = 0;
  while (pos < n->num_children && n->keys[pos] != c)
    pos++;
  if (pos == n->num_children)
    return;

  for (uint16_t i = pos; i < n->num_children - 1; i++) {
    n->keys[i] = n->keys[i + 1];
    n->children[i] = n->children[i + 1];
  }
  n->num_children--;
}

static void remove_child_node16(Node16 *n, uint8_t c) {
  uint16_t pos = 0;
  while (pos < n->num_children && n->keys[pos] != c)
    pos++;
  if (pos == n->num_children)
    return;

  for (uint16_t i = pos; i < n->num_children - 1; i++) {
    n->keys[i] = n->keys[i + 1];
    n->children[i] = n->children[i + 1];
  }
  n->num_children--;
}

static void remove_child_node48(Node48 *n, uint8_t c) {
  uint8_t pos = n->child_index[c];
  if (pos == ART_EMPTY_SLOT)
    return;
  n->children[pos] = NULL;
  n->child_index[c] = ART_EMPTY_SLOT;
  n->num_children--;
}

static void remove_child_node256(Node256 *n, uint8_t c) {
  if (n->children[c] != NULL) {
    n->children[c] = NULL;
    n->num_children--;
  }
}

// --- 3. Internal Engine: Recursive Deletion & Memory Merging ---

static void *recursive_delete(void *node, const char *key, int depth,
                              bool *deleted, ArtTree *tree) {
  if (node == NULL)
    return NULL;

  NodeHeader *header = (NodeHeader *)node;
  int key_len = strlen(key);

  if (header->type == LEAF_NODE) {
    ArtLeaf *leaf = (ArtLeaf *)node;
    if (strcmp(leaf->key, key) == 0) {
      *deleted = true;
      tree->size--;
      free(leaf->key);
      free(leaf);
      return NULL;
    }
    return node;
  }

  if (header->prefix_len > 0) {
    int match_len = check_prefix(header, key, depth);
    uint32_t expected_match =
        (header->prefix_len < 10) ? header->prefix_len : 10;
    if ((uint32_t)match_len != expected_match)
      return node;
    depth += header->prefix_len;

    if (depth > key_len)
      return NULL;
  }

  uint8_t c = (uint8_t)key[depth];
  void **child_ptr = NULL;

  if (header->type == NODE4)
    child_ptr = find_child_node4((Node4 *)node, c);
  else if (header->type == NODE16)
    child_ptr = find_child_node16((Node16 *)node, c);
  else if (header->type == NODE48)
    child_ptr = find_child_node48((Node48 *)node, c);
  else if (header->type == NODE256)
    child_ptr = find_child_node256((Node256 *)node, c);

  if (child_ptr == NULL || *child_ptr == NULL)
    return node;

  void *new_child = recursive_delete(*child_ptr, key, depth + 1, deleted, tree);

  *child_ptr = new_child;

  if (*deleted && new_child == NULL) {
    if (header->type == NODE4)
      remove_child_node4((Node4 *)node, c);
    else if (header->type == NODE16)
      remove_child_node16((Node16 *)node, c);
    else if (header->type == NODE48)
      remove_child_node48((Node48 *)node, c);
    else if (header->type == NODE256)
      remove_child_node256((Node256 *)node, c);

    if (header->type == NODE256 && ((Node256 *)node)->num_children == 48) {
      void *smaller = downgrade_node256_to_node48((Node256 *)node);
      return smaller ? smaller : node; // OOM: stay as Node256, don't lose data
    }
    if (header->type == NODE48 && ((Node48 *)node)->num_children == 16) {
      void *smaller = downgrade_node48_to_node16((Node48 *)node);
      return smaller ? smaller : node; // OOM: stay as Node48
    }
    if (header->type == NODE16 && ((Node16 *)node)->num_children == 4) {
      void *smaller = downgrade_node16_to_node4((Node16 *)node);
      return smaller ? smaller : node; // OOM: stay as Node16
    }

    if (header->type == NODE4) {
      Node4 *n = (Node4 *)node;
      if (n->num_children == 1) {
        void *surviving_child = n->children[0];
        uint8_t routing_char = n->keys[0];
        NodeHeader *child_header = (NodeHeader *)surviving_child;

        if (child_header->type != LEAF_NODE) {
          uint32_t new_len = header->prefix_len + 1 + child_header->prefix_len;
          uint32_t limit = (new_len < 10) ? new_len : 10;
          uint8_t new_prefix[10];

          uint32_t pos = 0;
          for (uint32_t i = 0; i < header->prefix_len && pos < limit; i++)
            new_prefix[pos++] = header->prefix[i];
          if (pos < limit)
            new_prefix[pos++] = routing_char;
          for (uint32_t i = 0; i < child_header->prefix_len && pos < limit; i++)
            new_prefix[pos++] = child_header->prefix[i];

          child_header->prefix_len = new_len;
          memcpy(child_header->prefix, new_prefix, limit);
        }

        free(n);
        return surviving_child;
      }
    }
  }

  return node;
}

// Internal Helper: Optimistic Leaf Finder
// Finds the first available leaf under a given node to recover unverified
// prefix bytes

static ArtLeaf *find_minimum_leaf(void *node) {
  if (node == NULL) {
    return NULL;
  }

  NodeHeader *header = (NodeHeader *)node;

  while (header->type != LEAF_NODE) {
    if (header->type == NODE4) {
      node = ((Node4 *)node)->children[0];
    }

    else if (header->type == NODE16) {
      node = ((Node16 *)node)->children[0];
    }

    else if (header->type == NODE48) {
      Node48 *n = (Node48 *)node;

      for (int i = 0; i < 256; i++) {
        if (n->child_index[i] != ART_EMPTY_SLOT) {
          node = n->children[n->child_index[i]];
          break;
        }
      }
    }

    else if (header->type == NODE256) {
      Node256 *n = (Node256 *)node;

      for (int i = 0; i < 256; i++) {
        if (n->children[i] != NULL) {
          node = n->children[i];
          break;
        }
      }
    }

    header = (NodeHeader *)node;
  }

  return (ArtLeaf *)node;
}

// --- LifeCycle ---

ArtTree *create_art() {
  ArtTree *tree = malloc(sizeof(ArtTree));
  if (tree == NULL) {
    return NULL;
  }
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

size_t size_art(ArtTree *tree) { return tree->size; }

void print_art(ArtTree *tree) {
  printf("\n=== 🌲 ART TREE DUMP (Total Keys: %zu) ===\n", tree->size);
  if (tree == NULL || tree->root == NULL) {
    printf("Tree is empty.\n");
  } else {
    bool is_last[256] = {false};
    print_node_visual(tree->root, '\0', is_last, 0);
  }
  printf("==========================================\n\n");
}

bool delete_art(ArtTree *tree, const char *key) {
  if (tree == NULL || tree->root == NULL)
    return false;

  bool deleted = false;
  tree->root = recursive_delete(tree->root, key, 0, &deleted, tree);
  return deleted;
}

// --- Main Engine: Insert ---
bool insert_art(ArtTree *tree, const char *key, void *value) {
  if (tree->root == NULL) {
    tree->root = alloc_leaf(key, value);
    if (tree->root == NULL)
      return false;
    tree->size++;
    return true;
  }

  void **current_ptr = &tree->root;
  uint8_t *key_bytes = (uint8_t *)key;
  int depth = 0;
  int key_len = (int)strlen(key);

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
      if (new_node4 == NULL)
        return false;

      uint32_t shared_len = (uint32_t)(i - depth);
      new_node4->header.prefix_len = shared_len;
      uint32_t prefix_save = (shared_len < 10) ? shared_len : 10;
      for (uint32_t p = 0; p < prefix_save; p++) {
        new_node4->header.prefix[p] = (uint8_t)key[depth + p];
      }

      new_node4->keys[0] = (uint8_t)leaf->key[i];
      new_node4->children[0] = leaf;
      new_node4->num_children++;

      ArtLeaf *new_leaf = alloc_leaf(key, value);
      if (new_leaf == NULL) {
        free(new_node4); // don't leak the node4 we just built
        return false;
      }
      new_node4->keys[1] = (uint8_t)key[i];
      new_node4->children[1] = new_leaf;
      new_node4->num_children++;

      *current_ptr = new_node4;
      tree->size++;
      return true;
    }

    if (header->prefix_len > 0) {
      int match_len = check_prefix(header, key, depth);
      uint32_t expected_match =
          (header->prefix_len < 10) ? header->prefix_len : 10;

      // -- Optimistic Fix: Fetch a leaf to to verify the rest of the long
      // prefix
      if ((uint32_t)match_len == expected_match && header->prefix_len > 10) {
        ArtLeaf *leaf = find_minimum_leaf(*current_ptr);

        while ((uint32_t)match_len < header->prefix_len &&
               key[depth + match_len] != '\0' &&
               key[depth + match_len] == leaf->key[depth + match_len]) {
          match_len++;
        }
      }

      // Now we use the TRUE match_len to determine if we split
      if ((uint32_t)match_len < header->prefix_len) {
        // Allocate both new nodes BEFORE mutating header->prefix_len so the
        // tree is never left in a half-modified state on OOM.
        Node4 *new_node = alloc_node4();
        if (new_node == NULL)
          return false;

        ArtLeaf *new_leaf = alloc_leaf(key, value);
        if (new_leaf == NULL) {
          free(new_node);
          return false;
        }

        new_node->header.prefix_len = (uint32_t)match_len;

        uint32_t save_len = (match_len < 10) ? match_len : 10;
        for (uint32_t p = 0; p < save_len; p++) {
          new_node->header.prefix[p] = (uint8_t)key[depth + p];
        }

        // Fetch the diverging chars from the leaf, as they might not be in our
        // 10-byte buffer
        ArtLeaf *leaf = find_minimum_leaf(*current_ptr);
        uint8_t old_char = (uint8_t)leaf->key[depth + match_len];

        header->prefix_len -= ((uint32_t)match_len + 1);
        uint32_t new_len = (header->prefix_len < 10) ? header->prefix_len : 10;

        for (uint32_t p = 0; p < new_len; p++) {
          header->prefix[p] = (uint8_t)leaf->key[depth + match_len + 1 + p];
        }

        new_node->keys[0] = old_char;
        new_node->children[0] = *current_ptr;
        new_node->num_children++;

        uint8_t new_char = (uint8_t)key[match_len + depth];
        new_node->keys[1] = new_char;
        new_node->children[1] = new_leaf;
        new_node->num_children++;

        *current_ptr = new_node;
        tree->size++;
        return true;
      }

      depth += header->prefix_len;
      if (depth > key_len) {
        return false;
      }
    }

    uint8_t c = key_bytes[depth];
    void **next_ptr = NULL;

    if (header->type == NODE4) {
      Node4 *n = (Node4 *)*current_ptr;
      next_ptr = find_child_node4(n, c);

      if (next_ptr == NULL) {
        if (n->num_children < 4) {
          ArtLeaf *new_leaf = alloc_leaf(key, value);
          if (new_leaf == NULL)
            return false;
          n->keys[n->num_children] = c;
          n->children[n->num_children] = new_leaf;
          n->num_children++;
          tree->size++;
          return true;
        } else {
          Node16 *new_node = upgrade_node4_to_node16(n);
          if (new_node == NULL) // old Node4 still intact, tree valid
            return false;
          *current_ptr = new_node;

          ArtLeaf *new_leaf = alloc_leaf(key, value);
          if (new_leaf == NULL)
            return false;
          new_node->keys[new_node->num_children] = c;
          new_node->children[new_node->num_children] = new_leaf;
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
          ArtLeaf *new_leaf = alloc_leaf(key, value);
          if (new_leaf == NULL)
            return false;
          n->keys[n->num_children] = c;
          n->children[n->num_children] = new_leaf;
          n->num_children++;
          tree->size++;
          return true;
        } else {
          Node48 *new_node = upgrade_node16_to_node48(n);
          if (new_node == NULL) // old Node16 still intact
            return false;
          *current_ptr = new_node;

          ArtLeaf *new_leaf = alloc_leaf(key, value);
          if (new_leaf == NULL)
            return false;
          uint8_t new_index = new_node->num_children;
          new_node->children[new_index] = new_leaf;
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
          ArtLeaf *new_leaf = alloc_leaf(key, value);
          if (new_leaf == NULL)
            return false;
          uint8_t new_index = n->num_children;
          n->children[new_index] = new_leaf;
          n->child_index[c] = new_index;
          n->num_children++;
          tree->size++;
          return true;
        } else {
          Node256 *new_node = upgrade_node48_to_node256(n);
          if (new_node == NULL) // old Node48 still intact
            return false;
          *current_ptr = new_node;

          ArtLeaf *new_leaf = alloc_leaf(key, value);
          if (new_leaf == NULL)
            return false;
          new_node->children[c] = new_leaf;
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
        ArtLeaf *new_leaf = alloc_leaf(key, value);
        if (new_leaf == NULL)
          return false;
        *next_ptr = new_leaf;
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
  int key_len = strlen(key);

  while (current != NULL) {
    NodeHeader *header = (NodeHeader *)current;

    if (header->type == LEAF_NODE) {
      ArtLeaf *leaf = (ArtLeaf *)current;
      if (strcmp(leaf->key, key) == 0) {
        return leaf->value;
      }
      return NULL;
    }

    if (header->prefix_len > 0) {
      int match_len = check_prefix(header, key, depth);
      uint32_t expected_match =
          (header->prefix_len < 10) ? header->prefix_len : 10;

      if ((uint32_t)match_len != expected_match) {
        return NULL;
      }
      depth += header->prefix_len;

      if (depth > key_len)
        return NULL;
    }

    uint8_t c = key_bytes[depth];
    void *next = NULL;

    if (header->type == NODE4) {
      Node4 *n = (Node4 *)current;
      for (uint16_t i = 0; i < n->num_children; i++) {
        if (n->keys[i] == c) {
          next = n->children[i];
          break;
        }
      }
    }

    else if (header->type == NODE16) {
      Node16 *n = (Node16 *)current;
      for (uint16_t i = 0; i < n->num_children; i++) {
        if (n->keys[i] == c) {
          next = n->children[i];
          break;
        }
      }
    }

    else if (header->type == NODE48) {
      Node48 *n = (Node48 *)current;
      uint8_t index = n->child_index[c];
      if (index != ART_EMPTY_SLOT) {
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

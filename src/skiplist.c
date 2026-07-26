#include "CDSA/skiplist.h"
#include "CDSA/allocator.h"
#include "CDSA/error.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct SkipNode {
  double score; // the sorting weight
  char *value;  // the actual data
  int level;    // How tall is this node (1 to MAX_LEVEL)
  struct SkipNode *
      *forward; // array of pointers to the next nodes at higher level
} SkipNode;

struct SkipList {
  SkipNode *header; // starting node (dummy)
  int level;        // current highest level in use
  size_t size;      // total number of items
};

// --- Internal Helpers ---

// The "Coin Flip" - 50% chance to grow taller
static int random_level() {
  int level = 1;
  // Keep flipping as long as we get heads (even number) and haven't hit the
  // ceiling
  while ((rand() % 2 == 0) && level < SKIPLIST_MAX_LEVEL) {
    level++;
  }
  return level;
}

// Portable strdup — no POSIX feature-test macro required
static char *safe_strdup(const char *s) {
  size_t len = strlen(s) + 1;
  char *dup = CDSA_MALLOC(len);
  if (dup)
    memcpy(dup, s, len);
  return dup;
}

static SkipNode *cdsa_create_node(int level, double score, const char *value) {
  SkipNode *node = CDSA_MALLOC(sizeof(SkipNode));
  if (node == NULL)
    return NULL;

  node->value = safe_strdup(value); // portable, no implicit-decl risk
  if (node->value == NULL) {
    CDSA_FREE(node);
    return NULL;
  }

  node->forward = CDSA_CALLOC(level, sizeof(SkipNode *));
  if (node->forward == NULL) {
    CDSA_FREE(node->value);
    CDSA_FREE(node);
    return NULL;
  }

  node->score = score;
  node->level = level;
  return node;
}

// --- Lifecycle ---

SkipList *cdsa_create_skiplist() {
  SkipList *sl = CDSA_MALLOC(sizeof(SkipList));

  if (sl == NULL) {
    return NULL;
  }
  sl->level = 1;
  sl->size = 0;

  // The Dummy Header sits at max level but holds no real data
  sl->header = cdsa_create_node(SKIPLIST_MAX_LEVEL, 0.0, "");

  if (sl->header == NULL) {
    CDSA_FREE(sl);
    return NULL;
  }

  return sl;
}

void cdsa_free_skiplist(SkipList *sl) {
  if (sl == NULL)
    return;

  SkipNode *current = sl->header;
  while (current != NULL) {
    SkipNode *next = current->forward[0]; // Level 0 is a standard linked list
    CDSA_FREE(current->value);
    CDSA_FREE(current->forward); // Free the dynamic array of pointers
    CDSA_FREE(current);
    current = next;
  }
  CDSA_FREE(sl);
}

// --- Core Operations ---

size_t cdsa_size_skiplist(SkipList *sl) {
  if (sl == NULL)
    return 0;
  return sl->size;
}

int level_skiplist(SkipList *sl) {
  if (sl == NULL)
    return 0;
  return sl->level;
}

CDSA_STATUS insert_skiplist(SkipList *sl, double score, const char *value) {
  if (sl == NULL || value == NULL) {
    return CDSA_ERR_INVALID;
  }

  SkipNode *current = sl->header;

  // Breadcrumb trail: remembers the last node we saw at each level before
  // dropping down
  SkipNode *update[SKIPLIST_MAX_LEVEL];

  // Search from the top level down to level 0
  for (int i = sl->level - 1; i >= 0; i--) {
    // Keep moving right if the next node's score is smaller
    // (If scores are tied, we sort alphabetically by value, exactly like
    // Redis!)
    while (current->forward[i] != NULL &&
           (current->forward[i]->score < score ||
            (current->forward[i]->score == score &&
             strcmp(current->forward[i]->value, value) < 0))) {
      current = current->forward[i];
    }
    update[i] = current; // Drop a breadcrumb
  }

  // Roll the dice for the new node's height
  int new_level = random_level();

  // If it's the tallest node we've ever seen, update the header's routing.
  // Save old level so we can roll back if cdsa_create_node fails.
  int old_level = sl->level;
  if (new_level > sl->level) {
    for (int i = sl->level; i < new_level; i++) {
      update[i] = sl->header;
    }
    sl->level = new_level;
  }

  // Create the node and splice it in using our breadcrumbs
  SkipNode *new_node = cdsa_create_node(new_level, score, value);
  if (new_node == NULL) {
    sl->level = old_level; // undo the level bump, list stays consistent
    return CDSA_ERR_OOM;
  }

  for (int i = 0; i < new_level; i++) {
    new_node->forward[i] = update[i]->forward[i];
    update[i]->forward[i] = new_node;
  }

  sl->size++;
  return CDSA_OK;
}

CDSA_STATUS remove_skiplist(SkipList *sl, double score, const char *value) {
  if (sl == NULL || value == NULL) {
    return CDSA_ERR_INVALID;
  }

  SkipNode *current = sl->header;
  SkipNode *update[SKIPLIST_MAX_LEVEL];

  // 1. Find the node and drop breadcrumbs
  for (int i = sl->level - 1; i >= 0; i--) {
    while (current->forward[i] != NULL &&
           (current->forward[i]->score < score ||
            (current->forward[i]->score == score &&
             strcmp(current->forward[i]->value, value) < 0))) {
      current = current->forward[i];
    }
    update[i] = current;
  }

  // Move to the actual target node (if it exists, it will be right after our
  // level 0 breadcrumb)
  current = current->forward[0];

  // 2. Verify this is exactly the node we want to delete
  if (current != NULL && current->score == score &&
      strcmp(current->value, value) == 0) {

    // 3. Rewire the pointers across all levels the node existed on
    for (int i = 0; i < sl->level; i++) {
      // If the breadcrumb at this level doesn't point to our target, stop
      // looking up
      if (update[i]->forward[i] != current) {
        break;
      }
      // Bypass the node!
      update[i]->forward[i] = current->forward[i];
    }

    // 4. If we just deleted the tallest node in the list, lower the list's max
    // height
    while (sl->level > 1 && sl->header->forward[sl->level - 1] == NULL) {
      sl->level--;
    }

    // 5. Safely return the memory to the OS
    CDSA_FREE(current->value);
    CDSA_FREE(current->forward);
    CDSA_FREE(current);

    sl->size--;
    return CDSA_OK; // Successfully deleted
  }

  return CDSA_ERR_NOT_FOUND; // Node didn't exist
}

char **get_range_skiplist(SkipList *sl, double min_score, double max_score,
                          int *out_count) {
  if (sl == NULL || out_count == NULL) {
    return NULL;
  }

  SkipNode *current = sl->header;

  // 1. Fast-forward to the exact starting point using the express lanes
  for (int i = sl->level - 1; i >= 0; i--) {
    while (current->forward[i] != NULL &&
           current->forward[i]->score < min_score) {
      current = current->forward[i];
    }
  }

  // Drop to Level 0 (the actual start of our target range)
  current = current->forward[0];

  // 2. Count how many nodes match the range so we can allocate memory
  int count = 0;
  SkipNode *temp = current;
  while (temp != NULL && temp->score <= max_score) {
    count++;
    temp = temp->forward[0];
  }

  *out_count = count;
  if (count == 0)
    return NULL; // Nobody in this range!

  // 3. Allocate an array of string pointers
  char **results = CDSA_MALLOC(count * sizeof(char *));
  if (results == NULL) {
    *out_count = 0;
    return NULL;
  }

  // 4. Populate the array by walking the Level 0 linked list
  int idx = 0;
  while (current != NULL && current->score <= max_score) {
    // safe_strdup ensures the caller owns these strings!
    results[idx] = safe_strdup(current->value);

    if (results[idx] == NULL) {
      // Free every string we already duplicated, then the array itself
      for (int j = 0; j < idx; j++)
        CDSA_FREE(results[j]);
      CDSA_FREE(results);
      *out_count = 0;
      return NULL;
    }

    idx++;
    current = current->forward[0];
  }

  return results;
}

struct SkipListIterator {
  SkipList *sl;
  SkipNode *current;
};

SkipListIterator *cdsa_create_skiplist_iterator(SkipList *sl) {
  if (sl == NULL)
    return NULL;

  SkipListIterator *iter = CDSA_MALLOC(sizeof(SkipListIterator));
  if (iter == NULL)
    return NULL;

  iter->sl = sl;
  // Level 0 of the header points to the first actual node in the list
  iter->current = sl->header->forward[0];

  return iter;
}

bool cdsa_has_next_skiplist(SkipListIterator *iter) {
  if (iter == NULL)
    return false;
  return iter->current != NULL;
}

CDSA_STATUS cdsa_next_skiplist(SkipListIterator *iter, double *out_score,
                          const char **out_value) {
  if (iter == NULL || iter->current == NULL) {
    return CDSA_ERR_NOT_FOUND;
  }

  // Extract the data safely
  if (out_score != NULL) {
    *out_score = iter->current->score;
  }
  if (out_value != NULL) {
    *out_value = iter->current->value;
  }

  // Advance the iterator along the Level 0 base list
  iter->current = iter->current->forward[0];

  return CDSA_OK;
}

void cdsa_free_skiplist_iterator(SkipListIterator *iter) {
  if (iter == NULL)
    return;
  CDSA_FREE(iter);
}

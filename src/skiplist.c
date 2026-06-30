#include "CDSA/skiplist.h"
#include <stdlib.h>
#include <string.h>

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

static SkipNode *create_node(int level, double score, const char *value) {
  SkipNode *node = malloc(sizeof(SkipNode));
  if (node == NULL) {
    return NULL; // bail before touching node->anything
  }

  node->value = strdup(value);
  if (node->value == NULL) {
    free(node); // undo the first allocation
    return NULL;
  }

  node->forward = calloc(level, sizeof(SkipNode *));
  if (node->forward == NULL) {
    free(node->value); // undo every allocation made so far
    free(node);
    return NULL;
  }

  node->score = score;
  node->level = level;
  return node;
}
// --- Lifecycle ---

SkipList *create_skiplist() {
  SkipList *sl = malloc(sizeof(SkipList));

  if (sl == NULL) {
    return NULL;
  }
  sl->level = 1;
  sl->size = 0;

  // The Dummy Header sits at max level but holds no real data
  sl->header = create_node(SKIPLIST_MAX_LEVEL, 0.0, "");

  if (sl->header == NULL) {
    free(sl);
    return NULL;
  }

  return sl;
}

void free_skiplist(SkipList *sl) {
  if (sl == NULL)
    return;

  SkipNode *current = sl->header;
  while (current != NULL) {
    SkipNode *next = current->forward[0]; // Level 0 is a standard linked list
    free(current->value);
    free(current->forward); // Free the dynamic array of pointers
    free(current);
    current = next;
  }
  free(sl);
}

// --- Core Operations ---

bool insert_skiplist(SkipList *sl, double score, const char *value) {
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

  // If it's the tallest node we've ever seen, update the header's routing
  if (new_level > sl->level) {
    for (int i = sl->level; i < new_level; i++) {
      update[i] = sl->header;
    }
    sl->level = new_level;
  }

  // Create the node and splice it in using our breadcrumbs
  SkipNode *new_node = create_node(new_level, score, value);
  for (int i = 0; i < new_level; i++) {
    new_node->forward[i] = update[i]->forward[i];
    update[i]->forward[i] = new_node;
  }

  sl->size++;
  return true;
}

bool remove_skiplist(SkipList *sl, double score, const char *value) {
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
    free(current->value);
    free(current->forward);
    free(current);

    sl->size--;
    return true; // Successfully deleted
  }

  return false; // Node didn't exist
}

char **get_range_skiplist(SkipList *sl, double min_score, double max_score,
                          int *out_count) {
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
  char **results = malloc(count * sizeof(char *));

  // 4. Populate the array by walking the Level 0 linked list
  int idx = 0;
  while (current != NULL && current->score <= max_score) {
    // strdup ensures the caller owns these strings!
    results[idx] = strdup(current->value);
    idx++;
    current = current->forward[0];
  }

  return results;
}

#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <stdbool.h>
#include <stddef.h>

#define SKIPLIST_MAX_LEVEL 16

typedef struct SkipNode {
  double score; // the sorting weight
  char *value;  // the actual data
  int level;    // How tall is this node (1 to MAX_LEVEL)
  struct SkipNode *
      *forward; // array of pointers to the next nodes at higher level
} SkipNode;

typedef struct {
  SkipNode *header; // starting node (dummy)
  int level;        // current highest level in use
  size_t size;      // total number of items
} SkipList;

// LifeCycle
SkipList *create_skiplist();
void free_skiplist(SkipList *sl);

// Core operations

// Returns true if inserted, false if it failed (like out of memory)
bool insert_skiplist(SkipList *sl, double score, const char *value);

// Returns true if found and deleted, false if it didn't exist
bool remove_skiplist(SkipList *sl, double score, const char *value);

char **get_range_skiplist(SkipList *sl, double min_score, double max_score,
                          int *out_score);

#endif

# Adaptive Radix Tree (ART) - Progress Report

## Current Status

This project is an educational implementation of an Adaptive Radix Tree (ART) written in C.

The current implementation supports:

* Node4
* Node16
* Node48
* Adaptive node promotion
* Leaf insertion
* Key search
* Recursive memory cleanup

---

## Implemented Features

### Adaptive Node Types

#### Node4

Supports up to 4 children.

```text
0-4 children
```

Used for maximum memory efficiency when occupancy is low.

---

#### Node16

Automatically created when a Node4 becomes full.

```text
Node4 -> Node16
```

Children and routing keys are migrated to the new node.

---

#### Node48

Automatically created when a Node16 becomes full.

```text
Node16 -> Node48
```

Uses:

* 256-byte lookup table
* 48-pointer compressed child array

to reduce memory overhead while maintaining fast lookups.

---

### Adaptive Promotion Engine

Implemented:

```text
Node4 -> Node16
Node16 -> Node48
```

Promotion copies:

* Header metadata
* Routing keys
* Child pointers

before safely freeing the old node.

---

### Search Engine

Implemented support for:

```c
search_art()
```

Supports:

* Node4 traversal
* Node16 traversal
* Node48 O(1) lookup-table traversal
* Leaf verification using strcmp()

---

### Memory Management

Implemented:

```c
free_art()
```

Features:

* Recursive teardown
* Child cleanup
* Leaf cleanup
* String deallocation
* Tree destruction

---

## Debugging Notes

### Segmentation Fault Encountered

During insertion development, a segmentation fault occurred while traversing leaf nodes.

Root Cause:

A missing pointer dereference.

Incorrect:

```c
ArtLeaf *leaf = (ArtLeaf *)current_ptr;
```

Correct:

```c
ArtLeaf *leaf = (ArtLeaf *)*current_ptr;
```

The bug was identified after multiple GDB debugging sessions.

Lesson learned:

```text
Pointer indirection matters.
```

---

## Node48 Design

Node48 uses:

```c
uint8_t child_index[256];
void *children[48];
```

### Sentinel Value

The value:

```text
255
```

represents an empty slot.

Valid child indices:

```text
0-47
```

Empty:

```text
255
```

This allows O(1) routing while avoiding the memory cost of storing 256 child pointers.

---

## Current Limitations

The implementation is not yet a full production ART.

Missing features include:

### Path Compression

Currently declared in the node header but not implemented.

```c
uint32_t prefix_len;
uint8_t prefix[10];
```

Path compression is one of the major optimizations described in the ART paper.

---

### Node256

Not implemented yet.

Current adaptive chain:

```text
Node4
↓
Node16
↓
Node48
```

Target chain:

```text
Node4
↓
Node16
↓
Node48
↓
Node256
```

---

### Deletion

Not implemented.

Future work includes:

* Leaf removal
* Node shrinking
* Tree rebalancing

---

### Node Shrinking

Currently supported:

```text
Node4 -> Node16
Node16 -> Node48
```

Future support:

```text
Node48 -> Node16
Node16 -> Node4
```

---

### Advanced Prefix Handling

Not implemented.

Required for:

* Path compression
* Prefix matching
* Efficient branching

---

## Educational Goals

This project is intended to teach:

* Systems Programming in C
* Memory Management
* Pointer Manipulation
* Tree Data Structures
* Database Indexing Concepts
* Cache-Aware Data Structures

---

## Roadmap

### Completed

* [x] Node4
* [x] Node16
* [x] Node48
* [x] Adaptive Promotion
* [x] Search
* [x] Memory Cleanup

### Next Steps

* [ ] Path Compression
* [ ] Node256
* [ ] Deletion
* [ ] Node Shrinking
* [ ] Prefix Matching
* [ ] Benchmarking

---

## Overall Assessment

Current implementation status:

```text
Educational ART : 9/10
Production ART  : 4/10
```

The adaptive node architecture is functional and demonstrates the core ideas behind ART.

Future work will focus on path compression, advanced prefix handling, deletion, and full production-level functionality.

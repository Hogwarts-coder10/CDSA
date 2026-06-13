# ⚙️ CDSA — Core Data Structures & Algorithms Library for Kedis-C

> “Before building the engine, build the components that power it.”

CDSA is a custom-built Data Structures & Algorithms library written in pure C that serves as the foundational infrastructure layer for:

```text
Kedis-C
```

Instead of depending heavily on external abstractions, CDSA focuses on understanding and implementing the low-level components required for systems software and database engineering.

This project exists to deeply understand:

* memory management,
* dynamic allocation,
* generic containers,
* abstraction design,
* low-level systems programming,
* and performance-oriented data structures.

---

# 🧠 Why CDSA Exists

Kedis-Python helped explore:

* database architecture,
* persistence systems,
* networking,
* recovery handling,
* and observability.

However, the upcoming:

```text
Kedis-C
```

requires deeper control over:

* memory,
* allocation,
* storage structures,
* and low-level performance.

CDSA is the preparation layer for that transition.

---

# 🏗️ Relationship with Kedis-C

CDSA is not a standalone academic DSA project.

It is intended to become the internal infrastructure toolkit powering:

```text
Kedis-C
```

Future integrations include:

| CDSA Component     | Kedis-C Usage                       |
| ------------------ | ----------------------------------- |
| Vector             | Dynamic storage buffers             |
| HashMap            | Keyspace engine                     |
| KString            | Protocol parsing & command handling |
| Stack              | Internal operation handling         |
| Skiplist (Planned) | Ordered indexes / sorted sets       |
| Queue (Planned)    | Request scheduling                  |
| Trees (Planned)    | Retrieval & indexing systems        |

---

# 🚀 Current Implementations

## 📦 Vector

A generic dynamically resizing array implementation.

### Features

* Generic element storage using `void *`
* Dynamic resizing
* Random access
* Front/Back operations
* Memory-safe resizing logic

### Systems Concepts

* Pointer arithmetic
* Heap memory management
* Generic abstraction design
* Capacity growth strategies

---

## 🔗 Linked List

A singly linked list implementation.

### Features

* Push front
* Pop front
* Traversal
* Size tracking
* Memory cleanup

### Systems Concepts

* Dynamic node allocation
* Pointer traversal
* Linked memory structures

---

## 🧱 Stack

A stack abstraction built on top of the vector implementation.

### Features

* Push
* Pop
* Top access
* Size tracking
* Empty checking

### Systems Concepts

* Abstraction layering
* Internal container reuse
* Systems-oriented composition

---

## 🧵 KString

A custom dynamic string implementation.

### Features

* Dynamic resizing
* String append operations
* Capacity tracking
* Null-terminated compatibility

### Systems Concepts

* Buffer management
* String memory handling
* Reallocation strategies

---

## 🗂️ HashMap (WIP)

A foundational hashmap implementation intended to power future Kedis-C keyspace storage.

### Planned Features

* Hash functions
* Collision handling
* Rehashing
* Key lookup
* Deletion support

---

# 🏎️ Long-Term Goal

The long-term vision is to evolve CDSA into a reusable low-level systems toolkit for:

* database engines,
* retrieval systems,
* AI infrastructure,
* vector indexing,
* and systems-oriented experimentation.

---

# 🧠 Philosophy

Most DSA learning focuses on:

```text
solving interview questions
```

CDSA focuses on:

```text
understanding how systems are built underneath
```

The goal is not only to:

```text
use abstractions
```

but to:

```text
build abstractions
```

that can power larger systems like:

```text
Kedis-C
```

---

# 🏗️ Project Structure

```text
CDSA/
├── include/
│   └── CDSA/
│       ├── vector.h
│       ├── linkedlist.h
│       ├── stack.h
│       ├── kstring.h
│       └── hashmap.h
│
├── src/
│   ├── vector.c
│   ├── linkedlist.c
│   ├── stack.c
│   ├── kstring.c
│   └── hashmap.c
│
├── tests/
│   ├── test_vector.c
│   ├── test_linkedlist.c
│   ├── test_stack.c
│   ├── test_kstring.c
│   └── test_hashmap.c
│
└── CMakeLists.txt
```

---

# 🔨 Build Instructions

## Requirements

* GCC / Clang
* CMake 3.20+

---

## Build

```bash
mkdir build
cd build

cmake ..
make
```

---

## Run Tests

Example:

```bash
./test_vector
./test_stack
./test_kstring
```

---

# 🚀 Planned Implementations

## Core Structures

* Queue
* Deque
* Binary Search Tree
* AVL Tree
* Red-Black Tree
* Skiplist
* Trie
* Graph
* Heap

---

## Systems Improvements

* Arena allocators
* Iterators
* Better memory safety
* Benchmark suite
* Performance profiling
* Generic macros
* Internal debugging utilities

---

# 🐧 Future Vision

The ultimate goal is:

```text
CDSA
↓
Kedis-C
↓
Advanced Systems Projects
↓
AI Infrastructure & Retrieval Systems
```

CDSA is the calm before the storm.

It is the foundational layer where:

* low-level memory understanding,
* systems intuition,
* and reusable infrastructure

are built before moving into larger systems engineering projects.

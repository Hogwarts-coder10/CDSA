# ⚙️ CDSA — Core Data Structures & Algorithms Library for Kedis-C

> "Before building the engine, build the components that power it."

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

Planned integrations include:

| CDSA Component        | Kedis-C Usage                           |
| ---------------------- | ---------------------------------------- |
| Vector                  | Dynamic storage buffers                  |
| HashMap                 | Keyspace engine                          |
| KString                 | Protocol parsing & command handling      |
| Stack / Queue / Deque   | Internal operation handling & scheduling |
| SkipList                | Ordered indexes / sorted sets            |
| ART                     | Retrieval & prefix-indexed lookups       |
| `cdsa_arena` / `kpool`  | Allocation strategy for hot-path objects |

---

# 🚀 Current Implementations

CDSA now has **11 core containers**, all generic (`void*`-backed), all with dedicated unit tests, and most with their own mutation-safe iterators.

## 📦 Vector
Generic dynamically resizing array. Random access, front/back operations, capacity growth strategy, iterator support.

## 🔗 Linked List
Generic singly linked list. Push/pop front, traversal, size tracking, safe payload teardown, iterator support.

## 🧱 Stack
Generic LIFO container, layered on top of Vector.

## 🧵 KString
Dynamic, auto-expanding string implementation. Append operations, capacity tracking, null-terminated compatibility.

## 🚦 Queue
Generic FIFO container.

## ⏫ Deque
Double-ended queue for O(1) head/tail push and pop.

## 🔁 RingBuffer
Fixed-capacity circular buffer for bounded producer/consumer workloads. Includes its own iterator.

## 🏔️ Priority Queue
Generic Min/Max heap backed by Vector.

## 🗂️ HashMap
Open-addressing hash table using **Robin Hood hashing** (probe-sequence-length tracking + backward-shift deletion — no tombstones). Auto-resizes at 75% load factor. Includes a mutation-safe iterator guarded by a snapshot version counter.

## 🪜 SkipList
Probabilistic O(log N) alternative to balanced trees, used as the basis for future sorted-set support in Kedis-C. Includes an iterator (currently unguarded against concurrent mutation — see Known Limitations).

## 🌳 Adaptive Radix Tree (ART)
High-performance prefix tree with dynamic node morphing (Node4 → Node16 → Node48 → Node256), path compression, and prefix gluing. Node allocation is routed through `kpool` slab allocators rather than raw `malloc`/`free` per node. Includes its own iterator.

## 🧮 Allocator Layer
* **`cdsa_arena`** — bump allocator with 8-byte alignment, `arena_alloc`/`arena_reset`/`arena_destroy`.
* **`kpool`** — fixed-size slab/free-list allocator, currently used internally by ART for all node classes.
* **`CDSA_STATUS`** — unified error enum (`CDSA_OK`, `CDSA_ERR_OOM`, `CDSA_ERR_NOT_FOUND`, `CDSA_ERR_EXISTS`, `CDSA_ERR_INVALID`, `CDSA_ERR_FULL`, `CDSA_ERR_EMPTY`, `CDSA_ERR_ITER_INVALIDATED`) with `cdsa_strerror()`.
* Allocation is routed through `CDSA_MALLOC` / `CDSA_CALLOC` / `CDSA_REALLOC` / `CDSA_FREE` macros so the whole library can be repointed at a custom allocator later (e.g. `kalloc`, still planned).

---

# 📈 Performance

Benchmarked against the C++ STL on 1,000,000 operations with randomized UUID-like keys (`-O3`, GCC). Full methodology and raw numbers in [`docs/BENCHMARKS.md`](docs/BENCHMARKS.md).

| Workload | CDSA | STL Baseline | Result |
| --- | --- | --- | --- |
| HashMap, preallocated | 8.43M ops/sec | `std::unordered_map`: 4.88M ops/sec | ~1.73x faster |
| HashMap, dynamic growth (11 resizes) | 4.15M ops/sec | `std::unordered_map`: 2.74M ops/sec | ~1.51x faster |
| Ordered index (ART vs. Red-Black Tree) | 2.66M ops/sec | `std::map`: 1.21M ops/sec | ~2.20x faster |

ART sustains ~84.29 MB heap usage for 1,000,000 uncompressible 32-byte keys. HashMap sustains up to 7.4M ops/sec under a mixed 50/50 read/write workload.

---

# 🛡️ Memory Safety

Every structure is stress-tested with a randomized chaos-monkey fuzzer (`bench/bench_fuzzer.c`) under Valgrind Memcheck, and the allocator layer can be built with AddressSanitizer + UndefinedBehaviorSanitizer via the `CDSA_USE_SANITIZERS` CMake option. GitHub Actions CI runs the fuzzer under Valgrind on every push and pull request.

**Known limitation:** the fuzzer and CI currently catch memory-safety regressions (leaks, use-after-free, invalid access) but do **not** run the correctness unit tests (`tests/test_*.c`) in CI — those still need `add_test()` registration and a CI step. See [`CDSA_REVIEW_ISSUES.md`](CDSA_REVIEW_ISSUES.md) for the current punch list, including this and the skiplist iterator's missing mutation guard.

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
│       ├── queue.h
│       ├── deque.h
│       ├── ringbuffer.h
│       ├── priority_queue.h
│       ├── hashmap.h
│       ├── skiplist.h
│       ├── art.h
│       ├── allocator.h
│       └── error.h
│
├── src/
│   ├── vector.c
│   ├── linkedlist.c
│   ├── stack.c
│   ├── kstring.c
│   ├── queue.c
│   ├── deque.c
│   ├── ringbuffer.c
│   ├── priority_queue.c
│   ├── hashmap.c
│   ├── skiplist.c
│   ├── art.c
│   ├── allocator.c
│   └── error.c
│
├── tests/
│   ├── test_vector.c / test_vector_iterator.c
│   ├── test_linkedlist.c / test_linkedlist_iterator.c
│   ├── test_stack.c
│   ├── test_kstring.c
│   ├── test_queue.c
│   ├── test_deque.c
│   ├── test_ringbuffer.c / test_ringbuffer_iterator.c
│   ├── test_priority_queue.c
│   ├── test_hashmap.c / test_hashmap_iterator.c
│   ├── test_skiplist.c / test_skiplist_iterator.c
│   ├── test_art.c / test_art_iterator.c
│   ├── test_allocator.c
│   └── test_wrappers_iterators.c
│
├── bench/
│   ├── bench_collections.c
│   ├── bench_workloads.c
│   ├── bench_stl_baseline.cpp
│   └── bench_fuzzer.c
│
├── docs/
│   └── BENCHMARKS.md
│
├── .github/workflows/ci.yml
├── CONTRIBUTING.md
├── ROADMAP.md
└── CMakeLists.txt
```

---

# 🔨 Build Instructions

## Requirements
* GCC / Clang
* CMake 3.20+

## Build
```bash
mkdir build
cd build
cmake ..
make
```

## Build with sanitizers
```bash
cmake -B build-san -DCDSA_USE_SANITIZERS=ON
cmake --build build-san
```

## Build with benchmarks / fuzzer
```bash
cmake -B build-bench -DCMAKE_BUILD_TYPE=Release -DCDSA_BUILD_BENCHMARKS=ON
cmake --build build-bench
./build-bench/bench_fuzzer
```

## Run tests
Each test compiles to its own executable, e.g.:
```bash
./build/test_vector
./build/test_hashmap
./build/test_skiplist_iterator
```
(Not yet wired into `ctest` — run the binaries directly for now.)

---

# 🚀 Planned / In Progress

* `kalloc` integration (custom global allocator backing `CDSA_MALLOC`/`CDSA_FREE`)
* Wiring `add_test()` + `ctest` into CI so correctness regressions, not just crashes/leaks, fail the build
* Mutation guard for the SkipList iterator (matching the HashMap iterator's version-check pattern)
* `cdsa_*` namespace rename across all modules
* Trie, Graph, and general tree structures (BST/AVL/Red-Black) as needed by future Kedis-C features

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

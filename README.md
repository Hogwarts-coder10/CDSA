# CDSA (Core Data Structures & Algorithms)

![Build Status](https://img.shields.io/badge/build-passing-brightgreen) ![Memory Safety](https://img.shields.io/badge/Valgrind-0%20Leaks-blue) ![Language](https://img.shields.io/badge/Language-C11-orange)

A production-grade, fully generic, and strictly memory-safe data structures toolkit written in bare-metal C. 

CDSA is designed to be a lightweight, zero-dependency alternative to standard libraries, providing highly optimized containers ranging from standard dynamic arrays to complex database-level indexes like the Adaptive Radix Tree.

## 🚀 Features

All structures are fully generic (`void*` backed), heavily tested, and verified for absolute memory safety.

**Linear Structures**
*   `Vector` - Dynamic contiguous array with automatic reallocation.
*   `LinkedList` - Generic singly-linked list with safe payload teardown.
*   `KString` - Safe, dynamic string implementation with auto-expansion.
*   `Stack` - Generic LIFO container.
*   `Queue` - Generic FIFO container.
*   `Deque` - Double-ended queue for fast head/tail operations.
*   `RingBuffer` - Highly efficient, fixed-size circular queue.

**Advanced Routing & Indexing**
*   `Priority Queue` - Generic Min/Max heap backed by a dynamic Vector.
*   `HashMap` - O(1) generic key-value store.
*   `SkipList` - O(log N) probabilistic alternative to balanced trees for sorted sets.
*   `Adaptive Radix Tree (ART)` - High-performance prefix tree supporting dynamic node morphing (Node4, Node16, Node48, Node256), surgical path compression, and prefix gluing.

## 🏎️ Performance & Telemetry

CDSA includes a built-in advanced workload simulator to track operations per second (ops/sec) and precise heap memory footprints. 

*   **ART Engine:** Dynamically packs 1,000,000 completely randomized, un-compressible keys into just `84.29 MB` of RAM, sustaining ~3.6 Million ops/sec during randomized UUID insertions.
*   **HashMap:** Auto-resizes at exactly 75% capacity to prevent Primary Clustering, sustaining ~7.4 Million ops/sec under heavy concurrent read/write cross-traffic.

## 🛡️ Memory Safety Guarantee

Every data structure in this library has been aggressively stress-tested using Valgrind. CDSA guarantees strict memory safety: all internal allocations, node morphing, array shifts, and string expansions are completely leak-free. The library routes through a custom global tracking allocator (`CDSA_MALLOC`, `CDSA_FREE`) to monitor exact byte footprints, ensuring 0 bytes lost across all teardown sequences.

## 📦 System-Wide Installation

CDSA uses CMake and can be installed globally on your machine. This allows you to include it in any project using standard angle brackets (e.g., `#include <CDSA/vector.h>`).

```bash
# 1. Clone the repository
git clone [https://github.com/yourusername/CDSA.git](https://github.com/yourusername/CDSA.git)
cd CDSA

# 2. Configure and build the static library
cmake -S . -B build
cmake --build build

# 3. Install globally (Requires Admin Privileges)
sudo cmake --install build
```

## ⚡ Quick Start

Once installed, you can use CDSA in any C project anywhere on your computer.
```C
#include <CDSA/vector.h>
#include <stdio.h>

int main() {
    // Create a vector that holds integers
    cdsa_vector *vec = cdsa_create_vector(sizeof(int));

    int a = 42, b = 100;
    cdsa_push_vector(vec, &a);
    cdsa_push_vector(vec, &b);

    printf("First item: %d\n", *(int*)cdsa_get_vector(vec, 0));
    printf("Total size: %zu\n", cdsa_size_vector(vec));

    // Safely free all memory
    cdsa_free_vector(vec);
    return 0;
}
```
**Compile and Link:** Tell GCC to link the installed library using the `-lcdsa` flag:
```bash
gcc main.c -lcdsa -o my_app
./my_app
```
## 📊 Running Benchmarks

If you want to run the wind tunnel tests yourself, you can compile the benchmarking executables:
```# Configure the build with benchmarks enabled
cmake -B build-bench -DCDSA_BUILD_BENCHMARKS=ON

# Compile the executables
cmake --build build-bench

# Run the standard benchmarks & load factor stress test
./build-bench/bench_collections

# Run the Advanced Workload Simulator (Memory & Mixed R/W)
./build-bench/bench_workloads
```


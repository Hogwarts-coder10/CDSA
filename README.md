# CDSA (Core Data Structures & Algorithms)

A production-grade, fully generic, and strictly memory-safe data structures toolkit written in bare-metal C. 

CDSA is designed to be a lightweight, zero-dependency alternative to standard libraries, providing highly optimized containers ranging from standard dynamic arrays to complex database-level indexes like the Adaptive Radix Tree.

## 🚀 Features

All structures are fully generic (`void*` backed), heavily tested, and verified for absolute memory safety (0 bytes lost via Valgrind).

**Linear Structures**
- `Vector` - Dynamic contiguous array with automatic reallocation.
- `LinkedList` - Generic singly-linked list with safe payload teardown.
- `KString` - Safe, dynamic string implementation with auto-expansion.
- `Stack` - Generic LIFO container.
- `Queue` - Generic FIFO container.
- `Deque` - Double-ended queue for fast head/tail operations.
- `RingBuffer` - Highly efficient, fixed-size circular queue.

**Advanced Routing & Indexing**
- `Priority Queue` - Generic Min/Max heap backed by a dynamic Vector.
- `HashMap` - O(1) generic key-value store.
- `SkipList` - O(log N) probabilistic alternative to balanced trees for sorted sets.
- `Adaptive Radix Tree (ART)` - High-performance prefix tree supporting dynamic node morphing (Node4, Node16, Node48, Node256), surgical path compression, and prefix gluing.

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
```c
#include <CDSA/vector.h>
#include <stdio.h>

int main() {
    // Create a vector that holds integers
    Vector *vec = create_vector(sizeof(int));

    int a = 42, b = 100;
    push_vector(vec, &a);
    push_vector(vec, &b);

    printf("First item: %d\n", *(int*)get_vector(vec, 0));
    printf("Total size: %zu\n", size_vector(vec));

    // Safely free all memory
    free_vector(vec);
    return 0;
}
```

## Compile and Link:
Tell GCC to link the installed library using the -lcdsa flag:
```bash
gcc main.c -lcdsa -o my_app
./my_app
```

## 🛡️ Memory Safety Guarantee
Every data structure in this library has been aggressively stress-tested using Valgrind. CDSA guarantees strict memory safety: all internal allocations, node morphing, array shifts, and string expansions are completely leak-free (0 bytes in 0 blocks).

# 📊 CDSA Benchmark Results & Performance Telemetry

This document records the empirical performance characteristics, memory efficiency, and standard library baseline comparisons for the **CDSA (Core Data Structures & Algorithms)** C library.

All benchmarks were executed on **1,000,000 operations** using randomly generated UUID-like ASCII keys (`-O3` release build, GCC engine).

---

## 🏁 1. The Rival Baseline: CDSA vs. C++ STL

We pitted CDSA's bare-metal C containers directly against the C++ Standard Template Library (`std::unordered_map` and `std::map`).

### Test A: Preallocated Capacity (Pure Throughput)
*   `std::unordered_map::reserve(1,000,000)`
*   `cdsa_create_hashmap(2,097,152)` (pre-sized to prevent reallocations)

| Container | Implementation | Execution Time | Throughput | vs. STL Baseline |
| :--- | :--- | :---: | :---: | :---: |
| **C++ STL** | `std::unordered_map` | 0.2049s | 4.88M ops/sec | 1.0x (Baseline) |
| **CDSA** | `cdsa_hashmap` | **0.1186s** | **8.43M ops/sec** | **~1.73x Faster (⚡ +73%)** |

> **Key Takeaway:** `std::unordered_map` relies on separate chaining (linked lists per bucket), introducing pointer-chasing CPU cache misses. CDSA's open-addressing linear probing keeps memory contiguous, feeding CPU L1/L2 caches at maximum bandwidth.

---

### Test B: Automatic Dynamic Growth
*   `std::unordered_map` (default constructor)
*   `cdsa_create_hashmap(1024)` (started tiny, resized/rehashed **11 times** up to 2.09M slots)

| Container | Implementation | Resizes | Execution Time | Throughput | vs. STL Baseline |
| :--- | :--- | :---: | :---: | :---: | :---: |
| **C++ STL** | `std::unordered_map` | Dynamic | 0.3654s | 2.74M ops/sec | 1.0x (Baseline) |
| **CDSA** | `cdsa_hashmap` | 11 Resizes | **0.2410s** | **4.15M ops/sec** | **~1.51x Faster (⚡ +51%)** |

> **Key Takeaway:** Even with the performance penalty of 11 sequential heap reallocations and full-table rehashes, CDSA's cache-friendly bucket layout outpaces C++ STL's default growth engine by over 50%.

---

### Test C: Ordered Indexing (ART vs. Red-Black Tree)

| Container | Implementation | Execution Time | Throughput | vs. STL Baseline |
| :--- | :--- | :---: | :---: | :---: |
| **C++ STL** | `std::map` (Red-Black Tree) | 0.8237s | 1.21M ops/sec | 1.0x (Baseline) |
| **CDSA** | `cdsa_art_tree` (Adaptive Radix Tree) | **0.3763s** | **2.66M ops/sec** | **~2.20x Faster (⚡ +120%)** |

> **Key Takeaway:** Red-Black Trees require frequent pointer adjustments and rotations to maintain balance ($O(\log N)$ tree depth). CDSA's Adaptive Radix Tree uses path compression, prefix gluing, and dynamic node morphing (`Node4` $\rightarrow$ `Node256`), eliminating rebalancing passes.

---

## 📈 2. Advanced Workload Simulation & Telemetry

Using CDSA's built-in global allocator wrappers (`CDSA_MALLOC` / `CDSA_FREE`), heap consumption was recorded across 1,000,000 randomized operations.

### Adaptive Radix Tree (ART)
*   **Memory Footprint:** **84.29 MB** total heap utilization for 1,000,000 uncompressible 32-byte UUID keys (~88 bytes/key including all node headers and prefixes).
*   **Sequential / Best-Case Path Compression:** >3.6 Million ops/sec.
*   **Random UUID Insertion:** ~226k–2.65M ops/sec (depending on CPU cache warm-state).

### Open-Addressing HashMap
*   **Mixed R/W Workload (50% Insert / 50% Read):** Up to **7.4 Million ops/sec**.
*   **Max Capacity Load Factor Constraint:** Automatically resizes at **75% capacity** to eliminate primary clustering degradation.

---

## 🛡️ 3. Memory Safety & Leak Verification

All benchmark targets were profiled under `Valgrind Memcheck` (v3.25.1) and AddressSanitizer (ASan).

```text
==19467== HEAP SUMMARY:
==19467==     in use at exit: 0 bytes in 0 blocks
==19467==   total heap usage: 3,477,504 allocs, 3,477,504 frees, 188,014,864 bytes allocated
==19467== 
==19467== All heap blocks were freed -- no leaks are possible
==19467== ERROR SUMMARY: 0 errors from 0 contexts
```
```
```
```
```
```


Definite / Indirect Leaks: 0 bytes

Pointer Integrity: Clean tree node collapsing, prefix detachment, and table teardowns verified.

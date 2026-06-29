# Contributing to CDSA (Kedis-C Storage Engine)

First off, thank you for considering contributing! 

This library serves as the foundational bare-metal data structures engine for Kedis-C. Because we are building a high-performance, Redis-inspired in-memory database, we have to be absolutely ruthless about memory safety, pointer arithmetic, and algorithmic efficiency. 

Bare-metal C is unforgiving. Please read through these guidelines before submitting a Pull Request to ensure your code matches the project's strict architecture standards.

## 🧠 Core Philosophy

* **Memory Safety is Non-Negotiable:** A single missing tombstone check, out-of-bounds read, or unhandled allocation failure brings the whole server down.
* **Bone-Clean Compilation:** The project must compile with zero warnings under strict `gnu11` flags.
* **Self-Documenting Code:** No magic numbers. If a value has distinct logical meaning (like `255` representing an empty slot), use a `#define` (e.g., `ART_EMPTY_SLOT`).

## 🛠️ Local Development Setup

1. **Clone the repository:**
   ```bash
   git clone [https://github.com/YOUR-USERNAME/YOUR-REPO-NAME.git](https://github.com/YOUR-USERNAME/YOUR-REPO-NAME.git)
   cd YOUR-REPO-NAME

# cpp-from-scratch

[![CI](https://github.com/ca0904/cpp-from-scratch/actions/workflows/ci.yml/badge.svg)](https://github.com/ca0904/cpp-from-scratch/actions/workflows/ci.yml)

C++ implementations of standard library types and concurrency primitives, written to
understand how they work.

| Folder | Contents |
| --- | --- |
| `os/` | threads, atomics, mutex, semaphore, condition variable, reader-writer lock, producer-consumer, dining philosophers, thread pool, promise/future/async |
| `smartptr/` | `auto_ptr`, `unique_ptr`, `shared_ptr` |
| `stl/` | vector, hash table, BST, AVL tree, red-black tree |
| `oop/` | encapsulation, abstraction, inheritance, polymorphism |

## Tests

The classes are tested in `tests/` against their standard-library counterparts, under
AddressSanitizer and UndefinedBehaviorSanitizer, with ThreadSanitizer for the threaded ones:

```sh
tests/run.sh        # or: tests/run.sh g++
```

GitHub Actions runs them with GCC 16 and clang on Ubuntu 26.04 on every push. Before
GCC 16, libstdc++'s `std::counting_semaphore` can deadlock under contention
([GCC bug 104928](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=104928)), which
occasionally hung the semaphore-based producer-consumer, dining philosophers and
reader-writer.

## Pre-commit hook

Checks that each staged file is clang-formatted and compiles. Enable it once per clone:

```sh
git config core.hooksPath .githooks
```

## Competitive programming

My contest snippets (modular arithmetic, combinatorics, sieve, Euler's totient, extended
GCD, matrix power, DSU, sparse table, Fenwick tree, segment trees with lazy propagation,
merge sort tree, LCA, heavy-light decomposition, SCC with condensation, bridges and
articulation points, rolling hash, KMP, Z-function, Manacher, tries) are in a gist:
[cp-snippets.cpp](https://gist.github.com/ca0904/435fba67992f697ede85d4e24389a7b8).

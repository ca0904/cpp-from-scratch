// Counts live allocations made through operator new, so a test can check that
// everything it allocated was freed
#pragma once
#include <cstdlib>
#include <new>

inline long long g_live = 0;

void *operator new(std::size_t n) {
  ++g_live;
  if (void *p = std::malloc(n ? n : 1))
    return p;
  throw std::bad_alloc();
}
void *operator new[](std::size_t n) { return operator new(n); }
void operator delete(void *p) noexcept {
  if (p) {
    --g_live;
    std::free(p);
  }
}
void operator delete[](void *p) noexcept { operator delete(p); }
void operator delete(void *p, std::size_t) noexcept { operator delete(p); }
void operator delete[](void *p, std::size_t) noexcept { operator delete(p); }

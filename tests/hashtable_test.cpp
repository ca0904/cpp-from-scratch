// HashTable against std::set on random operations, including capacity 0
#include <cassert>
#include <cstdio>
#include <random>
#include <set>

#include "../stl/hashtable.cpp"

int main() {
  std::mt19937 rng(5);
  for (std::size_t capacity : {0, 1, 2, 16}) {
    HashTable h(capacity);
    std::set<int> r;
    for (int op = 0; op < 20000; op++) {
      int x = (int)(rng() % 2000) - 1000;
      if (rng() % 3) {
        h.insert(x);
        r.insert(x);
      } else {
        h.erase(x);
        r.erase(x);
      }
      int q = (int)(rng() % 2000) - 1000;
      assert(h.contains(q) == (bool)r.count(q));
    }
  }
  puts("hashtable: OK");
}

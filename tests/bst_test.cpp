// BST against std::set on random operations
#include "alloc_count.hpp"
#include <cassert>
#include <climits>
#include <cstdio>
#include <random>
#include <set>

// the test reads the tree's internals to check the ordering
#define private public
#include "../stl/self_balancing_trees/bst.cpp"
#undef private

bool ordered(Node *n, long long lo, long long hi) {
  return !n || (lo < n->val && n->val < hi && ordered(n->left, lo, n->val) &&
                ordered(n->right, n->val, hi));
}

int main() {
  std::mt19937 rng(2);
  long long base = g_live;
  for (int round = 0; round < 300; round++) {
    BST t;
    std::set<int> r;
    for (int op = 0; op < 300; op++) {
      int x = rng() % 50;
      if (rng() % 3) {
        t.insert(x);
        r.insert(x);
      } else {
        t.erase(x);
        r.erase(x);
      }
      assert(ordered(t.root, LLONG_MIN, LLONG_MAX));
      int q = rng() % 50;
      assert(t.search(q) == (bool)r.count(q));
      assert(t.min() == (r.empty() ? -1 : *r.begin()));
      assert(t.max() == (r.empty() ? -1 : *r.rbegin()));
    }
  }
  assert(g_live == base); // every node freed
  puts("bst: OK");
}

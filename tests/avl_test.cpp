// AVL against std::set on random operations, checking heights and balance
#include "alloc_count.hpp"
#include <algorithm>
#include <cassert>
#include <climits>
#include <cstdio>
#include <random>
#include <set>

// the test reads the tree's internals to check heights and ordering
#define private public
#include "../stl/self_balancing_trees/avl.cpp"
#undef private

bool ordered(Node *n, long long lo, long long hi) {
  return !n || (lo < n->val && n->val < hi && ordered(n->left, lo, n->val) &&
                ordered(n->right, n->val, hi));
}

int realHeight(Node *n) {
  return n ? 1 + std::max(realHeight(n->left), realHeight(n->right)) : -1;
}

bool heightsCorrect(Node *n) {
  return !n || (n->height == realHeight(n) && heightsCorrect(n->left) &&
                heightsCorrect(n->right));
}

int main() {
  std::mt19937 rng(3);
  long long base = g_live;
  for (int round = 0; round < 300; round++) {
    AVL t;
    std::set<int> r;
    for (int op = 0; op < 300; op++) {
      int x = rng() % 60;
      if (rng() % 3) {
        t.insert(x);
        r.insert(x);
      } else {
        t.erase(x);
        r.erase(x);
      }
      assert(ordered(t.root, LLONG_MIN, LLONG_MAX));
      assert(heightsCorrect(t.root) && t.isBalanced());
      int q = rng() % 60;
      assert(t.search(q) == (bool)r.count(q));
      assert(t.min() == (r.empty() ? -1 : *r.begin()));
      assert(t.max() == (r.empty() ? -1 : *r.rbegin()));
    }
  }
  {
    // sorted inserts stay balanced: an AVL tree of 100000 keys has height <= 24
    AVL t;
    for (int i = 0; i < 100000; i++)
      t.insert(i);
    assert(realHeight(t.root) <= 24);
  }
  assert(g_live == base); // every node freed
  puts("avl: OK");
}

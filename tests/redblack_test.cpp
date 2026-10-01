// RedBlackTree: every red-black rule checked after every operation, on random
// operations, on every insertion order of 8 keys and on large patterns
#include "alloc_count.hpp"
#include <algorithm>
#include <cassert>
#include <climits>
#include <cstdio>
#include <numeric>
#include <random>
#include <set>
#include <vector>

// the test reads the tree's internals to check colours and parent pointers
#define private public
#include "../stl/self_balancing_trees/redblack.cpp"
#undef private

// Black height of the subtree, or -1 if a rule or the ordering is broken
int check(RedBlackTree &t, Node *n, long long lo, long long hi) {
  if (n == t.NIL)
    return 1;
  if (!(lo < n->val && n->val < hi))
    return -1;
  if (n->left != t.NIL && n->left->parent != n)
    return -1;
  if (n->right != t.NIL && n->right->parent != n)
    return -1;
  if (n->color == RED && (n->left->color == RED || n->right->color == RED))
    return -1;
  int a = check(t, n->left, lo, n->val), b = check(t, n->right, n->val, hi);
  if (a < 0 || b < 0 || a != b)
    return -1;
  return a + (n->color == BLACK);
}

int count(RedBlackTree &t, Node *n) {
  return n == t.NIL ? 0 : 1 + count(t, n->left) + count(t, n->right);
}

void valid(RedBlackTree &t, const std::set<int> &r) {
  assert(t.root->color == BLACK && t.NIL->color == BLACK);
  assert(check(t, t.root, LLONG_MIN, LLONG_MAX) > 0);
  assert(count(t, t.root) == (int)r.size());
  for (int v : r)
    assert(t.search(v));
}

int main() {
  long long base = g_live;

  // random inserts and erases, including keys that aren't present
  std::mt19937 rng(4);
  for (int round = 0; round < 300; round++) {
    RedBlackTree t;
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
      valid(t, r);
      int q = rng() % 60;
      assert(t.search(q) == (bool)r.count(q));
    }
  }

  // every insertion order of 8 keys, erased in ascending, descending and
  // insertion order
  {
    std::vector<int> keys(8);
    std::iota(keys.begin(), keys.end(), 1);
    do {
      for (int mode = 0; mode < 3; mode++) {
        RedBlackTree t;
        std::set<int> r;
        for (int x : keys) {
          t.insert(x);
          r.insert(x);
          valid(t, r);
        }
        std::vector<int> order = keys;
        if (mode == 0)
          std::sort(order.begin(), order.end());
        if (mode == 1)
          std::sort(order.rbegin(), order.rend());
        for (int x : order) {
          t.erase(x);
          r.erase(x);
          valid(t, r);
        }
      }
    } while (std::next_permutation(keys.begin(), keys.end()));
  }

  // large patterns: sorted inserts, erasing every other key, reverse inserts
  {
    RedBlackTree t;
    std::set<int> r;
    for (int i = 0; i < 20000; i++) {
      t.insert(i);
      r.insert(i);
    }
    valid(t, r);
    for (int i = 0; i < 20000; i += 2) {
      t.erase(i);
      r.erase(i);
    }
    valid(t, r);
    for (int i = 19999; i >= 0; i -= 3) {
      t.insert(i);
      r.insert(i);
    }
    valid(t, r);
    for (int i = 19999; i >= 0; i--) {
      t.erase(i);
      r.erase(i);
    }
    valid(t, r);
    assert(t.root == t.NIL);
  }

  assert(g_live == base); // every node freed
  puts("red-black: OK");
}

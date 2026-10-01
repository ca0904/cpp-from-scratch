// Vector against std::vector on random operations, with std::string elements
#include "alloc_count.hpp"
#include <cassert>
#include <cstdio>
#include <random>
#include <string>
#include <vector>

#include "../stl/vectorimpl.cpp"

using S = std::string;

bool same(const Vector<S> &v, const std::vector<S> &r) {
  if (v.size() != r.size())
    return false;
  const S *p = v.begin();
  for (std::size_t i = 0; i < r.size(); i++)
    if (p[i] != r[i])
      return false;
  return true;
}

int main() {
  std::mt19937 rng(1);
  auto randomString = [&] { return S(1 + rng() % 40, char('a' + rng() % 3)); };
  long long base = g_live;
  {
    for (int round = 0; round < 300; round++) {
      Vector<S> v;
      std::vector<S> r;
      for (int op = 0; op < 200; op++) {
        int k = rng() % 10;
        if (k < 3) {
          S s = randomString();
          v.push_back(s);
          r.push_back(s);
        } else if (k == 3) {
          S s = randomString();
          S copy = s;
          v.push_back(std::move(copy));
          r.push_back(s);
        } else if (k == 4 && !r.empty()) {
          // an element of the vector itself, possibly while it's full
          std::size_t i = rng() % r.size();
          v.push_back(v[i]);
          r.push_back(r[i]);
        } else if (k == 5 && !r.empty()) {
          v.pop_back();
          r.pop_back();
        } else if (k == 6) {
          std::size_t n = rng() % 20;
          S s = randomString();
          v.resize(n, s);
          r.resize(n, s);
        } else if (k == 7) {
          v.reserve(rng() % 40);
        } else if (k == 8) {
          v.emplace_back(5, 'z');
          r.emplace_back(5, 'z');
        } else {
          Vector<S> copied = v;
          assert(copied == v);
          Vector<S> assigned;
          assigned = copied;
          assert(same(assigned, r));
          Vector<S> moved = std::move(assigned);
          assert(same(moved, r) && assigned.empty());
        }
        assert(same(v, r) && v.capacity() >= v.size());
      }
      Vector<S> w{S("a"), S("b")};
      w.swap(v);
      assert(same(w, r) && v.size() == 2 && v[1] == "b");
      w.assign(3, S("q"));
      assert(w.size() == 3 && w.back() == "q");
      w.clear();
      assert(w.empty());
    }

    // push_back of its own element when full used to read freed memory
    Vector<S> v;
    v.push_back(S(40, 'x'));
    v.push_back(v[0]);
    assert(v[1] == S(40, 'x'));

    // copying a const Vector used not to compile
    const Vector<int> cv{1, 2, 3};
    Vector<int> copy = cv;
    int sum = 0;
    for (int x : cv)
      sum += x;
    assert(copy == cv && sum == 6);
  }
  assert(g_live == base); // everything freed
  puts("vector: OK");
}

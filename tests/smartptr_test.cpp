// UniquePtr, AutoPtr and SharedPtr: every object destroyed exactly once, and
// SharedPtr copies shared between threads (run this under ThreadSanitizer too)
#include <atomic>
#include <cassert>
#include <cstdio>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

#include "../smartptr/autoptr.cpp"
#include "../smartptr/sharedptr.cpp"
#include "../smartptr/uniqueptr.cpp"

std::atomic<int> alive{0}, destroyed{0};

struct Tracked {
  int v;
  explicit Tracked(int v) : v(v) { ++alive; }
  ~Tracked() {
    --alive;
    ++destroyed;
  }
};

// a raw pointer must not silently become an owner
static_assert(!std::is_convertible_v<Tracked *, UniquePtr<Tracked>>);
static_assert(!std::is_convertible_v<Tracked *, AutoPtr<Tracked>>);
static_assert(!std::is_convertible_v<Tracked *, SharedPtr<Tracked>>);
static_assert(!std::is_copy_constructible_v<UniquePtr<Tracked>>);
static_assert(std::is_nothrow_move_constructible_v<UniquePtr<Tracked>>);
static_assert(std::is_nothrow_move_constructible_v<SharedPtr<Tracked>>);
static_assert(std::is_nothrow_move_assignable_v<SharedPtr<Tracked>>);

// Assign an object to itself through two references, so the compiler doesn't
// warn about the deliberate self-assignment
template <class T> void assign(T &to, T &from) { to = from; }
template <class T> void moveAssign(T &to, T &from) { to = std::move(from); }

int main() {
  {
    UniquePtr<Tracked> a(new Tracked(1));
    UniquePtr<Tracked> b(std::move(a));
    assert(a.isNull() && b->v == 1);
    UniquePtr<Tracked> c;
    c = std::move(b);
    assert(b.isNull() && (*c).v == 1);
    moveAssign(c, c);
    assert(!c.isNull() && alive == 1);
    c = UniquePtr<Tracked>(new Tracked(2)); // the old object is freed
    assert(alive == 1 && c->v == 2);
  }
  assert(alive == 0);

  {
    // copying an AutoPtr transfers ownership
    AutoPtr<Tracked> a(new Tracked(3));
    AutoPtr<Tracked> b(a);
    assert(a.isNull() && b->v == 3);
    AutoPtr<Tracked> c;
    c = b;
    assert(b.isNull() && c->v == 3);
    assign(c, c);
    assert(!c.isNull() && alive == 1);
  }
  assert(alive == 0);

  {
    SharedPtr<Tracked> a(new Tracked(4));
    assert(a.useCount() == 1);
    {
      SharedPtr<Tracked> b(a);
      SharedPtr<Tracked> c;
      c = b;
      assert(a.useCount() == 3 && c->v == 4);
      assign(c, c);
      assert(a.useCount() == 3);
    }
    assert(a.useCount() == 1);
    SharedPtr<Tracked> m(std::move(a));
    assert(a.isNull() && a.useCount() == 0 && m.useCount() == 1);
    SharedPtr<Tracked> other(new Tracked(5));
    other = m; // Tracked(5) is freed
    assert(alive == 1 && m.useCount() == 2);
    SharedPtr<Tracked> empty;
    assert(empty.isNull() && empty.useCount() == 0);
    empty = std::move(other);
    assert(m.useCount() == 2);
  }
  assert(alive == 0);

  // copies of one SharedPtr created and dropped in 8 threads at once
  for (int round = 0; round < 50; round++) {
    int before = destroyed;
    {
      SharedPtr<Tracked> shared(new Tracked(6));
      std::vector<std::thread> threads;
      for (int t = 0; t < 8; t++)
        threads.emplace_back([shared] {
          for (int i = 0; i < 2000; i++) {
            SharedPtr<Tracked> c(shared);
            SharedPtr<Tracked> d;
            d = c;
          }
        });
      for (auto &t : threads)
        t.join();
      assert(shared.useCount() == 1);
    }
    assert(destroyed == before + 1);
  }
  assert(alive == 0);
  puts("smart pointers: OK");
}

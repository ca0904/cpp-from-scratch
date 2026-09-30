#include <atomic>

template <typename T> class SharedPtr {
  T *ptr;
  std::atomic<int> *refCount; // atomic: copies may live in different threads

  void _cleanup() {
    if (refCount == nullptr)
      return;
    if (--(*refCount) == 0) {
      delete ptr;
      delete refCount;
    }
  }

public:
  explicit SharedPtr(T *ptr = nullptr) : ptr(ptr) {
    if (ptr == nullptr)
      refCount = nullptr;
    else
      refCount = new std::atomic<int>(1);
  }

  ~SharedPtr() { _cleanup(); }

  SharedPtr(const SharedPtr &s) : ptr(s.ptr), refCount(s.refCount) {
    if (refCount)
      ++(*refCount);
  }

  SharedPtr &operator=(const SharedPtr &s) {
    if (this == &s)
      return *this;
    _cleanup();
    ptr = s.ptr;
    refCount = s.refCount;
    if (refCount)
      ++(*refCount);
    return *this;
  }

  SharedPtr(SharedPtr &&s) noexcept : ptr(s.ptr), refCount(s.refCount) {
    s.ptr = nullptr;
    s.refCount = nullptr;
  }

  SharedPtr &operator=(SharedPtr &&s) noexcept {
    if (this == &s)
      return *this;
    _cleanup();
    ptr = s.ptr;
    refCount = s.refCount;
    s.ptr = nullptr;
    s.refCount = nullptr;
    return *this;
  }

  T &operator*() const { return *ptr; }
  T *operator->() const { return ptr; }
  bool isNull() const { return ptr == nullptr; }
  int useCount() const { return refCount ? refCount->load() : 0; }
};
/**
 * Smart Pointers are wrappers around the raw pointers which ensures that the
 * memory is freed when the smart pointer goes out of scope.
 */

template <typename T> class AutoPtr {
  T *ptr;

public:
  explicit AutoPtr(T *ptr = nullptr) : ptr(ptr) {}

  ~AutoPtr() { delete ptr; }

  AutoPtr(AutoPtr &a) : ptr(a.ptr) { a.ptr = nullptr; }

  AutoPtr &operator=(AutoPtr &a) {
    if (this == &a)
      return *this;
    delete ptr;
    ptr = a.ptr;
    a.ptr = nullptr;
    return *this;
  }

  T &operator*() const { return *ptr; }
  T *operator->() const { return ptr; }
  bool isNull() const { return ptr == nullptr; }
};
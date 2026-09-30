template <typename T> class UniquePtr {
  T *ptr;

public:
  explicit UniquePtr(T *ptr = nullptr) : ptr(ptr) {}

  ~UniquePtr() { delete ptr; }

  UniquePtr(const UniquePtr &u) = delete;
  UniquePtr &operator=(const UniquePtr &u) = delete;

  UniquePtr(UniquePtr &&a) noexcept : ptr(a.ptr) { a.ptr = nullptr; }

  UniquePtr &operator=(UniquePtr &&u) noexcept {
    if (this == &u)
      return *this;
    delete ptr;
    ptr = u.ptr;
    u.ptr = nullptr;
    return *this;
  }

  T &operator*() const { return *ptr; }
  T *operator->() const { return ptr; }
  bool isNull() const { return ptr == nullptr; }
};
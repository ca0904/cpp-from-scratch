#include <algorithm>
#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <utility>

template <typename T> class Vector {
  std::size_t length_ = 0;
  std::size_t capacity_ = 0;
  T *arr = nullptr;

public:
  Vector(std::size_t length = 0, T value = T{})
      : length_(length), capacity_(length) {
    if (length_ > 0) {
      arr = new T[capacity_];
      std::fill(arr, arr + length_, value);
    }
  }

  ~Vector() { delete[] arr; }

  Vector(std::initializer_list<T> l) : Vector(l.size()) {
    std::copy(l.begin(), l.end(), arr);
  }

  Vector(const Vector &v) : length_(v.length_), capacity_(v.capacity_) {
    arr = new T[capacity_];
    std::copy(v.begin(), v.end(), arr);
  }

  Vector &operator=(const Vector &v) {
    if (this == &v)
      return *this;
    clear();
    length_ = v.length_;
    capacity_ = v.capacity_;
    arr = new T[capacity_];
    std::copy(v.begin(), v.end(), arr);
    return *this;
  }

  Vector(Vector &&v) noexcept
      : length_(v.length_), capacity_(v.capacity_), arr(v.arr) {
    v.length_ = 0;
    v.capacity_ = 0;
    v.arr = nullptr;
  }

  Vector &operator=(Vector &&v) noexcept {
    if (this == &v)
      return *this;
    delete[] arr;
    arr = v.arr;
    length_ = v.length_;
    capacity_ = v.capacity_;
    v.arr = nullptr;
    v.length_ = 0;
    v.capacity_ = 0;
    return *this;
  }

  bool operator==(const Vector &v) const {
    if (length_ != v.length_)
      return false;
    return std::equal(arr, arr + length_, v.arr);
  }

  bool operator!=(const Vector &v) const { return !(*this == v); }

  T &operator[](std::size_t index) {
    assert(index < length_);
    return arr[index];
  }

  T &at(std::size_t index) {
    assert(index < length_);
    return arr[index];
  }

  T &front() {
    assert(length_ > 0);
    return arr[0];
  }

  T &back() {
    assert(length_ > 0);
    return arr[length_ - 1];
  }

  bool empty() const { return length_ == 0; }
  std::size_t size() const { return length_; }
  std::size_t capacity() const { return capacity_; }

  T *begin() { return arr; }
  T *end() { return arr + length_; }
  const T *begin() const { return arr; }
  const T *end() const { return arr + length_; }

  void push_back(const T &value) { push_back(T(value)); }

  void push_back(T &&value) {
    // value may be an element of arr, which growing frees: take it out first
    T item = std::move(value);
    if (length_ == capacity_) {
      capacity_ = (capacity_ == 0) ? 1 : capacity_ * 2;
      T *temp = new T[capacity_]{};
      std::move(arr, arr + length_, temp);
      delete[] arr;
      arr = temp;
    }
    arr[length_++] = std::move(item);
  }

  template <typename... Args> void emplace_back(Args &&...args) {
    push_back(T(std::forward<Args>(args)...));
  }

  void pop_back() {
    assert(length_ > 0);
    --length_;
  }

  void clear() {
    delete[] arr;
    arr = nullptr;
    length_ = 0;
    capacity_ = 0;
  }

  void resize(std::size_t length, T value = T{}) {
    if (length <= length_) {
      length_ = length;
      return;
    }
    if (length > capacity_) {
      capacity_ =
          std::max(length, capacity_ == 0 ? std::size_t{1} : capacity_ * 2);
      T *temp{new T[capacity_]{}};
      std::move(arr, arr + length_, temp);
      delete[] arr;
      arr = temp;
    }
    std::fill(arr + length_, arr + length, value);
    length_ = length;
  }

  void assign(std::size_t length, T value) {
    clear();
    if (length == 0)
      return;
    length_ = capacity_ = length;
    arr = new T[capacity_]{};
    std::fill(arr, arr + length, value);
  }

  void reserve(std::size_t capacity) {
    if (capacity <= capacity_)
      return;
    capacity_ = capacity;
    T *temp{new T[capacity_]{}};
    std::move(arr, arr + length_, temp);
    delete[] arr;
    arr = temp;
  }

  void swap(Vector &v) noexcept {
    std::swap(arr, v.arr);
    std::swap(length_, v.length_);
    std::swap(capacity_, v.capacity_);
  }
};
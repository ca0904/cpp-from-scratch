#include <algorithm>
#include <cstddef>
#include <functional>
#include <list>
#include <vector>

class HashTable {
private:
  std::size_t size_;
  float max_load_factor_;
  std::hash<int> hasher_;
  std::vector<std::list<int>> buckets_;

  std::size_t bucket_index(int key) const {
    return static_cast<std::size_t>(hasher_(key)) % buckets_.size();
  }

  float load_factor() const {
    return static_cast<float>(size_) / buckets_.size();
  }

  void rehash() {
    std::size_t new_capacity = buckets_.size() * 2;
    std::vector<std::list<int>> new_buckets(new_capacity);
    for (const auto &bucket : buckets_) {
      for (const int &key : bucket) {
        std::size_t new_index = hasher_(key) % new_capacity;
        new_buckets[new_index].push_back(key);
      }
    }
    buckets_ = std::move(new_buckets);
  }

public:
  HashTable(std::size_t initial_capacity = 16, float max_load_factor = 0.75f)
      : size_(0), max_load_factor_(max_load_factor),
        buckets_(std::max<std::size_t>(initial_capacity, 1)) {}

  bool contains(int key) {
    if (buckets_.empty())
      return false;
    std::size_t index = bucket_index(key);
    for (const int &k : buckets_[index])
      if (k == key)
        return true;
    return false;
  }

  void insert(int key) {
    if (contains(key))
      return;
    std::size_t index = bucket_index(key);
    buckets_[index].push_back(key);
    ++size_;
    if (load_factor() > max_load_factor_)
      rehash();
  }

  void erase(int key) {
    if (!contains(key))
      return;
    std::size_t index = bucket_index(key);
    buckets_[index].remove(key);
    --size_;
  }
};
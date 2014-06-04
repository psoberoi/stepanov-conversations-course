#include "concepts.h"

#include <cstddef>
#include <algorithm>

// T1 and T2 must be:
//   copyable
//   assignable
//   default constructable
// i.e. Semiregular

// why not reference_type?

template <Semiregular T, size_t k>
struct array {
  typedef T value_type;
  typedef size_t size_type;
  typedef T& reference;
  typedef const T& const_reference;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef pointer iterator;
  typedef const_pointer const_iterator;

  T data[k];

  reference operator[](size_type i) { return *(begin() + i); }
  const_reference operator[](size_type i) const { return *(begin() + i); }

  size_type size() const { return k; }
  size_type max_size() const { return size(); }
  bool empty() const { return !size(); }
  void swap(array& x) { std::swap(data, x.data); }

  iterator begin() { return data; }
  const_iterator begin() const { return data; }

  friend
  iterator begin(array& x) { return x.begin(); }
  friend
  const_iterator begin(const array& x) { return x.begin(); }

  iterator end() { return begin() + size(); }
  const_iterator end() const { return begin() + size(); }

  friend
  iterator end(array& x) { return x.end(); }
  friend
  const_iterator end(const array& x) { return x.end(); }
};

template <Regular T, size_t k>
bool operator==(const array<T, k>& x,
                const array<T, k>& y) {
  return std::equal(begin(x), end(x), begin(y));
}

template <Regular T, size_t k>
bool operator!=(const array<T, k>& x,
                const array<T, k>& y) {
  return !(x == y);
}

template <TotallyOrdered T, size_t k>
bool operator<(const array<T, k>& x,
               const array<T, k>& y) {
  return std::lexicographical_compare(begin(x), end(x),
                                      begin(y), end(y));
}

template <TotallyOrdered T, size_t k>
bool operator>(const array<T, k>& x,
               const array<T, k>& y) {
  return y < x;
}

template <TotallyOrdered T, size_t k>
bool operator<=(const array<T, k>& x,
                const array<T, k>& y) {
  return !(y < x);
}

template <TotallyOrdered T, size_t k>
bool operator>=(const array<T, k>& x,
                const array<T, k>& y) {
  return !(x < y);
}


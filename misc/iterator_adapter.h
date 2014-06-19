#include "concepts.h"

#include <iterator>

// Concept IteratorBasis
// - semiregular
// - state_type, value_type, reference, pointer, iterator_category
// - state_type is regular
// - deref() -> reference
// - increment() -> void
// - state() -> state_type
// For bidirectional iterators:
// - decrement()
// For random access iterators:
// - difference_type
// - offset(difference_type)
// - difference(y) -> difference_type

template <IteratorBasis B>
struct iterator {
  typedef typename B::state_type state_type;
  typedef typename B::value_type value_type;
  typedef typename B::difference_type difference_type;
  typedef typename B::reference reference;
  typedef typename B::pointer pointer;
  typedef typename B::iterator_category iterator_category;

  B basis;

  iterator() {}
  iterator(state_type x) : basis{x} {}

  template <typename T>
  iterator(const T& x) : basis{x} {}

  state_type state() const { return basis.state(); }

  reference operator*() const { return basis.deref(); }

  pointer operator->() const { return &(**this); }

  iterator& operator++() {
    basis.increment();
    return *this;
  }

  iterator operator++(int) {
    iterator tmp = *this;
    ++*this;
    return tmp;
  }

  friend
  bool operator==(const iterator& x, const iterator& y) {
    return x.state() == y.state();
  }

  friend
  bool operator!=(const iterator& x, const iterator& y) {
    return !(x == y);
  }

  // for bidirectional iterator

  iterator& operator--() {
    basis.decrement();
    return *this;
  }

  iterator operator--(int) {
    iterator tmp = *this;
    --*this;
    return tmp;
  }

  // for random access iterator

  iterator& operator+=(difference_type i) {
    basis.offset(i);
    return *this;
  }

  iterator& operator-=(difference_type i) {
    *this += -i;
    return *this;
  }

  reference operator[](difference_type i) const {
    return *(*this + i);
  }

  friend
  difference_type operator-(const iterator& x, const iterator& y) {
    return x.basis.difference(y.basis);
  }

  iterator operator-(difference_type i) {
    return *this  + (-i);
  }

  friend
  iterator operator+(iterator x, difference_type i) {
    return x += i;
  }

  friend
  iterator operator+(difference_type i, iterator x) {
    return x += i;
  }

  friend
  bool operator<(const iterator& x, const iterator& y) {
    return x - y < 0;
  }

  friend
  bool operator>(const iterator& x, const iterator& y) {
    return y < x;
  }

  friend
  bool operator<=(const iterator& x, const iterator& y) {
    return !(y < x);
  }

  friend
  bool operator>=(const iterator& x, const iterator& y) {
    return !(x < y);
  }
};

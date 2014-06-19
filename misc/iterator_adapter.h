#include "concepts.h"

#include <iterator>

// Concept IteratorBasis
// - semiregular
// - state_type, value_type, reference, pointer, iterator_category
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

  // add a state() member function to iterator?
  friend
  bool operator==(const iterator& x, const iterator& y) {
    return x.basis.state() == y.basis.state();
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

  // random access iterator

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

  // should this be a friend function?
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
    return y - x > 0;
    //return x - y < 0;  what's more natural?
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

#include "array.h"
#include "pair.h"
#include "concepts.h"
#include "iterator_adapter.h"

#include <iostream>
#include <numeric>
#include <iterator>
#include <array>

template <InputIterator I>
// if x is of type I, then x->second is well formed
struct second_iterator_basis {
  I position;
  typedef decltype(position->second) value_type;
  typedef value_type& reference;
  typedef value_type* pointer;
  typedef DifferenceType<I> difference_type;
  typedef IteratorCategory<I> iterator_category;
  typedef I state_type;

  reference deref() const { return position->second; }
  void increment() { ++position; }
  // for bidirectional iterator
  void decrement() { --position; }
  // for random access iterator
  void offset(difference_type i) { position += i; }
  difference_type difference(second_iterator_basis y) const {
    return position - y.position;
  }
  state_type state() const { return position; }
};

template <RandomAccessIterator I>
using second_iterator_t = iterator<second_iterator_basis<I>>;

template <BidirectionalIterator I>
struct reverse_iterator_basis {
  I position;
  typedef ValueType<I> value_type;
  typedef Reference<I> reference;
  typedef Pointer<I> pointer;
  typedef DifferenceType<I> difference_type;
  typedef IteratorCategory<I> iterator_category;
  typedef I state_type;

  reference deref() const {
    I tmp = position;
    --tmp;
    return *tmp;
  }
  void increment() { --position; }
  void decrement() { ++position; }
  // for random access iterator
  void offset(difference_type i) { position -= i; }
  difference_type difference(reverse_iterator_basis y) const {
    return y.position - position;
  }
  state_type state() const { return position; }
};

template <RandomAccessIterator I>
using reverse_iterator_t = iterator<reverse_iterator_basis<I>>;

template <InputIterator I>
void print_range(I f, I l) {
  while (f != l) std::cout << *f++ << " ";
  std::cout << std::endl;
}

int main() {
  std::array<int, 10> x1;
  std::iota(begin(x1), end(x1), 0);
  std::random_shuffle(begin(x1), end(x1));
  print_range(begin(x1), end(x1));
  print_range(reverse_iterator_t<int*>(end(x1)),
              reverse_iterator_t<int*>(begin(x1)));
  std::sort(reverse_iterator_t<int*>(end(x1)),
            reverse_iterator_t<int*>(begin(x1)));
  print_range(begin(x1), end(x1));


  typedef pair<int, int> P;
  array<P, 3> x2 { {{ 1, 1}, {2, 4}, {3, 9}} };
  print_range(second_iterator_t<P*>(begin(x2)),
              second_iterator_t<P*>(end(x2)));
  for (const auto& v: x2) std::cout << v.second << " ";
  std::cout << std::endl;
}

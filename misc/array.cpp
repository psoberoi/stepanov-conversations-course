#include "array.h"
#include "pair.h"
#include "concepts.h"
#include "iter.h"

#include <iostream>
#include <numeric>
#include <iterator>

template <RandomAccessIterator I>
// if x is of type I, then x->second is well formed
struct second_iterator_basis {
  I position;
  typedef decltype(position->second) value_type;
  typedef value_type& reference;
  typedef value_type* pointer;
  typedef DifferenceType<I> difference_type;
  typedef std::random_access_iterator_tag iterator_category;
  typedef I state_type;

  reference deref() const { return position->second; }
  void increment() { ++position; }
  void decrement() { --position; }
  void offset(difference_type i) { position += i; }
  difference_type difference(second_iterator_basis y) const {
    return position - y.position;
  }
  state_type state() const { return position; }
};

template <ForwardIterator I>
using IterSecond = iterator<second_iterator_basis<I>>;

template <InputIterator I>
void print_range(I f, I l) {
  while (f != l) std::cout << *f++ << " ";
  std::cout << std::endl;
}

int main() {
  array<int, 10> x1;
  std::iota(begin(x1), end(x1), 0);
  print_range(begin(x1), end(x1));

  typedef pair<int, int> P;
  array<P, 3> x2 { {{ 1, 1}, {2, 4}, {3, 9}} };
  //print_range(begin(x2), end(x2));
  print_range(IterSecond<P*>(begin(x2)), IterSecond<P*>(end(x2)));
  for (const auto& v: x2) std::cout << v.second << " ";
  std::cout << std::endl;
}

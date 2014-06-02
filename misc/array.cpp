#include "array.h"
#include "pair.h"

#include <iostream>
#include <numeric>

template <InputIterator I>
void print_range(I f, I l) {
  while (f != l) std::cout << *f++ << " ";
  std::cout << std::endl;
}

int main() {
  array<int, 10> x1;
  std::iota(begin(x1), end(x1), 0);
  print_range(begin(x1), end(x1));
 
  array<pair<int, int>, 3> x2 { {{ 1, 1}, {2, 4}, {3, 9}} };
  //print_range(begin(x2), end(x2));
  for (const auto& v: x2) std::cout << v.second << " ";
  std::cout << std::endl;
}




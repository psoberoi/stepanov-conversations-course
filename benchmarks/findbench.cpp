#include <vector>
#include <set>
#include <unordered_set>
#include "boost/container/flat_set.hpp"

#include <chrono>
#include <numeric>
#include <iostream>
#include <iomanip>

#define Sequence typename
#define Container typename
#define TotallyOrdered typename
#define ForwardIterator typename
#define RandomAccessIterator typename

template <typename I> // I is InputIterator
using ValueType = typename std::iterator_traits<I>::value_type;

template <typename I> // I is InputIterator
using DifferenceType = typename std::iterator_traits<I>::difference_type;

class timer {
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
public:
    void start() {
        start_time = std::chrono::high_resolution_clock::now();
    }

    double stop() {
        auto stop_time = std::chrono::high_resolution_clock::now();
        return double(std::chrono::duration_cast<std::chrono::nanoseconds>(stop_time - start_time).count());
    }
};

template <RandomAccessIterator I>
void random_iota(I first, I last) {
  std::iota(first, last, 0);
  std::random_shuffle(first, last);
}

template <typename T>
void print_cell(const T& x, int precision = 0) {
  std::cout << "\t" << std::setw(6)
            << std::fixed << std::setprecision(precision)
            << x << std::flush;
}

template <Container C, ForwardIterator I>
double time_lookups(const C& c, I first, I last, size_t iterations) {
  timer t;
  t.start();
  while (iterations--) {
    I current = first;
    while (current != last) {
      if (c.find(*current) == c.end()) std::cout << "Element missing!" << std::endl;
      ++current;
    }
  }
  return t.stop();
}

template <Container C, ForwardIterator I>
// ValueType<C> == ValueType<S>
void time_lookup_container(I first, I last, size_t iterations, size_t max_size) {
  C c(first, last);
  double t = time_lookups(c, first, last, iterations);
  print_cell(t / max_size);
}

template <typename T, ForwardIterator I>
// ValueType<C> == ValueType<S>
void time_lookup_container_flat_set(I first, I last, size_t iterations, size_t max_size) {
  std::vector<T> tmp(first, last);
  std::sort(begin(tmp), end(tmp));
  boost::container::flat_set<T> c(boost::container::ordered_unique_range_t(), begin(tmp), end(tmp));
  double t = time_lookups(c, first, last, iterations);
  print_cell(t / max_size);
}

std::string type_description(double) { return std::string("double"); }

template <TotallyOrdered T>
void run_test(size_t min_size, size_t max_size) {
  time_t now = time(0);
  std::cout << "Iterating over " << type_description(T(0))
            << " from " << min_size << " up to " << max_size
            << " elements at: " << asctime(localtime(&now));

  std::vector<const char*> names {
    " bsrch",
    "   set",
    "  hash",
  };
  std::cout << std::setw(12) << "size";
  for (auto x: names) print_cell(x);
  std::cout << std::endl;

  std::vector<T> vec(max_size);
  random_iota(begin(vec), end(vec));
  for (size_t array_size(min_size); array_size <= max_size; array_size *= 2) {
    const size_t n = (max_size / array_size);
    auto last = begin(vec) + array_size;

    std::cout << std::setw(12) << array_size;

    time_lookup_container_flat_set<T>(begin(vec), last, n, max_size);
    time_lookup_container<std::set<T>>(begin(vec), last, n, max_size);
    time_lookup_container<std::unordered_set<T>>(begin(vec), last, n, max_size);
    std::cout << std::endl;
  }
}

const size_t min_size(8);
const size_t max_size(2 * 1024 * 1024);

int main() {
  typedef double element_type;
  run_test<element_type>(min_size, max_size);
}

#include <chrono>
#include <iterator>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <vector>
#include <forward_list>
#include <list>
#include <set>

#define TotallyOrdered typename
#define RandomAccessIterator typename

template <typename I> // I is InputIterator
using ValueType = typename std::iterator_traits<I>::value_type;

template <typename I> // I is InputIterator
using DifferenceType = typename std::iterator_traits<I>::difference_type;

template <typename I> // I is InputIterator
using IteratorCategory = typename std::iterator_traits<I>::iterator_category;

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

template <TotallyOrdered T>
struct sort_algorithm {
  virtual const char* name() = 0;
  virtual void sort(T*, T*) = 0;
};

template <TotallyOrdered T>
struct std_sort : public sort_algorithm<T> {
  virtual const char* name() { return "quick"; }
  virtual void sort(T* f, T* l) { std::sort(f, l); }
};

template <TotallyOrdered T>
struct std_stable_sort : public sort_algorithm<T> {
  virtual const char* name() { return "merge"; }
  virtual void sort(T* f, T* l) { std::stable_sort(f, l); }
};

template <TotallyOrdered T>
struct std_heap_sort : public sort_algorithm<T> {
  virtual const char* name() { return "heap"; }
  virtual void sort(T* f, T* l) { std::partial_sort(f, l, l); }
};

template <TotallyOrdered N>
int three_way_cmp(const void* x, const void* y) {
  const N* a = static_cast<const N*>(x);
  const N* b = static_cast<const N*>(y);
  if (*b < *a) return 1;
  if (*a < *b) return -1;
  return 0;
}

template <TotallyOrdered T>
struct std_qsort : public sort_algorithm<T> {
  virtual const char* name() { return "qsort"; }
  virtual void sort(T* f, T* l) {
    std::qsort((void*)(f), size_t(l - f), sizeof(T),
               three_way_cmp<T>);
  }
};

template <TotallyOrdered T>
struct flist_sort : public sort_algorithm<T> {
  virtual const char* name() { return "flist"; }
  virtual void sort(T* f, T* l) {
    std::forward_list<T> x(f, l);
    x.sort();
    std::copy(begin(x), end(x), f);
  }
};

template <TotallyOrdered T>
struct list_sort : public sort_algorithm<T> {
  virtual const char* name() { return "list"; }
  virtual void sort(T* f, T* l) {
    std::list<T> x(f, l);
    x.sort();
    std::copy(begin(x), end(x), f);
  }
};

template <TotallyOrdered T>
struct set_sort : public sort_algorithm<T> {
  virtual const char* name() { return "set"; }
  virtual void sort(T* f, T* l) {
    std::multiset<T> x(f, l);
    std::copy(begin(x), end(x), f);
  }
};

template <TotallyOrdered T>
double time_sort(T* first, T* last, sort_algorithm<T>* a, T* buffer, size_t size) {
  timer t;
  t.start();
  while (size <= last - first) {
    std::copy(first, first + size, buffer);
    first += size;
    a->sort(buffer, buffer + size);
    if (!std::is_sorted(buffer, buffer + size)) {
      std::cerr << "*** SORT FAILED! ***" << std::endl;
      return 0;
    }
  }
  return t.stop();
}

std::string type_description(double) { return std::string("double"); }

template <TotallyOrdered T>
void run_test(size_t min_size, size_t max_size) {
  time_t now = time(0);
  std::cout << "Sorting " << type_description(T(0))
            << " from " << min_size << " up to " << max_size
            << " elements at: " << asctime(localtime(&now));

  std_qsort<T> alg1;
  std_sort<T> alg2;
  std_stable_sort<T> alg3;
  std_heap_sort<T> alg4;
  flist_sort<T> alg5;
  list_sort<T> alg6;
  set_sort<T> alg7;
  sort_algorithm<T>* alg[] =
    { &alg1, &alg2, &alg3, &alg4, &alg5, &alg6, &alg7 };

  std::cout << std::setw(12) << "size";
  for (auto a: alg) print_cell(a->name());
  std::cout << std::endl;

  std::vector<T> vec(max_size);
  std::vector<T> tmp(max_size);
  T* vec_begin = &(*begin(vec));
  T* vec_end = &(*end(vec));
  T* tmp_begin = &(*begin(tmp));
  random_iota(begin(vec), end(vec));
  for (size_t array_size(min_size); array_size <= max_size; array_size *= 2) {
    std::cout << std::setw(12) << array_size;
    for (auto a: alg) {
      double t = time_sort(vec_begin, vec_end, a,
                           tmp_begin, array_size);
      print_cell(t / max_size);
    }
    std::cout << std::endl;
  }
}

const size_t min_size(8);
const size_t max_size(16 * 1024 * 1024);

int main() {
  typedef double element_type;
  run_test<element_type>(min_size, max_size);
}

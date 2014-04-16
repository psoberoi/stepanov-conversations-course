#include <vector>
#include <list>
#include <forward_list>

#include <chrono>
#include <numeric>
#include <iostream>
#include <iomanip>
#include <algorithm>

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

template <template<typename T, typename Allocator> class Container>
// Container is a templatized sequence
struct time_unshuffled {
  template <ForwardIterator I>
  double operator()(I first, I last, size_t iterations) {
    Container<ValueType<I>, std::allocator<ValueType<I>>> test_container(first, last);
    timer t;
    t.start();
    while (iterations--) {
      std::copy(first, last, begin(test_container));
    }
    return t.stop();
  }
};

template <template<typename T, typename Allocator> class Container>
// Container is a templatized sequence
struct time_shuffled {
  template <ForwardIterator I>
  double operator()(I first, I last, size_t iterations) {
    Container<ValueType<I>, std::allocator<ValueType<I>>> test_container(first, last);
    test_container.sort();
    timer t;
    t.start();
    while (iterations--) {
      std::copy(first, last, begin(test_container));
    }
    return t.stop();
  }
};

template <template<typename T, typename Allocator> class Container>
// Container is a templatized sequence
struct time_copy_container {
  template <ForwardIterator I>
  void operator()(I first, I last, size_t iterations, size_t num_elements) {
    double t0 = time_unshuffled<Container>()(first, last, iterations);
    double t1 = time_shuffled<Container>()(first, last, iterations);
    print_cell(t0 / num_elements);
    print_cell(t1 / num_elements);
    print_cell(t1 / t0, 1);
  }
};

std::string type_description(double) { return std::string("double"); }

template <TotallyOrdered T>
void run_test(size_t min_size, size_t max_size) {
  time_t now = time(0);
  std::cout << "Iterating over " << type_description(T(0))
            << " from " << min_size << " up to " << max_size
            << " elements at: " << asctime(localtime(&now));

  std::vector<const char*> names {
    "   vec",
    "  lseq",
    "  lshf",
    " ratio",
    "  fseq",
    "  fshf",
    " ratio"
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

    auto base_time = time_unshuffled<std::vector>()(begin(vec), last, n);
    print_cell(base_time / max_size, 2);
    time_copy_container<std::list>()(begin(vec), last, n, max_size);
    time_copy_container<std::forward_list>()(begin(vec), last, n, max_size);
    std::cout << std::endl;
  }
}

const size_t min_size(8);
const size_t max_size(16 * 1024 * 1024);

int main() {
  typedef double element_type;
  run_test<element_type>(min_size, max_size);
}

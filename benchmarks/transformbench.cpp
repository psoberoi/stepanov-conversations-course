#include <stdint.h>
#include <cstddef>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <numeric>
#include <cmath>
#include <list>
#include <vector>
#include <deque>

#define TotallyOrdered typename
#define Sequence typename
#define Number typename
#define BinaryOperation typename
#define RandomAccessIterator typename

#if __cplusplus <= 199711L

#include <time.h>

class timer {
private:
    clock_t start_time;
public:
    typedef double result_type;

    void start() {
        start_time = clock();
    }

    result_type stop() {
        return 1000000000. * ((clock() - start_time) / double(CLOCKS_PER_SEC));
    }
};

template<class ForwardIterator, class T>
void iota(ForwardIterator first, ForwardIterator last, T value)
{
    while(first != last) {
        *first++ = value;
        ++value;
    }
}

#else

#include <chrono>

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

using std::iota;

#endif

template <RandomAccessIterator I>
void random_iota(I first, I last) {
  iota(first, last, 0);
  std::random_shuffle(first, last);
}

template <typename N>
struct plus
{
  N operator() (const N& x, const N& y) { return  x + y; }
};

template <typename N>
struct multiplies
{
  N operator() (const N& x, const N& y) { return x * y; }
};

template <typename N>
struct divides
{
  N operator() (const N& x, const N& y) {
    if (y == N(0)) return x;
    return x / y;
  }
};

template <typename N>
struct fplus
{
  N operator() (const N& x, const N& y) __attribute__((noinline));
};

template <typename N>
N fplus<N>::operator() (const N& x, const N& y) {
  return x + y;
}

template <TotallyOrdered T>
struct minimum
{
  T operator()(const T& x, const T& y) {
    if (y < x) return y;
    return x;
  }
};

template <TotallyOrdered T>
struct norm
{
  T operator()(const T& x, const T& y) {
    return x * x + y * y;
  }
};

template <Number T>
struct conversion 
{
  T operator()(const T& x, const T& y) { return T(double(x) + double(y)); }
};

template <Number N>
struct square_root
{
  N operator()(N x, N y) {
    return N(std::sqrt(std::abs(x + y))); 
  }
};

template <Number N>
struct cube_root_pow
{
  N operator()(N x, N y) {
    return N(std::pow(std::abs(x + y), 1.0/3.0)); 
  }
};

template <Sequence S1, Sequence S2, Sequence S3, BinaryOperation Op>
void time_test(const S1& s1, const S2& s2, S3& s3, Op op, size_t n, size_t m) {
  typedef typename S1::iterator I;
  timer t;
  t.start();
  for (int i = 0; i < m; ++i)
    std::transform(s1.begin(), s1.end(), s2.begin(), s3.begin(), op);
  double time = t.stop()/(double(n) * m);
  std::cout << std::setw(6) << std::fixed << std::setprecision(2) << time << "\t";
}

std::string type_description(double) { return std::string("double"); }
std::string type_description(long double) { return std::string("long double"); }
std::string type_description(float) { return std::string("float"); }
std::string type_description(uint8_t) { return std::string("uint8_t"); }
std::string type_description(uint16_t) { return std::string("uint16_t"); }
std::string type_description(uint32_t) { return std::string("uint32_t"); }
std::string type_description(uint64_t) { return std::string("uint64_t"); }
std::string type_description(int8_t) { return std::string("int8_t"); }
std::string type_description(int16_t) { return std::string("int16_t"); }
std::string type_description(int32_t) { return std::string("int32_t"); }
std::string type_description(int64_t) { return std::string("int64_t"); }

template <typename T, typename Alloc>
std::string type_description(const std::vector<T, Alloc>&) { return std::string("vector<") + type_description(T()) + std::string(">"); }

template <typename T, typename Alloc>
std::string type_description(const std::deque<T, Alloc>&) { return std::string("deque<") + type_description(T()) + std::string(">"); }

template <typename T, typename Alloc>
std::string type_description(const std::list<T, Alloc>&) { return std::string("list<") + type_description(T()) + std::string(">"); }

void print_header(size_t n, size_t m) {
  time_t now = time(0);

  std::cout << "Applying operation " 
	    << n * m << " times" 
	    << " (sequence length " << n << " by " << m << " iterations)"
	    << " at: " << asctime(localtime(&now))
	    << std::setw(15) << "  type\t"
	    << "  plus\t"
	    << " fplus\t"
	    << " times\t" 
	    << "  norm\t" 
	    << " cnvrt\t" 
	    << "   div\t" 
	    << "  sqrt\t"
	    << "   pow\t"
	    << std::endl;
}

template <Number T, template <Number N, class Alloc = std::allocator<N> > class Seq>
struct test_instructions 
{
  void operator()(size_t n, size_t m) {
    std::vector<T> data(n);
    random_iota(data.begin(), data.end());
    Seq<T> v1(data.begin(), data.end());
    std::random_shuffle(data.begin(), data.end());
    Seq<T> v2(data.begin(), data.end());
    Seq<T> v3(n);
    std::cout << std::setw(15) << type_description(v1) << "\t";
    time_test(v1, v2, v3, plus<T>(), n, m);
    time_test(v1, v2, v3, fplus<T>(), n, m);
    time_test(v1, v2, v3, multiplies<T>(), n, m);
    time_test(v1, v2, v3, norm<T>(), n, m);
    time_test(v1, v2, v3, conversion<T>(), n, m);
    time_test(v1, v2, v3, divides<T>(), n, m);
    time_test(v1, v2, v3, square_root<T>(), n, m);
    time_test(v1, v2, v3, cube_root_pow<T>(), n, m);
    std::cout << std::endl;
  }

};

template <template <Number N, class Alloc = std::allocator<N> > class Seq>
struct test_all_types
{
  void operator() (size_t n, size_t m) {
    test_instructions<int8_t, Seq>()(n, m); 
    test_instructions<int16_t, Seq>()(n, m); 
    test_instructions<int32_t, Seq>()(n, m); 
    test_instructions<int64_t, Seq>()(n, m); 
    test_instructions<float, Seq>()(n, m); 
    test_instructions<double, Seq>()(n, m);
  }
}; 

int main(int argc, char* argv[]) {
  uint64_t n = 1000; 
  if (argc == 2) n = atoi(argv[1]);
  uint64_t m(100000000 / n);
  print_header(n, m);
  test_all_types<std::vector>()(n, m);
  test_all_types<std::deque>()(n, m);
  test_all_types<std::list>()(n, m);
}

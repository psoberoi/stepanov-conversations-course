

#include <algorithm>

template <typename T>
// requires TotallyOrdered<T>
struct min_max_semiring {
  typedef T element_type;
  struct plus_type {
    T operator()(const T x,
		 const T y) {
      return std::max(x, y);
    }
  } plus;
  struct times_type {
    T operator()(const T x,
		 const T y) {
      return std::min(x, y);
    }
  } times;
};

#define assert(x)

template <typename I1, typename I2, typename Integer, typename S>
typename S::element_type
// requires InputIterator<I1> && InputIterator<I2> &&
//          Semiring<S> &&
//          ValueType<I1> == ValueType<I2> &&
//          ValueType<I1> == ElementType<S> 
inner_product(I1 first1, I2 first2, Integer n, S semiring) {
  
  assert(n != 0);
  assert(valid_range(first1, n) && valid_range(first2, n));

  typename S::element_type result = semiring.times(*first1++, *first2++);
  // we do not require the additive identity and start with the first product

  while (--n != 0) {
    result = semiring.plus(result, semiring.times(*first1++, *first2++));
  }
  return result;
}




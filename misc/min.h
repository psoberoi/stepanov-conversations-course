// given objects a and b of the same type
// and some strict weak ordering cmp,
// returns a if cmp(a, b),
//         b if cmp(b, a),
//         a otherwise.
//
// returns b if cmp(b, a), a otherwise.

#include "concepts.h"

template <typename T, StrictWeakOrdering Cmp>
T& min(T& a, T& b, Cmp cmp) {
  if (cmp(b, a)) return b;
  return a;
}

template <typename T, StrictWeakOrdering Cmp>
const T& min(const T& a, const T& b, Cmp cmp) {
  if (cmp(b, a)) return b;
  return a;
}

// min(x, y) += 10000
// min(3, x)

template <typename T, StrictWeakOrdering Cmp>
T& max(T& a, T& b, Cmp cmp) {
  if (cmp(b, a)) return a;
  return b;
}

template <typename T, StrictWeakOrdering Cmp>
void sort(T& a, T& b, Cmp cmp) {
  if (!cmp(a, b)) std::swap(a, b);
}

// min, swap, linear search (aka find_if)




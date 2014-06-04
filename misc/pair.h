#include "concepts.h"

// T1 and T2 must be:
//   copyable
//   assignable
//   default constructable
// i.e. Semiregular

template <Semiregular T1, Semiregular T2>
struct pair {
  T1 first;
  T2 second;
};

template <Regular T1, Regular T2>
bool operator==(const pair<T1, T2>& x,
                const pair<T1, T2>& y) {
  return (x.first == y.first) && (x.second == y.second);
}

template <Regular T1, Regular T2>
bool operator!=(const pair<T1, T2>& x,
                const pair<T1, T2>& y) {
  return !(x == y);
}

template <TotallyOrdered T1, TotallyOrdered T2>
bool operator<(const pair<T1, T2>& x,
               const pair<T1, T2>& y) {
  if (x.first < y.first) return true;
  if (y.first < x.first) return false;
  // assert x.first == y.first
  return x.second < y.second;
}

template <TotallyOrdered T1, TotallyOrdered T2>
bool operator>(const pair<T1, T2>& x,
               const pair<T1, T2>& y) {
  return y < x;
}

template <TotallyOrdered T1, TotallyOrdered T2>
bool operator<=(const pair<T1, T2>& x,
                const pair<T1, T2>& y) {
  return !(y < x);
}

template <TotallyOrdered T1, TotallyOrdered T2>
bool operator>=(const pair<T1, T2>& x,
                const pair<T1, T2>& y) {
  return !(x < y);
}


#include <cctype>
#include <cstddef>
#include <iostream>
#include <algorithm>
#include <vector>
#include <functional>

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include "concepts.h"
#include "type_function.h"

using namespace std;
using namespace boost::interprocess;

template <typename I> // I is a ForwardIterator
using bigram = typename std::pair<std::pair<I, I>, std::pair<I, I>>;

template <typename I, typename Cmp>
// I is an InputIterator
// Cmp is a StrictWeakOrdering
int lexicographical_compare_3way(
  I first1, I last1,
  I first2, I last2,
  Cmp comp)
{
  while (true) {
    if (first1 == last1 && first2 == last2) return 0;
    if (first1 == last1) return -1;
    if (first2 == last2) return 1;
    if (comp(*first1, *first2)) return -1;
    if (comp(*first2, *first1)) return 1;
    ++first1;
    ++first2;
  }
}

template <typename Cmp>
struct compare_bigram {
  Cmp comp;
  compare_bigram() {}
  compare_bigram(Cmp comp) : comp(comp) {}
  template <typename I>
  bool operator()(const bigram<I>& x, const bigram<I>& y) {
    switch (lexicographical_compare_3way(
      x.first.first, x.first.second,
      y.first.first, y.first.second,
      comp))
    {
      case -1: return true;
      case  0: return std::lexicographical_compare(
                        x.second.first, x.second.second,
                        y.second.first, y.second.second,
                        comp);
      case  1: return false;
    }
  }
};

template <typename Cmp>
struct compare_count_index {
  compare_bigram<Cmp> cmp;
  compare_count_index() {}
  compare_count_index(Cmp cmp) : cmp(cmp) {}
  template <typename P>
  bool operator()(const P& x, const P& y) {
    if (x.first < y.first) return true;
    if (y.first < x.first) return false;
    return cmp(*x.second, *y.second);
  }
};

template <InputIterator I1, InputIterator I2, EquivalenceRelation R>
bool equal(I1 first1, I1 last1, I2 first2, I2 last2, R r) {
  while (true) {
    if (first1 == last1 && first2 == last2) return true;
    if (first1 == last1 || first2 == last2) return false;
    if (!r(*first1, *first2)) return false;
    ++first1;
    ++first2;
  }
}


template <EquivalenceRelation Equ>
struct equal_bigram {
  Equ eq;
  equal_bigram() {}
  equal_bigram(Equ eq) : eq(eq) {}
  template <typename I>
  bool operator()(const bigram<I>& x, const bigram<I>& y) {
    return equal(x.first.first, x.first.second,
                 y.first.first, y.first.second,
                 eq)
           &&
           equal(x.second.first, x.second.second,
                 y.second.first, y.second.second,
                 eq);
  }
};

template <typename I>
inline
bool valid_bigram(const bigram<I>& x) {
  return (x.second.first != x.second.second);
}

struct is_not_whitespace {
  bool operator()(int c) { return !isspace(c); }
};

template <typename I, typename P>
// I is a ForwardIterator
// P is a UnaryPredicate on ValueType<I>
pair<I, I> find_run(I first, I last, P pred) {
  first = find_if(first, last, pred),
  last = find_if_not(first, last, pred);
  return {first, last};
}

template <typename I, typename P>
// I is a ForwardIterator
// P is a UnaryPredicate on ValueType<I>
bigram<I> find_bigram(I first, I last, P pred) {
  auto first_word = find_run(first, last, pred);
  auto second_word = find_run(first_word.second, last, pred);
  return {first_word, second_word};
}

template <ForwardIterator I, UnaryPredicate P>
vector<bigram<I>> make_bigram_index(I first, I last, P pred) {
  vector<bigram<I>> index;
  while (true) {
    bigram<I> bg = find_bigram(first, last, pred);
    if (!valid_bigram(bg)) return index;
    index.push_back(bg);
    first = bg.second.first;
  }
}

template <ForwardIterator I, BinaryPredicate P>
pair<I, DifferenceType<I>> adjacent_count_if(I first, I last, P pred) {
  DifferenceType<I> n{0};
  if (first == last) return {first, n};
  while (next(first) != last && pred(*first, *next(first))) {
    ++n;
    ++first;
  }
  return {next(first), ++n};
}

template <typename I>
void print_range(I first, I last) {
  while (first != last) {
    cout << *first;
    ++first;
  }
}

template <typename I>
void print_bigram(bigram<I> bg) {
  print_range(bg.first.first, bg.first.second);
  cout << " ";
  print_range(bg.second.first, bg.second.second);
}

int main(int argc, char** argv) {
  if (argc != 2) {
    cerr << "one argument expected" << endl;
    return 1;
  }
  file_mapping f_mapped(argv[1], read_only);
  mapped_region f_region(f_mapped, read_only);
  const char* data = (const char*)(f_region.get_address());

  vector<bigram<const char*>> index = make_bigram_index(
    data, data + f_region.get_size(),
    is_not_whitespace());

  sort(begin(index), end(index), compare_bigram<std::less<char>>());

  typedef vector<bigram<const char*>>::iterator bigram_iterator;
  typedef vector<bigram<const char*>>::size_type size_type;
  vector<pair<size_type, bigram_iterator>> count_index;

  auto idxfirst = begin(index);
  auto idxlast = end(index);
  while (idxfirst != idxlast) {
    auto run = adjacent_count_if(idxfirst, idxlast, equal_bigram<equal_to<char>>());
    count_index.push_back({run.second, idxfirst});
    idxfirst = run.first;
  }

  sort(begin(count_index), end(count_index), compare_count_index<std::less<char>>());

  auto first = begin(count_index);
  auto last = end(count_index);
  int i = 0;
  while (first != last && i < 250) {
    --last;
    print_bigram(*(last->second));
    cout << " " << last->first << endl;
    ++i;
  }
}

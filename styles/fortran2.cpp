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

typedef uint64_t prefix_t;
typedef uint32_t char_idx_t;
typedef const char* pchar_t;

template <UnsignedIntegral N>
uint8_t high_byte(N x) {
  return x >> ((sizeof(N) - 1) * 8);
}

template <typename I>
void print_range(I first, I last) {
  while (first != last) {
    cout << *first;
    ++first;
  }
}

struct word_t {
  char_idx_t first;
  char_idx_t last;
  prefix_t prefix;

  bool is_valid() const { return prefix || first != last; }

  void next(pchar_t base, char_idx_t size) {
    auto current = find_if(base + last, base + size,
                           [](char x){ return !isspace(x); });
    auto offset = sizeof(prefix_t);
    first = current - base;
    prefix = 0;

    while (current != base + size && !isspace(*current)) {
      if (offset) {
        ++first;
        prefix <<= 8;
        prefix |= static_cast<uint8_t>(*current);
        --offset;
      }
      ++current;
    }

    prefix <<= offset * 8;
    last = current - base;
  }

  void print(pchar_t base) const {
    auto p = prefix;
    while (true) {
      char c = high_byte(p);
      if (!c) break;
      cout << c;
      p <<= 8;
    }
    print_range(base + first, base + last);
  }
};

struct bigram_t {
  word_t first_word;
  word_t second_word;

  bigram_t() { second_word.last = 0; }

  bool is_valid() const { return second_word.is_valid(); }

  void next(pchar_t base, char_idx_t size) {
    first_word = second_word;
    second_word.next(base, size);
  }

  void print (pchar_t base) const {
    first_word.print(base);
    cout << " ";
    second_word.print(base);
  }
};

vector<bigram_t> make_bigram_index(pchar_t base, char_idx_t size) {
  vector<bigram_t> vec;
  bigram_t bigram;
  bigram.next(base, size);
  while (true) {
    bigram.next(base, size);
    if (!bigram.is_valid()) return vec;
    vec.push_back(bigram);
  }
}

template <typename I,
          typename Cmp = std::less<ValueType<I>>>
// I is an InputIterator
// Cmp is a StrictWeakOrdering
int lexicographical_compare_3way(
  I first1, I last1,
  I first2, I last2,
  Cmp comp = Cmp())
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

inline
int word_compare_3way(pchar_t base, const word_t& x, const word_t& y) {
  if (x.prefix < y.prefix) return -1;
  if (y.prefix < x.prefix) return 1;
  return lexicographical_compare_3way(base + x.first, base + x.last,
                                      base + y.first, base + y.last);
}

inline
int bigram_compare_3way(pchar_t base, const bigram_t& x, const bigram_t& y) {
  int cmp = word_compare_3way(base, x.first_word, y.first_word);
  if (cmp) return cmp;
  else return word_compare_3way(base, x.second_word, y.second_word);
};

struct bigram_compare {
  pchar_t base;
  bigram_compare(pchar_t base) : base(base) {}
  bool operator()(const bigram_t& x, const bigram_t& y) {
    return bigram_compare_3way(base, x, y) == -1;
  }
};

struct bigram_equal {
  pchar_t base;
  bigram_equal(pchar_t base) : base(base) {}
  bool operator()(const bigram_t& x, const bigram_t& y) {
    return bigram_compare_3way(base, x, y) == 0;
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

template <ForwardIterator I, BinaryPredicate P>
pair<DifferenceType<I>, I> adjacent_count_if(I first, I last, P pred) {
  DifferenceType<I> n{0};
  if (first == last) return {n, first};
  while (next(first) != last && pred(*first, *next(first))) {
    ++n;
    ++first;
  }
  return {++n, next(first)};
}

template <ForwardIterator I, OutputIterator O, BinaryPredicate P>
O unique_copy_with_count(I first, I last, O result, P pred) {
  while (true) {
    auto cnt = adjacent_count_if(first, last, pred);
    if (!cnt.first) return result;
    *result = {cnt.first, *first};
    ++result;
    first = cnt.second;
  }
}

struct bigram_count_compare {
  pchar_t base;
  bigram_compare bgcmp;
  bigram_count_compare(pchar_t base) : bgcmp(base) {}
  template <typename N>
  bool operator()(const pair<N, bigram_t>& x, const pair<N, bigram_t>& y) {
    if (x.first < y.first) return true;
    if (y.first < x.first) return false;
    return bgcmp(x.second, y.second);
  }
};

int main(int argc, char** argv) {
  if (argc != 2) {
    cerr << "one argument expected" << endl;
    return 1;
  }
  file_mapping f_mapped(argv[1], read_only);
  mapped_region f_region(f_mapped, read_only);
  pchar_t data = (pchar_t)(f_region.get_address());

  auto bigram_index = make_bigram_index(data, f_region.get_size());
  sort(begin(bigram_index), end(bigram_index), bigram_compare(data));

  vector<pair<size_t, bigram_t>> bigram_count_index;
  unique_copy_with_count(begin(bigram_index), end(bigram_index),
                         back_inserter(bigram_count_index),
                         bigram_equal(data));

  sort(begin(bigram_count_index), end(bigram_count_index), bigram_count_compare(data));

  auto first = begin(bigram_count_index);
  auto last = end(bigram_count_index);
  int i = 0;
  while (first != last && i < 250) {
    --last;
    last->second.print(data);
    cout << " " << last->first << endl;
    ++i;
  }
}

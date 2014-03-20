#include <map>
#include <set>
#include <string>
#include <memory>
#include <cstddef>
#include <fstream>
#include <iostream>

using namespace std;

struct bigram {
  set<string>::iterator first;
  set<string>::iterator second;
  friend
  bool operator<(const bigram& x, const bigram& y) {
    // first word
    if (*x.first < *y.first) return true;
    if (*y.first < *x.first) return false;
    // second word
    return *x.second < *y.second;
  }
};

struct compare_bigram_count {
  template <typename I>
  bool operator()(I x, I y) {
    // count
    if (x->second < y->second) return true;
    if (y->second < x->second) return false;
    // bigram
    return x->first < y->first;
  }
};

int main(int argc, char** argv) {
  set<string> words;
  map<bigram, size_t> counts;

  if (argc != 2) {
    cerr << "one argument expected" << endl;
    return 1;
  }
  fstream input_file(argv[1], ios_base::in);

  auto prev = end(words);
  string word;
  while (input_file >> word) {
    auto current = words.insert(word).first;
    if (prev != end(words)) {
      ++counts[{prev, current}];
    }
    prev = current;
  }

  set<map<bigram, size_t>::iterator, compare_bigram_count> sorted_bigrams;
  for (auto x = begin(counts); x != end(counts); ++x) {
    sorted_bigrams.insert(x);
  }

  auto first = begin(sorted_bigrams);
  auto last = end(sorted_bigrams);
  int i = 0;
  while (first != last && i < 250) {
    --last;
    cout << *(*last)->first.first << " " << *(*last)->first.second
         << " " << (*last)->second
         << endl;
    ++i;
  }
}

#ifndef QUICKSORT_DOUBLE_H
#define QUICKSORT_DOUBLE_H

#include <cstddef>

inline
void unguarded_linear_insert(int64_t* last, int64_t value) {
  int64_t* previous = last;
  while (value < *--previous) *last-- = *previous;
  *last = value;
}

inline
void unguarded_insertion_sort(int64_t* first, int64_t* last) {
  for (int64_t* i = first; i != last; ++i)
    unguarded_linear_insert(i, *i);
}

inline
void linear_insert(int64_t* first, int64_t* last, int64_t value) {
  if (value < *first) {
    while (first != last--) *(last + 1) = *last;
    *first = value;
  }
  else unguarded_linear_insert(last, value);
}

inline
void insertion_sort(int64_t* first, int64_t* last) {
  if (first == last) return;
  for (int64_t* i = first + 1; i != last; ++i)
    linear_insert(first, i, *i);
}

inline
int64_t median_of_3(int64_t a, int64_t b, int64_t c) {
  if (a < b)
    if (b < c) return b;
    else if (a < c) return c;
    else return a;
  else if (a < c) return a;
  else if (b < c) return c;
  else return b;
}

int64_t* unguarded_partition(int64_t* first, int64_t* last, int64_t pivot) {
  --last;
  while (*first < pivot) ++first;
  while (pivot < *last) --last;
  while (first < last) {
    int64_t tmp = *first;
    *first = *last;
    *last = tmp;
    ++first; --last;
    while (*first < pivot) ++first;
    while (pivot < *last) --last;
  }
  return first;
}

void quicksort_loop(int64_t* first, int64_t* last, ptrdiff_t threshold) {
  ptrdiff_t len = last - first;
  while (len > threshold) {
    int64_t* middle = first + (last - first) / 2;
    int64_t pivot = median_of_3(*first, *middle, *(last - 1));
    int64_t* cut = unguarded_partition(first, last, pivot);
    if (last - cut < cut - first) {
      quicksort_loop(cut, last, threshold);
      last = cut;
    } else {
      quicksort_loop(first, cut, threshold);
      first = cut;
    }
    len = last - first;
  }
}

void quicksort(int64_t* first, int64_t* last) {
  ptrdiff_t len = last - first;
  ptrdiff_t threshold = 16;
  if (len <= threshold) {
    insertion_sort(first, last);
  } else {
    quicksort_loop(first, last, threshold);
    int64_t* middle = first + threshold;
    insertion_sort(first, middle);
    unguarded_insertion_sort(middle, last);
  }
}

#endif

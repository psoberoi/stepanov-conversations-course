public final class Quicksort64
{
  static
  void unguarded_linear_insert(long[] array, int last, long value) {
    int previous = last;
    while (value < array[--previous]) array[last--] = array[previous];
    array[last] = value;
  }

  static
  void unguarded_insertion_sort(long[] array, int first, int last) {
    for (int i = first; i != last; ++i)
      unguarded_linear_insert(array, i, array[i]);
  }

  static
  void linear_insert(long[] array, int first, int last, long value) {
    if (value < array[first]) {
      while (first != last--) array[last + 1] = array[last];
      array[first] = value;
    }
    else unguarded_linear_insert(array, last, value);
  }

  static
  void insertion_sort(long[] array, int first, int last) {
    if (first == last) return;
    for (int i = first + 1; i != last; ++i)
      linear_insert(array, first, i, array[i]);
  }

  public static
  long median_of_3(long a, long b, long c) {
    if (a < b)
      if (b < c) return b;
      else if (a < c) return c;
      else return a;
    else if (a < c) return a;
    else if (b < c) return c;
    else return b;
  }

  static
  int unguarded_partition(long[] array, int first, int last, long pivot) {
    --last;
    while (array[first] < pivot) ++first;
    while (pivot < array[last]) --last;
    while (first < last) {
      long tmp = array[first];
      array[first] = array[last];
      array[last] = tmp;
      ++first; --last;
      while (array[first] < pivot) ++first;
      while (pivot < array[last]) --last;
    }
    return first;
  }

  static
  void quicksort_loop(long[] array, int first, int last, int threshold) {
    int len = last - first;
    while (len > threshold) {
      int middle = first + (last - first) / 2;
      long pivot = median_of_3(array[first], array[middle], array[last - 1]);
      int cut = unguarded_partition(array, first, last, pivot);
      if (last - cut < cut - first) {
        quicksort_loop(array, cut, last, threshold);
        last = cut;
      } else {
        quicksort_loop(array, first, cut, threshold);
        first = cut;
      }
      len = last - first;
    }
  }

  public static
  void quicksort(long[] array, int first, int last) {
    int len = last - first;
    int threshold = 16;
    if (len <= threshold) {
      insertion_sort(array, first, last);
    } else {
      quicksort_loop(array, first, last, threshold);
      int middle = first + threshold;
      insertion_sort(array, first, middle);
      unguarded_insertion_sort(array, middle, last);
    }
  }

  /* We don't need a constructor. */
  private Quicksort64() {}
}

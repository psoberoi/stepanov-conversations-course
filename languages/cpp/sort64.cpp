#include <cstdio>
#include <cstddef>
#include <cinttypes>
#include <chrono>
#include <vector>
#include <numeric>
#include <algorithm>

#include "quicksort64.h"

using namespace std;
using namespace std::chrono;

uint64_t nano_time() {
  auto now = high_resolution_clock::now().time_since_epoch();
  return duration_cast<nanoseconds>(now).count();
}

uint64_t time_sort(int64_t* data, int64_t* data_end, int64_t* buffer, size_t size) {
  auto startTime = nano_time();
  while (size <= data_end - data) {
    copy(data, data + size, buffer);
    quicksort(buffer, buffer + size);
    data += size;
  }
  return nano_time() - startTime;
}

int main() {
  size_t min_size(8);
  size_t max_size(16 * 1024 * 1024);

  printf("           %12s %6s %6s\n", "size", "time", "log2");

  vector<int64_t> data(max_size);
  vector<int64_t> buffer(max_size);
  int64_t* data_begin = &*begin(data);
  int64_t* data_end = data_begin + max_size;
  int64_t* buffer_begin = &*begin(buffer);

  iota(data_begin, data_end, 0);
  random_shuffle(data_begin, data_end);

  int lg = 3;
  for (size_t array_size = min_size; array_size <= max_size; array_size *= 2) {
    uint64_t time = time_sort(data_begin, data_end, buffer_begin, array_size);
    printf("     [C++] %12zd %6" PRIu64 " %6.1f\n", array_size, time / max_size, (double)time / (max_size * lg));
    ++lg;
  }
}

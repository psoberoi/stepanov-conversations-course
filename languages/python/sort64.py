from __future__ import division

import time
import array
import random

import quicksort64

def main():
  min_size = 8
  max_size = 1024 * 1024
  # max_size = 16 * 1024 * 1024

  print "           %12s %6s %6s" % ("size", "time", "log2")

  data = list(range(max_size))
  buffer = list(range(max_size))
  random.shuffle(data)

  array_size = min_size
  lg = 3
  while array_size < max_size:
    time = time_sort(data, buffer, array_size)
    print "  [Python] %12d %6d %6.1f" % (array_size, time // max_size, time / (max_size * lg))
    array_size *= 2
    lg += 1

def time_sort(data, buffer, size):
  first = 0;
  start_time = time.time()
  while first <= len(data) - size:
    buffer[0:size] = data[first:first+size]
    quicksort64.quicksort(buffer, 0, size)
    first += size
  return (time.time() - start_time) * 1e9

if __name__ == "__main__":
  main()

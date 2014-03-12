def unguarded_linear_insert(array, last, value):
  while value < array[last - 1]:
    array[last] = array[last - 1]
    last = last - 1
  array[last] = value

def unguarded_insertion_sort(array, first, last):
  for i in range(first, last):
    unguarded_linear_insert(array, i, array[i])

def linear_insert(array, first, last, value):
  if value < array[first]:
    while first != last:
      array[last] = array[last - 1]
      last = last - 1
    array[first] = value
  else:
    unguarded_linear_insert(array, last, value)

def insertion_sort(array, first, last):
  if (first == last): return
  for i in range(first + 1, last):
    linear_insert(array, first, i, array[i])

def median_of_3(a, b, c):
  if a < b:
    if b < c: return b
    elif a < c: return c
    else: return a
  elif a < c: return a
  elif b < c: return c
  else: return b

def unguarded_partition(array, first, last, pivot):
  last -= 1
  while array[first] < pivot: first += 1
  while pivot < array[last]: last -= 1
  while first < last:
    array[first], array[last] = array[last], array[first]
    first += 1
    last -= 1
    while array[first] < pivot: first += 1
    while pivot < array[last]: last -= 1
  return first

def quicksort_loop(array, first, last, threshold):
  len = last - first
  while len > threshold:
    middle = first + (last - first) // 2
    pivot = median_of_3(array[first], array[middle], array[last - 1])
    cut = unguarded_partition(array, first, last, pivot)
    if last - cut < cut - first:
      quicksort_loop(array, cut, last, threshold)
      last = cut
    else:
      quicksort_loop(array, first, cut, threshold)
      first = cut
    len = last - first

def quicksort(array, first, last):
  len = last - first
  threshold = 16
  if len <= threshold:
    insertion_sort(array, first, last)
  else:
    quicksort_loop(array, first, last, threshold)
    middle = first + threshold
    insertion_sort(array, first, middle)
    unguarded_insertion_sort(array, middle, last)


fn unguarded_linear_insert(array: &mut [u64], last: uint, value: u64) {
  let mut previous = last - 1;
  let mut current = last;
  while value < array[previous] {
    array[current] = array[previous];
    previous -= 1;
    current -= 1;
  }
  array[current] = value;
}

fn unguarded_insertion_sort(array: &mut [u64], first: uint, last: uint) {
  let mut i = first;
  while i != last {
    unguarded_linear_insert(array, i, array[i]);
    i += 1;
  }
}

fn linear_insert(array: &mut [u64], first: uint, last: uint, value: u64) {
  let mut last = last;
  if value < array[first] {
    while first != last {
      array[last + 1] = array[last];
      last -= 1;
    }
    array[first] = value;
  } else {
    unguarded_linear_insert(array, last, value);
  }
}

fn insertion_sort(array: &mut [u64], first: uint, last: uint) {
  if first == last { return; }
  let mut i = first + 1;
  while i != last {
    linear_insert(array, first, i, array[i]);
    i += 1;
  }
}

fn median_of_3(a: u64, b: u64, c: u64) -> u64 {
  if a < b {
    if b < c { return b; }
    else if a < c { return c; }
    else { return a; }
  } else if a < c { return a; }
  else if b < c { return c; }
  else { return b; }
}

fn unguarded_partition(array: &mut [u64], first: uint, last: uint, pivot: u64) -> uint {
  let mut first = first;
  let mut last = last - 1;
  while array[first] < pivot { first += 1; }
  while pivot < array[last] { last -= 1; }
  while first < last {
    let tmp = array[first];
    array[first] = array[last];
    array[last] = tmp;
    first += 1;
    last -= 1;
    while array[first] < pivot { first += 1; }
    while pivot < array[last] { last -= 1; }
  }
  return first;
}

fn quicksort_loop(array: &mut [u64], first: uint, last: uint, threshold: uint) {
  let mut first = first;
  let mut last = last;
  let mut len = last - first;
  while len > threshold {
    let middle = first + (last - first) / 2;
    let pivot = median_of_3(array[first], array[middle], array[last - 1]);
    let cut = unguarded_partition(array, first, last, pivot);
    if last - cut < cut - first {
      quicksort_loop(array, cut, last, threshold);
      last = cut;
    } else {
      quicksort_loop(array, first, cut, threshold);
      first = cut;
    }
    len = last - first;
  }
}

pub fn quicksort(array: &mut [u64], first: uint, last: uint) {
  let len = last - first;
  let threshold = 16u;
  if len <= threshold {
    insertion_sort(array, first, last);
  } else {
    quicksort_loop(array, first, last, threshold);
    let middle = first + threshold;
    insertion_sort(array, first, middle);
    unguarded_insertion_sort(array, middle, last);
  }
}

local quicksort64 = {}

local function unguarded_linear_insert(array, last, value)
  while value < array[last - 1] do
    array[last] = array[last - 1]
    last = last - 1
  end
  array[last] = value
end

local function unguarded_insertion_sort(array, first, last)
  for i = first, last - 1 do
    unguarded_linear_insert(array, i, array[i])
  end
end

local function linear_insert(array, first, last, value)
  if value < array[first] then
    while first ~= last do
      array[last] = array[last - 1]
      last = last - 1
    end
    array[first] = value
  else
    unguarded_linear_insert(array, last, value)
  end
end

local function insertion_sort(array, first, last)
  if first == last then return end
  for i = first + 1, last - 1 do
    linear_insert(array, first, i, array[i])
  end
end

local function median_of_3(a, b, c)
  if a < b then
    if b < c then return b
    elseif a < c then return c
    else return a end
  elseif a < c then return a
  elseif b < c then return c
  else return b end
end

local function unguarded_partition(array, first, last, pivot)
  last = last - 1
  while array[first] < pivot do first = first + 1 end
  while pivot < array[last] do last = last - 1 end
  while first < last do
    array[first], array[last] = array[last], array[first]
    first = first + 1
    last = last - 1
    while array[first] < pivot do first = first + 1 end
    while pivot < array[last] do last = last - 1 end
  end
  return first
end

local function quicksort_loop(array, first, last, threshold)
  len = last - first
  while len > threshold do
    middle = first + math.floor((last - first) / 2)
    pivot = median_of_3(array[first], array[middle], array[last - 1])
    cut = unguarded_partition(array, first, last, pivot)
    if last - cut < cut - first then
      quicksort_loop(array, cut, last, threshold)
      last = cut
    else
      quicksort_loop(array, first, cut, threshold)
      first = cut
    end
    len = last - first
  end
end

function quicksort64.quicksort(array, first, last)
  len = last - first
  threshold = 16
  if len <= threshold then
    insertion_sort(array, first, last)
  else
    quicksort_loop(array, first, last, threshold)
    middle = first + threshold
    insertion_sort(array, first, middle)
    unguarded_insertion_sort(array, middle, last)
  end
end

return quicksort64

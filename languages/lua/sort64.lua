local quicksort64 = require 'quicksort64'

function main()
  math.randomseed(os.time())

  min_size = 8
  max_size = 16 * 1024 * 1024

  print(("           %12s %6s %6s"):format("size", "time", "log2"))

  data = {}
  for i = 1, max_size do data[i] = i - 1 end
  buffer = {}
  for i, v in ipairs(data) do buffer[i] = v end
  shuffle(data)

  array_size = min_size
  lg = math.log(min_size, 2)
  while array_size <= max_size do
    time = time_sort(data, buffer, array_size)
    print(("     [Lua] %12d %6d %6.1f"):format(array_size, time / max_size, time / (max_size * lg)))
    array_size = array_size * 2
    lg = lg + 1
  end
end

function time_sort(data, buffer, size)
  first = 0;
  start_time = os.time()
  while first <= #data - size do
    for i = 1, size do buffer[i] = data[first + i] end
    quicksort64.quicksort(buffer, 1, size + 1)
    first = first + size
  end
  return os.difftime(os.time(), start_time) * 1e9
end

function shuffle(t)
  for i = #t, 1, -1 do
    x = math.random(i)
    t[i], t[x] = t[x], t[i]
  end
end

main()

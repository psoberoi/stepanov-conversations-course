
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

#include "vbyte_descriptor.h"
#include "tape.h"
#include "minmax.h"

int main() {
  typedef tape<vbyte_descriptor> vbyte_tape;
  uint64_t array1[] = {268435456, 1, 77777, 1,  77777777, 1,      88};
  uint64_t array2[] = {1,         7,  1, 7777777, 1,        899999, 89999999999};
  vbyte_tape vbytes1(array1, array1 + sizeof(array1)/8);
  vbyte_tape vbytes2(array2, array2 + sizeof(array2)/8);
  uint64_t result = inner_product(vbytes1.begin(), vbytes2.begin(), vbytes1.size(), min_max_semiring<uint64_t>());
  std::cout << result << std::endl;
}

 


#include <algorithm>
#include <iostream>
#include <vector>
#include <numeric>

#include "vbyte_descriptor.h"
#include "tape.h"
#include "accumulate_iterator.h"

template <typename Container>
void display_container(const Container& c, const char* s = NULL) {
  std::cout << std::endl << s << " ";
  typename Container::const_iterator i = c.begin();
  while (i != c.end()) {
    std::cout << *i++ << " ";
  }
  std::cout << std::endl;
}

int main() {
  // assuming that  
  typedef tape<vbyte_descriptor> vbyte_tape;
  typedef accumulate_iterator<vbyte_tape::iterator> iter;

  // we can take two arrays of postings
  uint32_t docids1[] = {123456, 123458, 123461, 124456, 124457, 126123, 126125, 126131};
  uint32_t docids2[] = {123458, 123459, 123461, 124457, 124459, 126125, 126126, 126131};

  // them we convert them to compressed (delta-encoded form) into two tapes:
  
  vbyte_tape vbytes1;
  vbyte_tape vbytes2;

  std::adjacent_difference(docids1, docids1 + 8, std::back_inserter(vbytes1));
  std::adjacent_difference(docids2, docids2 + 8, std::back_inserter(vbytes2));


  display_container(vbytes1, "vbytes1:"); // prints: vbytes1: 123456 2 3 995 1 1666 2 6 

  display_container(vbytes2, "vbytes2:"); // prints: vbytes2: 123458 1 2 996 2 1666 1 5 

  std::vector<uint32_t> intersection;
  
  std::set_intersection(iter(vbytes1.begin()), iter(vbytes1.end()),
			iter(vbytes2.begin()), iter(vbytes2.end()),
			std::back_inserter(intersection));

  display_container(intersection, "intersection:"); // prints: intersection: 123458 123461 124457 126125 126131 


}

/*

  // assuming that  
  typedef tape<vbyte_descriptor> vbyte_tape;
  typedef accumulate_iterator<vbyte_tape::iterator> iter;

  // we can take two arrays of postings
  uint32_t docids1[] = {123456, 123458, 123461, 124456, 124457, 126123, 126125, 126131};
  uint32_t docids2[] = {123458, 123459, 123461, 124457, 124459, 126125, 126126, 126131};

  // them we convert them to compressed (delta-encoded form) into two tapes:
  
  vbyte_tape vbytes1;
  vbyte_tape vbytes2;

  std::adjacent_difference(docids1, docids1 + 8, std::back_inserter(vbytes1));
  std::adjacent_difference(docids2, docids2 + 8, std::back_inserter(vbytes2));


  // vbytes1 contains: 123456 2 3 995 1 1666 2 6 

  // vbytes2 contains: 123458 1 2 996 2 1666 1 5 

  std::vector<uint32_t> intersection;
  
  std::set_intersection(iter(vbytes1.begin()), iter(vbytes1.end()),
			iter(vbytes2.begin()), iter(vbytes2.end()),
			std::back_inserter(intersection));

  // intersection contains: 123458 123461 124457 126125 126131 


 */

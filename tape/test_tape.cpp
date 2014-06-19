
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include "timer.h"

#include "vbyte_descriptor.h"
#include "tape.h"

template <typename Tape>
double measure(Tape& tape) {
  uint64_t result(0);
  timer t;
  t.start();
  std::accumulate(tape.begin(), tape.end(), result);
  return t.stop();
}

int main() {
  typedef tape<vbyte_descriptor> vbyte_tape;
  //  typedef std::reverse_iterator<vbyte_tape::const_iterator> r_iterator;
  uint64_t array[] = {268435456, 7, 777, 77777, 77777777, 8, 88, 8888, 3141111111111ll, 1, 256, 1048576, 268435456};
  vbyte_tape vbytes(array, array + sizeof(array)/8);
  measure(vbytes);
  std::copy(vbytes.begin(), vbytes.end(), std::ostream_iterator<uint64_t>(std::cout, " "));
  std::cout << std::endl << "size of tape: " << vbytes.size() << std::endl;
  vbytes.insert(vbytes.end(), array, array + sizeof(array)/8);
  vbyte_tape::const_iterator first_8s = std::find(vbytes.begin(), vbytes.end(), 8);
  vbyte_tape::const_iterator last_8s = std::find(vbytes.begin(), vbytes.end(), 8888);
  vbytes.erase(first_8s, ++last_8s);
  vbyte_tape::const_iterator first = vbytes.begin();
  vbyte_tape::const_iterator last =  vbytes.end();
  
  //  std::cout << uint64_t(first.position) << " " <<  uint64_t(last.position) << std::endl;
  //  std::copy(first.position, last.position, std::ostream_iterator<uint64_t>(std::cout, " "));
  std::copy(first, last, std::ostream_iterator<uint64_t>(std::cout, " "));
  std::cout << std::endl << "size of tape: " << vbytes.size() << std::endl;
  /* 

  r_iterator r_last(vbytes.begin());
  r_iterator r_first(vbytes.end());
  
  std::copy(r_first, r_last, std::ostream_iterator<uint64_t>(std::cout, " "));
  std::cout << std::endl << "size of tape: " << vbytes.size() << std::endl;
  */ 
  std::istringstream  iss(std::string("9 99 999 9999 99999 999999 9999999 9999999999"));
  std::istream_iterator<uint64_t> i_first(iss);
  std::istream_iterator<uint64_t> i_last;
  vbytes.insert(vbytes.end(), i_first, i_last);

  first = vbytes.begin();
  last =  vbytes.end();
  std::copy(first, last, std::ostream_iterator<uint64_t>(std::cout, " "));
  std::cout << std::endl << "size of tape: " << vbytes.size() << std::endl;

  vbyte_tape vbytes1 = vbytes;

  if (!(vbytes == vbytes1)) std::cout << "equality failed" << std::endl;
  if (vbytes < vbytes1) std::cout << "less_than failed" << std::endl;

  std::copy(vbytes1.begin(), vbytes1.end(), std::ostream_iterator<uint64_t>(std::cout, " "));
  std::cout << std::endl << "size of tape: " << vbytes1.size() << std::endl;

  vbytes1 = vbyte_tape();

  std::copy(vbytes1.begin(), vbytes1.end(), std::ostream_iterator<uint64_t>(std::cout, " "));

  std::cout << std::endl << "size of tape: " <<  vbytes1.size() << " size of pointer: " << sizeof(first) << std::endl;


  vbyte_tape vbytes2 = vbytes;

  std::cout << "inserting from vbytes" << std::endl;
  vbytes2.insert(vbytes2.end(), vbytes.begin(), vbytes.end());
  

  std::copy(vbytes2.begin(), vbytes2.end(), std::ostream_iterator<uint64_t>(std::cout, " "));
  std::cout << std::endl << "size of tape: " <<  vbytes2.size() << std::endl;
  
  
}

// Local Variables:
// mode: c++
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:


#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include <stdint.h>
#include <stdlib.h>
#include <iostream>
#include <limits>
#include <list>
#include <algorithm>
#include <numeric>

#include "vbyte_descriptor.h"
#include "tape.h"
#include "statistic.h"


class TapeTest : public CppUnit::TestFixture {
private:
  typedef tape<vbyte_descriptor> vbyte_tape;
  vbyte_tape vbytes;

public:
  static const uint64_t test_data[];
  static const uint64_t* test_data_end;

  void setUp();
  void tearDown();

  void testConstructionFromRange();
  void testEncodingMaxVal();
  void testCopyAndEquality();
  void testEmptyComparisons();
  void testAssignment();
  void testInsertAtEnd();
  void testPushBack();
  void testInsertInMiddle();
  void testEmptyInserts();
  void testComparison();
  void testReverseIterator();
  void testInsertionFromInputIterator();
  void testInsertionFromAnotherTape();
  void testErase();
  void testAdjustByteCapacity();
  void testIterators();

  CPPUNIT_TEST_SUITE( TapeTest );
  CPPUNIT_TEST( testConstructionFromRange );
  CPPUNIT_TEST( testEncodingMaxVal );
  CPPUNIT_TEST( testCopyAndEquality );
  CPPUNIT_TEST( testEmptyComparisons );
  CPPUNIT_TEST( testAssignment );  
  CPPUNIT_TEST( testInsertAtEnd );
  CPPUNIT_TEST( testPushBack );
  CPPUNIT_TEST( testInsertInMiddle );
  CPPUNIT_TEST( testEmptyInserts );
  CPPUNIT_TEST( testComparison );
  CPPUNIT_TEST( testReverseIterator );
  CPPUNIT_TEST( testInsertionFromInputIterator );
  CPPUNIT_TEST( testInsertionFromAnotherTape );
  CPPUNIT_TEST( testErase );
  CPPUNIT_TEST( testAdjustByteCapacity );
  CPPUNIT_TEST( testIterators );
  CPPUNIT_TEST_SUITE_END();

};


CPPUNIT_TEST_SUITE_REGISTRATION( TapeTest );

const uint64_t TapeTest::test_data[] = { 268435456, 7, 777, 77777, 77777777, 8, 88, 8888, 
					 3141111111111ll, 1, 256, 1048576, 268435456 };
const uint64_t* TapeTest::test_data_end = test_data + sizeof(test_data)/sizeof(uint64_t);


void TapeTest::setUp() {
  vbytes = vbyte_tape(test_data, test_data_end);
}
    
void TapeTest::tearDown() {
}


void TapeTest::testConstructionFromRange() {
  // tests construction from forward iterator range
  vbyte_tape vbytes1(test_data, test_data_end);
  CPPUNIT_ASSERT( vbytes.size() == (sizeof(test_data)/sizeof(uint64_t)) );
  CPPUNIT_ASSERT( std::equal(vbytes1.begin(), vbytes1.end(), test_data) );
}

void TapeTest::testInsertionFromInputIterator() {
  typedef std::list<uint64_t> list_t;
  list_t alist(vbytes.begin(), vbytes.end()); 

  CPPUNIT_ASSERT( std::equal(vbytes.begin(), vbytes.end(), alist.begin()) );

  vbyte_tape::const_iterator p = vbytes.begin();
  ++p; ++p; ++p;
  list_t::iterator lp = alist.begin();
  ++lp; ++lp; ++lp;
		  

  std::string the_input("9 99 999 9999 99999 999999 9999999 99999999 999999999 "
			"9999999999 9999999999 99999999999 9999999999999");
  std::istringstream  iss1(the_input);
  std::istringstream  iss2(the_input);
  std::istream_iterator<uint64_t> i1_first(iss1), i2_first(iss2);
  std::istream_iterator<uint64_t> i1_last, i2_last;
  vbytes.insert(p, i1_first, i1_last);
  alist.insert(lp, i2_first, i2_last);

  CPPUNIT_ASSERT( std::equal(vbytes.begin(), vbytes.end(), alist.begin()) );
}

void TapeTest::testInsertionFromAnotherTape() {
  vbyte_tape vbytes1;
  vbytes1.insert(vbytes1.end(), vbytes.begin(), vbytes.end());
  CPPUNIT_ASSERT( vbytes1 == vbytes );
}


void TapeTest::testEncodingMaxVal() {
  uint64_t max = std::numeric_limits<uint64_t>::max();
  vbyte_tape vbytes1(&max, &max + 1);
  CPPUNIT_ASSERT( vbyte_tape::descriptor_type().encoded_size(max) == vbytes1.get_extent().byte_size() );
  CPPUNIT_ASSERT( *vbytes1.begin() == max );
}
  
void TapeTest::testCopyAndEquality() {
  CPPUNIT_ASSERT( vbytes == vbytes );

  vbyte_tape vbytes_copy(vbytes);
  CPPUNIT_ASSERT( vbytes_copy == vbytes );

  uint64_t val[] = { 1 };
  vbytes_copy.insert(vbytes_copy.end(), val, val + 1);
  CPPUNIT_ASSERT( !(vbytes_copy == vbytes) && (vbytes_copy != vbytes) );

  vbytes_copy.erase(vbytes_copy.begin(), ++vbytes_copy.begin());
  CPPUNIT_ASSERT( !(vbytes_copy == vbytes) && (vbytes_copy != vbytes) );
}

void TapeTest::testEmptyComparisons() {
  vbyte_tape empty1;
  vbyte_tape empty2;

  CPPUNIT_ASSERT( empty1 == empty2 );

  CPPUNIT_ASSERT( !(empty1 < empty2) );

  CPPUNIT_ASSERT( !(empty2 < empty1) );
}


void TapeTest::testAssignment() {
  vbyte_tape vbytes1;
  // assign to empty
  vbytes1 = vbytes;
  CPPUNIT_ASSERT( vbytes1 == vbytes );
  // assign to nonempty
  vbytes1 = vbytes;
  CPPUNIT_ASSERT( vbytes1 == vbytes );
}

void TapeTest::testInsertAtEnd() {
  vbyte_tape vbytes1;
  for (int i = 1; i <= 100000; ++i) {
    uint64_t rand_val = (uint64_t(lrand48()) << 32 | uint64_t(lrand48())); 
    vbytes1.insert(vbytes1.end(), &rand_val, &rand_val + 1);
    CPPUNIT_ASSERT( vbytes1.size() == i );
    CPPUNIT_ASSERT( *(--vbytes1.end()) == rand_val );
  }
}

void TapeTest::testPushBack() {
  vbyte_tape vbytes1(vbytes);
  uint64_t rand_val = (uint64_t(lrand48()) << 32 | uint64_t(lrand48())); 
  vbytes1.insert(vbytes1.end(), &rand_val, &rand_val + 1);
  vbytes.push_back(rand_val);
  CPPUNIT_ASSERT( vbytes1 == vbytes );
}

void TapeTest::testInsertInMiddle() {
  vbyte_tape vbytes1;
  vbyte_tape::const_iterator p = vbytes1.begin();
  vbyte_tape::const_iterator p2;

  typedef std::list<uint64_t> list_t;
  list_t alist;
  list_t::iterator lp = alist.begin();
  list_t::iterator lp2;

  for (int i = 1; i <= 10; ++i) {
    // generate a random value
    uint64_t rand_val = (uint64_t(lrand48()) << 32 | uint64_t(lrand48())); 
    // Insert it into the middle
    p = vbytes1.insert(p, &rand_val, &rand_val + 1).first;
    // check that it seems to have gone in
    CPPUNIT_ASSERT( vbytes1.size() == i );
    CPPUNIT_ASSERT( *p == rand_val );
    if ((i & 1) == 1) ++p;  // keep p at the middle
    // do same in the list
    lp = alist.insert(lp, rand_val);
    if ((i & 1) == 1) ++lp;  // keep lp in the middle
  }
  // Iteration over tape and list should look equal at this point
  CPPUNIT_ASSERT( std::equal(vbytes1.begin(), vbytes1.end(), alist.begin()) );
}

void TapeTest::testEmptyInserts() {
  vbyte_tape empty1;
  vbyte_tape empty2;

  CPPUNIT_ASSERT( empty1.empty() && empty2.empty() );

  empty2.insert(empty2.end(), empty1.begin(), empty1.end());
  CPPUNIT_ASSERT( empty2.empty() );

  vbyte_tape vbyte_copy(vbytes);
  vbytes.insert(vbytes.end(), empty1.begin(), empty1.end());
  CPPUNIT_ASSERT( vbytes == vbyte_copy );


  std::string the_input("9 99 999 9999 99999 999999 9999999 99999999 999999999 "
			"9999999999 9999999999 99999999999 9999999999999");
  std::istringstream  iss1(the_input);
  std::istream_iterator<uint64_t> i1_first(iss1);
  vbytes.insert(vbytes.end(), i1_first, i1_first);
  CPPUNIT_ASSERT( vbytes == vbyte_copy );

  empty2.insert(empty2.end(), i1_first, i1_first);
  CPPUNIT_ASSERT( empty2.empty() );
}

void TapeTest::testComparison() {
  vbyte_tape vbytes1(vbytes);
  CPPUNIT_ASSERT( !((vbytes1 < vbytes) || (vbytes < vbytes1)) );

  vbytes1.push_back(uint64_t(1));
  CPPUNIT_ASSERT( vbytes < vbytes1 );
  CPPUNIT_ASSERT( vbytes1 > vbytes );

  vbytes1 = vbytes;
  uint64_t val = *++vbytes1.begin();
  val += 1;
  vbytes1.erase(++vbytes1.begin(), ++(++vbytes1.begin()));
  vbytes1.insert(++vbytes1.begin(), &val, &val + 1);
  CPPUNIT_ASSERT(vbytes < vbytes1 );
  CPPUNIT_ASSERT(vbytes1 > vbytes );
}

void TapeTest::testReverseIterator() {
  typedef std::reverse_iterator<vbyte_tape::const_iterator> r_iterator;
  r_iterator r_last(vbytes.begin());
  r_iterator r_first(vbytes.end());
  std::list<uint64_t> alist;
  while (r_first != r_last) {
    alist.push_front(*r_first++);
  }
  CPPUNIT_ASSERT( std::equal(vbytes.begin(), vbytes.end(), alist.begin()) );
}

void TapeTest::testErase() {
  typedef std::list<uint64_t> list_t;
  list_t alist(vbytes.begin(), vbytes.end());
  vbyte_tape::const_iterator p = vbytes.begin();
  ++p; 
  vbyte_tape::const_iterator p2 = p;
  ++p2; ++p2;
  list_t::iterator lp = alist.begin();
  ++lp;
  list_t::iterator lp2 = lp;
  ++lp2; ++lp2;

  p = vbytes.erase(p, p2);
  lp = alist.erase(lp, lp2);

  CPPUNIT_ASSERT( std::distance(vbytes.begin(), p) == std::distance(alist.begin(), lp));
  CPPUNIT_ASSERT( std::equal(vbytes.begin(), vbytes.end(), alist.begin()) );

  vbyte_tape vbytes1(vbytes);
  vbytes1.erase(vbytes1.begin(), vbytes1.end());
  CPPUNIT_ASSERT( vbytes1.empty() );

  // test that erase on empty doesn't die 
  vbytes1.erase(vbytes1.begin(), vbytes1.end());
}


void TapeTest::testAdjustByteCapacity() {
  vbyte_tape vbytes1;
  zipf z(std::numeric_limits<uint32_t>::max());
  for (int i = 1; i <= 1000000; ++i) {
    uint32_t rand_val = uint32_t(z.random()); 
    vbytes1.insert(vbytes1.end(), &rand_val, &rand_val + 1);
  }
  
  vbyte_tape vbytes2(vbytes1);
  vbytes1.adjust_byte_capacity(107);
  CPPUNIT_ASSERT( vbytes1.get_extent().remaining_byte_capacity() == 107 );
  CPPUNIT_ASSERT( vbytes1 == vbytes2 );
}

void TapeTest::testIterators() {
  std::vector<uint64_t> v;
  vbyte_tape vbytes1;
  zipf z(std::numeric_limits<uint32_t>::max());
  for (int i = 1; i <= 1000000; ++i) {
    uint32_t rand_val = uint32_t(z.random()); 
    v.insert(v.end(), &rand_val, &rand_val + 1);
    vbytes1.insert(vbytes1.end(), &rand_val, &rand_val + 1);
  }
  
  vbyte_tape vbytes2(vbytes1);
  CPPUNIT_ASSERT( std::equal(vbytes1.begin(), vbytes1.end(), vbytes2.begin()) );
  CPPUNIT_ASSERT( vbytes1 == vbytes2 );
  CPPUNIT_ASSERT( std::equal(vbytes2.begin(), vbytes2.end(), v.begin()) );
}


// Not currently run
/*
void TapeTest::testSizeComparisonWithVector() {
  vbyte_tape vbytes1;
  std::vector<uint32_t> vec;
  zipf z(std::numeric_limits<uint32_t>::max());
  for (int i = 1; i <= 1000000; ++i) {
    uint32_t rand_val = uint32_t(z.random()); 
    vbytes1.insert(vbytes1.end(), &rand_val, &rand_val + 1);
    vec.insert(vec.end(), &rand_val, &rand_val + 1);
  }
  std::cout << std::endl << "Size of vbytes: " << sizeof(vbytes1) + vbytes1.total_byte_size() << std::endl;
  std::cout << "Size of vector: " << sizeof(vec) + vec.size() * sizeof(uint32_t) << std::endl;
}
*/

/* No longer used
template <typename Tape>
static double measure(Tape& tape) {
  uint64_t result(0);
  timer t;
  t.start();
  std::accumulate(tape.begin(), tape.end(), result);
  return t.stop();
}
*/




/*** Runner ***/

int main(int, char**) {
  // Adds the test to the list of test to run
  CppUnit::TextUi::TestRunner runner;
  runner.addTest( TapeTest::suite() );

  // Change the default outputter to an error format outputter
  runner.setOutputter( new CppUnit::CompilerOutputter( &runner.result(), std::cerr ) );

  // Run the tests; return 0 on success, 1 on failure
  return int(!runner.run());
}

// Local Variables:
// mode: c++
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:

#include <iterator>

#define Regular typename
#define Semiregular typename
#define TotallyOrdered typename
#define StrictWeakOrdering typename

#define InputIterator typename
#define ForwardIterator typename
#define BidirectionalIterator typename
#define RandomAccessIterator typename

#define IteratorBasis typename

template <InputIterator I>
using IteratorCategory = typename std::iterator_traits<I>::iterator_category;

template <InputIterator I>
using ValueType = typename std::iterator_traits<I>::value_type;

template <InputIterator I>
using DifferenceType = typename std::iterator_traits<I>::difference_type;

template <InputIterator I>
using Pointer = typename std::iterator_traits<I>::pointer;

template <InputIterator I>
using Reference = typename std::iterator_traits<I>::reference;

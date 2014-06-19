#ifndef ITERATOR_ADAPTER_H
#define ITERATOR_ADAPTER_H

#include <stddef.h>
#include <iterator>

namespace adapter {

/*
IteratorBasis is a minimal class out of which we can implement a full iterator interface.

concept IteratorBasis<SemiRegular X> =
     SemiRegular<X::value_type> 
  && Regular<X::state_type>
  && IteratorCategory<X::iterator_category>
  && SignedIntegral<X::difference_type>
  && Type<X::reference>
  && Type<X::pointer>
  && requires(X b, X b1, X::value_type v) {
                reference { b.deref() };
                void { b.increment() }; 
                // defined if iterator category is at least bidirecitional:
                void { b.decrement() };
                // defined if iterator category is random_access:
                void { b.increment(X::difference_type n) };
                difference_type { b.difference(const X& b1) };
                const state_type& { b.state() };
             }
*/

/*
template <typename Iterator>
class reverse_iterator_basis
{
  typedef Iterator state_type;
  typedef typename Iterator::iterator_category iterator_category; 
  typedef typename Iterator::value_type value_type;
  typedef typename Iterator::difference_type difference_type;
  typedef typename Iterator::reference reference;
  typedef typename Iterator::pointer pointer;  

  state_type position;

  reference deref() {
    state_type tmp = position;
    return *--tmp;
  }

  void increment() { --position; }
  void decrement() { ++position; }
  void increment(difference_type n) { position -= n; }
  difference_type difference(const reverse_iterator_basis& x) { 
    return position - x.position; 
  }
  const state_type& state() { return position; }

};

typedef iterator<reverse_iterator_basis<int*> > reverse_int_pointer;
*/

template <typename IteratorBasis>
class iterator
{
public:
  typedef typename IteratorBasis::state_type state_type;
  typedef typename IteratorBasis::iterator_category iterator_category;
  typedef typename IteratorBasis::value_type value_type;
  typedef typename IteratorBasis::difference_type difference_type;
  typedef typename IteratorBasis::reference reference;
  typedef typename IteratorBasis::pointer pointer;

private:
  IteratorBasis basis;
public:
  const state_type& state() const { return basis.state(); }

  iterator() {}

  iterator(const IteratorBasis& basis) : basis(basis) {} 

  reference operator*() const { 
    return basis.deref();
  }

  pointer operator->() const { return &(operator*()); }

  iterator& operator++() { 
    basis.increment();
    return *this;
  }

  iterator operator++(int) {
    iterator tmp = *this;
    ++*this;
    return tmp;
  }

  // requires bidirectional iterator (at least)
  iterator& operator--() { 
    basis.decrement();
    return *this;
  }

  // requires bidirectional iterator (at least)
  iterator operator--(int) {
    iterator tmp = *this;
    --*this;
    return tmp;
  }

  // requires random access iterator
  iterator& operator+=(difference_type n) {
    basis.increment(n);
    return *this;
  }

  // requires random access iterator
  iterator& operator-=(difference_type n) {
    return *this += -n;
  }

  // requires random access iterator
  friend
  iterator operator+(iterator x, difference_type n) {
    return x += n;
  }

  // requires random access iterator
  friend
  iterator operator+(difference_type n, iterator x) {
    return x += n;
  }

  // requires random access iterator
  friend
  iterator operator-(iterator x, difference_type n) {
    return x -= n;
  }

  // requires random access iterator
  friend
  difference_type operator-(const iterator& x, const iterator& y) {
    return x.basis.difference(y.basis);
  }

  // requires random access iterator
  reference operator[](difference_type n) const { return *(*this + n); }

  friend
  bool operator==(const iterator& x, const iterator& y) {
    return x.state() == y.state();
  }

  friend
  bool operator!=(const iterator& x, const iterator& y) {
    return !(x == y);
  }

  // requires random access iterator
  friend
  bool operator<(const iterator& x, const iterator& y) {
    return y - x > 0;
  }

  // requires random access iterator
  friend
  bool operator>(const iterator& x, const iterator& y) {
    return y < x;
  }

  // requires random access iterator
  friend
  bool operator<=(const iterator& x, const iterator& y) {
    return !(y < x);
  }

  // requires random access iterator
  friend
  bool operator>=(const iterator& x, const iterator& y) {
    return !(x < y);
  }
};

// Our implementation of output_iterator template depends on STL guarantee that
// "any [output] iterator value should be assigned through before
// it is incremented" (STL Report, page 8)
// http://www.stepanovpapers.com/STL/DOC.PDF

/*
OutputIteratorBasis is a minimal class out of which we can implement a full output iterator interface.

concept OutputIteratorBasis<SemiRegular X> =
     Concept<X::Storable>
  && SemiRegular<X::state_type>
  && requires(X b, Typename T) {
                const state_type& { b.state() };
                void { b.store(const Storable<T>& x) };
             }
*/

class cout_basis {
public:
  typedef void state_type;
  void state() {}
  void store(const int& x) { std::cout << x; }
  void store(const double& x) { std::cout << x; }
}; 

template <typename OutputIteratorBasis>
class output_iterator
{
public:
  typedef typename OutputIteratorBasis::state_type state_type;
  typedef std::output_iterator_tag iterator_category; 
  typedef void                     value_type;
  typedef void                     difference_type;
  typedef void                     pointer;
  typedef void                     reference;

private:
  OutputIteratorBasis basis;
public:
  const state_type& state() const { return basis.state(); }

  output_iterator() {}
  
  output_iterator(const OutputIteratorBasis& basis) : basis(basis) {}

  template <typename T>
  void operator=(const T& value) { 
    basis.store(value);
  }

  output_iterator& operator*() { return *this; }
  output_iterator& operator++() { return *this; }
  output_iterator& operator++(int) { return *this; }
};

template <typename OutputIteratorBasis>
inline
output_iterator<OutputIteratorBasis> 
make_output_iterator(const OutputIteratorBasis& basis) {
  return output_iterator<OutputIteratorBasis>(basis);
}

} // end namespace adapter

// Local Variables:
// mode: c++
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
#endif

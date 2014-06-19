#ifndef VARIABLE_SIZE_TYPE_ITERATOR_H
#define VARIABLE_SIZE_TYPE_ITERATOR_H

#include <stdint.h>
#include <stddef.h>
#include <iterator>
#include <algorithm>
#include <new>

// Only included for concepts
#include "variable_size_type.h"
#include "iterator_adapter.h"

template <typename VariableSizeTypeDescriptor,
          bool prefixed_size,
          typename IteratorCategory>
struct variable_size_iterator_basis;

template <typename VariableSizeTypeDescriptor>
struct variable_size_iterator_basis_base {
  typedef VariableSizeTypeDescriptor descriptor_type;

  struct state_type {
    const uint8_t* position;
    descriptor_type dsc;
    state_type(const uint8_t* position, descriptor_type dsc) :
      position(position), dsc(dsc) {}
    state_type() {}
  };

  friend bool
  operator==(const state_type& x,
             const state_type& y) {
    return x.position == y.position;
  }

  typedef typename VariableSizeTypeDescriptor::iterator_category iterator_category; 
  typedef typename VariableSizeTypeDescriptor::value_type value_type;
  typedef ptrdiff_t difference_type;
  typedef value_type reference;
  typedef void pointer;

  state_type st;
  
  variable_size_iterator_basis_base() {}
  variable_size_iterator_basis_base(const uint8_t* position, const descriptor_type& dsc) : st(position, dsc) {}

};


template <typename VariableSizeTypeDescriptor>
struct variable_size_iterator_basis<VariableSizeTypeDescriptor, 
                                    true,
                                    std::forward_iterator_tag> 
  : variable_size_iterator_basis_base<VariableSizeTypeDescriptor>
{
private:
  typedef variable_size_iterator_basis_base<VariableSizeTypeDescriptor> base;
public:
  variable_size_iterator_basis() {}

  variable_size_iterator_basis(const uint8_t*,
                               const uint8_t* position,
                               const VariableSizeTypeDescriptor& dsc) 
    : base(position, dsc) {}

  const typename base::state_type&
  state() { return this->st; }

  typename base::reference
  deref() const { return this->st.dsc.decode(this->st.position);  }

  void increment() { this->st.position += this->st.dsc.size(this->st.position); }
};

template <typename VariableSizeTypeDescriptor>
struct variable_size_iterator_basis<VariableSizeTypeDescriptor, 
                                    true, 
                                    std::bidirectional_iterator_tag>
  : variable_size_iterator_basis_base<VariableSizeTypeDescriptor>
{
private:
  typedef variable_size_iterator_basis_base<VariableSizeTypeDescriptor> base;
public:
  const uint8_t* origin;

  variable_size_iterator_basis() : base(), origin(NULL) {}

  variable_size_iterator_basis(const uint8_t* origin,
                               const uint8_t* position,
                               const VariableSizeTypeDescriptor& dsc) 
    : base(position, dsc), origin(origin) {}

  const typename base::state_type&
  state() { return this->st; }

  typename base::reference 
  deref() const { return this->st.dsc.decode(this->st.position); }

  void increment() { this->st.position += this->st.dsc.size(this->st.position); }

  void decrement() { this->st.position = this->st.dsc.previous(origin, this->st.position); }
};


template <typename VariableSizeTypeDescriptor>
struct variable_size_iterator_basis<VariableSizeTypeDescriptor, 
                                   false, 
                                   std::forward_iterator_tag>
  : variable_size_iterator_basis_base<VariableSizeTypeDescriptor>
{
private:
  typedef variable_size_iterator_basis_base<VariableSizeTypeDescriptor> base;
  std::pair<typename base::value_type, size_t> cache;
public:
  variable_size_iterator_basis() : base() {
    cache.second = size_t(0);
  }

  variable_size_iterator_basis(const uint8_t*,
                               const uint8_t* position,
                               const VariableSizeTypeDescriptor& dsc) 
    : base(position, dsc) {
    cache.second = size_t(0);
  }

  const typename base::state_type&
  state() { return this->st; }

  typename base::reference
  deref() const { 
    if (cache.second == size_t(0)) { 
      const_cast<variable_size_iterator_basis*>(this)->cache =
        this->st.dsc.attributes(this->st.position);
    }
    return cache.first;
  }

  void increment() { 
    this->st.position += cache.second ? cache.second : this->st.dsc.size(this->st.position);
    cache.second = size_t(0); 
  }
};

template <typename VariableSizeTypeDescriptor>
struct variable_size_iterator_basis<VariableSizeTypeDescriptor, 
                                    false, 
                                    std::bidirectional_iterator_tag>
  : variable_size_iterator_basis_base<VariableSizeTypeDescriptor>
{
private:
  typedef variable_size_iterator_basis_base<VariableSizeTypeDescriptor> base;
  std::pair<typename base::value_type, size_t> cache;
public:
  const uint8_t* origin;

  variable_size_iterator_basis() :  base(), origin(NULL) {
    cache.second = size_t(0);
  }

  variable_size_iterator_basis(const uint8_t* origin,
                               const uint8_t* position,
                               const VariableSizeTypeDescriptor& dsc) 
    : base(position, dsc), origin(origin) {
    cache.second = size_t(0);
  }

  const typename base::state_type&
  state() const { return this->st; }

  typename base::reference
  deref() const { 
    if (cache.second == size_t(0)) { 
      const_cast<variable_size_iterator_basis*>(this)->cache =
        this->st.dsc.attributes(this->st.position);
    }
    return cache.first;
  }

  void increment() { 
    this->st.position += cache.second ? cache.second : this->st.dsc.size(this->st.position);
    cache.second = size_t(0); 
  }

  void decrement() {
    cache = this->st.dsc.attributes_backward(origin, this->st.position);
    this->st.position -= cache.second;
  }
};


/* Equality and Inequality Operators */

template <typename VariableSizeTypeDescriptor,
          bool prefixed_size,
          typename IteratorCategory>
inline
bool operator==(const variable_size_iterator_basis<VariableSizeTypeDescriptor,
                                                   prefixed_size,
                                                   IteratorCategory> x,
                const variable_size_iterator_basis<VariableSizeTypeDescriptor,
                                                   prefixed_size,
                                                   IteratorCategory> y) {
  return x.st.position == y.st.position;
}

template <typename VariableSizeTypeDescriptor,
          bool prefixed_size,
          typename IteratorCategory>
inline
bool operator!=(const variable_size_iterator_basis<VariableSizeTypeDescriptor,
                                                   prefixed_size,
                                                   IteratorCategory> x,
                const variable_size_iterator_basis<VariableSizeTypeDescriptor,
                                                   prefixed_size,
                                                   IteratorCategory> y) {
  return !(x == y);
}

/* Specialized Equal */

template <typename VariableSizeTypeDescriptor, 
          bool prefixed_size,
          typename IteratorCategory>
inline
bool
equal(adapter::iterator<variable_size_iterator_basis<VariableSizeTypeDescriptor, 
                                            prefixed_size,
                                            IteratorCategory> >first1,
      adapter::iterator<variable_size_iterator_basis<VariableSizeTypeDescriptor, 
                                            prefixed_size,
                                            IteratorCategory> >last1,
      adapter::iterator<variable_size_iterator_basis<VariableSizeTypeDescriptor, 
                                            prefixed_size,
                                            IteratorCategory> >first2) {
  if(first1.state().dsc.equality_preserving) {
    return std::equal(first1.state().position, 
                      last1.state().position, 
                      first2.state().position);
  } else {
    return std::equal(first1, last1, first2);
  }
}


/* Output Iterator Basis */

template <typename WritableVariableSizeTypeDescriptor>
struct variable_size_output_iterator_basis {
  typedef WritableVariableSizeTypeDescriptor descriptor_type;
  typedef typename descriptor_type::value_type value_type;

  struct state_type {
    uint8_t* position;
    descriptor_type dsc;
    state_type(uint8_t* position, descriptor_type dsc) :
      position(position), dsc(dsc) {}
    state_type() {}
  };

  state_type st;

  variable_size_output_iterator_basis() : st(NULL, descriptor_type()) {}
  variable_size_output_iterator_basis(uint8_t* position, 
                                      const descriptor_type& dsc) : 
    st(position, dsc) {}

  const state_type& state() { return st; }

  void store(const value_type& v) { 
    st.position = st.dsc.encode(v, st.position);
  }
};


/* Specialized Copy */

template <typename WritableVariableSizeTypeDescriptor, 
          bool prefixed_size,
          typename IteratorCategory>
inline
adapter::output_iterator<variable_size_output_iterator_basis<WritableVariableSizeTypeDescriptor> >
copy(adapter::iterator<variable_size_iterator_basis<WritableVariableSizeTypeDescriptor,
                                                    prefixed_size,
                                                    IteratorCategory> > first,
     adapter::iterator<variable_size_iterator_basis<WritableVariableSizeTypeDescriptor,
                                                    prefixed_size,
                                                    IteratorCategory> > last,
     adapter::output_iterator<variable_size_output_iterator_basis<WritableVariableSizeTypeDescriptor> >
     result) {
  variable_size_output_iterator_basis<WritableVariableSizeTypeDescriptor> basis
    (std::copy(first.state().position, last.state().position, result.state().position), 
     result.state().dsc);
  return adapter::make_output_iterator(basis);
}
                                                                           
// Local Variables:
// mode: c++
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
#endif

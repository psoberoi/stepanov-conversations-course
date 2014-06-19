#ifndef TAPE_H
#define TAPE_H

#include <stdint.h>
#include <algorithm>
#include <new>

#include "extent.h"
#include "variable_size_type.h"
#include "iterator_adapter.h"
#include "variable_size_type_iterator.h"


//-----------------------------------------------------------------------
/*
template <typename WritableVariableSizeTypeDescriptor>
class tape {
public:
  typedef WritableVariableSizeTypeDescriptor descriptor_type;
  typedef variable_size_iterator_state<
    descriptor_type,
    descriptor_type::prefixed_size,
    typename descriptor_type::iterator_category> iterator_state;

  // standard container affiliated types:

  typedef typename descriptor_type::value_type value_type;
  typedef value_type reference;
  typedef value_type const_reference;
  //typedef void pointer;
  typedef iterator<iterator_state> const_iterator;
  typedef const_iterator iterator;
  typedef typename const_iterator::difference_type difference_type;
  typedef size_t  size_type;

  size_type size() const;
  // We do not provide:  max_size() -- explain why
  bool empty() const;

  const_iterator begin() const;
  const_iterator end() const;

  friend
  bool operator==(const tape& x, const tape& y);
  friend
  bool operator!=(const tape& x, const tape& y);
  friend
  bool operator<(const tape& x, const tape& y);
  friend
  bool operator>=(const tape& x, const tape& y);
  friend
  bool operator>(const tape& x, const tape& y);
  friend
  bool operator<=(const tape& x, const tape& y);

  struct back_insert_iterator; // satisfies OutputIterator requirements

  back_insert_iterator back_inserter();

  void push_back(const value_type& v);

  template <typename InputIterator>
  std::pair<const_iterator, const_iterator>
  insert(const_iterator position, InputIterator first, InputIterator last);

  const_iterator erase(const_iterator first, const_iterator last);

  ~tape();

  tape();

  tape(const tape& x);

  tape& operator=(const tape& x);

  template <typename InputIterator>
  tape(InputIterator first, InputIterator last,
       const descriptor_type& dsc = descriptor_type());

  // not implemented, use swap(x,y) :   void swap(tape& x);

  friend
  void swap(tape& x, tape& y);

// tape specific interfaces:

  descriptor_type descriptor() const;

  size_type byte_size() const;
  size_type byte_capacity() const;
  // the memory footprint of the tape x is sizeof(x) + x.total_byte_size()
  size_type total_byte_size() const;
  size_type remaining_byte_capacity() const; // invariant: byte_capacity() = byte_size() + remaining_byte_capacity()
  void adjust_byte_capacity(size_type n); // postcondition: assert(remaining_byte_capacity() == n)
  size_type capacity() const;

};
*/

/*

Most implementations of STL make vector header contain 3 pointers +
the allocator. Only one pointer is really needed as stated, for
example, in:

"In a single-extent array the extent need only be present when the
size is nonzero. To avoid reallocation at every insert, the extent
contains a reserve area; when the reserve area is exhausted, the
extent is reallocated. The header contains a pointer to the extent;
additional pointers keeping track of the data and reserve areas nor-
mally reside in a prefix of the extent. Placing the additional
pointers in the prefix and not in the header improves both space and
time complexity when arrays are nested." (EoP page 220)

It is especially important for the tape since in many cases they are
contained in a sparse vector.  This is one of the reasons that we do
not store the allocator in the header but use malloc.
*/



template <typename WritableVariableSizeTypeDescriptor>
class tape {
public:
  typedef WritableVariableSizeTypeDescriptor descriptor_type;
  typedef typename descriptor_type::value_type value_type;
  typedef value_type reference;
  typedef value_type const_reference;
  //typedef void pointer;
  typedef variable_size_iterator_basis<
    descriptor_type,
    descriptor_type::prefixed_size,
    typename descriptor_type::iterator_category> iterator_state;
  typedef adapter::iterator<iterator_state> const_iterator;
  typedef const_iterator iterator;
  typedef typename const_iterator::difference_type difference_type;
  typedef size_t  size_type;

private:
  typedef uint8_t* pointer;
  typedef const uint8_t* const_pointer;
  typedef variable_size_output_iterator_basis<descriptor_type> output_iterator_state;
  typedef adapter::output_iterator<output_iterator_state> output_iterator;

  struct tape_metadata {
    size_t number_of_elements;
  };

  extent<tape_metadata> ext;
  descriptor_type dsc;

  size_type& number_of_elements() { // only safe when non-empty
    return ext.metadata()->number_of_elements;
  }

  const size_type& number_of_elements() const { // only safe when non-empty
    return ext.metadata()->number_of_elements;
  }

  struct back_insert_iterator_basis {
    typedef tape* state_type;
    state_type tape_p;
    state_type state() { return tape_p; }

    back_insert_iterator_basis() : tape_p(NULL) {}
    back_insert_iterator_basis(tape* tape_p) : tape_p(tape_p) {}

    struct writer {
      value_type value;
      descriptor_type dsc;

      writer(const value_type& value,
             const descriptor_type& dsc) : value(value), dsc(dsc) {}
      void operator()(pointer p) {
        dsc.encode(value, p);
      }
    };

    void store(const value_type& value) {
      tape_p->ext.insert_space(tape_p->dsc.encoded_size(value),
                               writer(value, tape_p->dsc));
      ++tape_p->number_of_elements();
    }
  };

  static
  const_pointer pos(const const_iterator& x) {
    return x.state().position;
  }

  static
  pointer pos_non_const(const const_iterator& x) {
    return const_cast<pointer>(x.state().position);
  }

public:
  const extent<tape_metadata>& get_extent() const { return ext; }

  bool empty() const { return get_extent().empty(); }

  // returns the number of values currently stored in the tape
  size_type size() const { return empty() ? size_type(0) : number_of_elements(); }

  // returns (a conservative estimate of) the number of values the tape can hold
  // without reallocation
  size_type capacity() const {
    return size() + (get_extent().remaining_byte_capacity() / sizeof(value_type));
  }

  descriptor_type descriptor() const { return dsc; }

  const_iterator begin() const {
    const_pointer p = get_extent().storage();
    return const_iterator(iterator_state(p, p, dsc));
  }

  const_iterator end() const {
    const_pointer p = ext.storage();
    return const_iterator(iterator_state(p, p + get_extent().byte_size(), dsc));
  }

  friend
  inline
  bool operator==(const tape& x, const tape& y) {
    if (x.size() != y.size()) return false;
    if (x.dsc.equality_preserving &&
        x.get_extent().byte_size() != y.get_extent().byte_size()) return false;
    return equal(x.begin(), x.end(), y.begin()); // calls our equal that optimizes based on equality_preserving
  }

  friend
  inline
  bool operator!=(const tape& x, const tape& y) {
    return !(x == y);
  }

  friend
  inline
  bool operator<(const tape& x, const tape& y) {
    if (x.dsc.order_preserving) {
      const_pointer x_p = x.ext.storage();
      const_pointer y_p = y.ext.storage();
      return std::lexicographical_compare(x_p, x_p + x.get_extent().byte_size(),
                                          y_p, y_p + y.get_extent().byte_size());
    } else {
      return std::lexicographical_compare(x.begin(), x.end(),
                                          y.begin(), y.end());
    }
  }

  friend
  inline
  bool operator>=(const tape& x, const tape& y) {
    return !(x < y);
  }

  friend
  inline
  bool operator>(const tape& x, const tape& y) {
    return (y < x);
  }

  friend
  inline
  bool operator<=(const tape& x, const tape& y) {
    return !(x > y);
  }

  typedef adapter::output_iterator<back_insert_iterator_basis> back_insert_iterator;

  back_insert_iterator back_inserter() {
    return back_insert_iterator(back_insert_iterator_basis(this));
  }

  void push_back(const value_type& v) {
    *back_inserter() = v;
  }

private:
  std::pair<const_iterator, const_iterator>
  inserted_range(pointer x, pointer y) {
    return std::make_pair(const_iterator(iterator_state(ext.storage(), x, dsc)),
                          const_iterator(iterator_state(ext.storage(), y, dsc)));
  }

  template <typename InputIterator>
  std::pair<const_iterator, const_iterator>
  insert(const_iterator position, InputIterator first, InputIterator last,
         std::input_iterator_tag) {
    size_type insertion_offset(pos(position) - ext.storage());
    size_type old_byte_size(get_extent().byte_size());
    std::copy(first, last, back_inserter()); // might reallocate and invalidate position
    pointer begin_inserted_range = ext.storage() + insertion_offset;
    size_t increment = get_extent().byte_size() - old_byte_size;
    pointer end_inserted_range = begin_inserted_range + increment;
    std::rotate(begin_inserted_range, ext.storage() + old_byte_size, ext.content_end());
    return inserted_range(begin_inserted_range, end_inserted_range);
  }

  template <typename InputIterator>
  struct writer {
    InputIterator first;
    InputIterator last;
    descriptor_type dsc;
    writer(InputIterator first, InputIterator last, const descriptor_type& dsc)
      : first(first), last(last), dsc(dsc) {}
    template <typename T>
    void write(pointer p, T) {
      output_iterator result(output_iterator_state(p, dsc));
      std::copy(first, last, result);
    }
    void write(pointer p, const_iterator) {
      std::copy(pos(first), pos(last), p);
    }
    void operator()(pointer p) { write(p, first); }
  };

  template <typename InputIterator>
  std::pair<size_type, size_type>
  size_count(InputIterator first, InputIterator last) {
    return total_encoded_size(first, last, dsc);
  }


  std::pair<size_type, size_type>
  size_count(const_iterator first, const_iterator last) {
    return std::make_pair(pos(last) - pos(first), std::distance(first, last));
  }

  template <typename ForwardIterator>
  std::pair<const_iterator, const_iterator>
  insert(const_iterator position,
         ForwardIterator first, ForwardIterator last,
         std::forward_iterator_tag) {
    std::pair<size_type, size_type>  size_and_count = size_count(first, last);
    pointer insert_position = pos_non_const(position);
    writer<ForwardIterator> w(first, last, dsc);
    pointer begin_inserted = ext.insert_space(insert_position, size_and_count.first, w);
    pointer end_inserted = begin_inserted + size_and_count.first;
    if (size_and_count.second) number_of_elements() += size_and_count.second;
    return inserted_range(begin_inserted, end_inserted);
  }

public:

  void adjust_byte_capacity(size_type n) {
    ext.adjust_byte_capacity(n);
  }

  template <typename InputIterator>
  std::pair<const_iterator, const_iterator>
  insert(const_iterator position, InputIterator first, InputIterator last) {
    typename std::iterator_traits<InputIterator>::iterator_category tag;
    return insert(position, first, last, tag);
  }

  const_iterator erase(const_iterator first, const_iterator last) {
    size_t number_of_erased_elements = size_t(std::distance(first, last));
    size_t size_of_erased_elements = pos(last) - pos(first);
    if (ext.erase_space(pos_non_const(first), size_of_erased_elements)) {
      number_of_elements() -= number_of_erased_elements;
      return first;
    } else {
      return const_iterator();
    }
  }

  tape(const descriptor_type& dsc = descriptor_type())
    : dsc(dsc) {}

  template <typename InputIterator>
  tape(InputIterator first, InputIterator last,
       const descriptor_type& dsc = descriptor_type())
  : dsc(dsc) {
    insert(end(), first, last);
  }

  friend
  void swap(tape& x, tape& y) {
    std::swap(x.ext, y.ext);
  }

};

// Local Variables:
// mode: c++
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
#endif

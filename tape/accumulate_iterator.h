
// any iterator value must be dereferenced before it is incremented
// otherwise dereferencing will not return correct result

template <typename InputIterator> 
struct accumulate_iterator {
  typedef accumulate_iterator self;

  typedef typename std::input_iterator_tag iterator_category; 
  typedef typename InputIterator::value_type value_type;
  typedef ptrdiff_t difference_type;
  typedef value_type reference;
  typedef void pointer;

  InputIterator position;
  value_type total;
  value_type cache; // the current incremental value

  friend bool
  operator==(const self& x, const self& y) {
    return x.position == y.position;
  }
  
  friend bool
  operator!=(const self& x, const self& y) {
    return !(x == y);
  } 

  reference operator*() const {
    const_cast<self*>(this)->cache = *position;
    return total + cache;
  }

  self& operator++() {
    total += cache;
    ++position;
    return *this;
  }

  self operator++(int) {
    self tmp = *this;
    ++*this;
    return tmp;
  }

  accumulate_iterator() {}
  accumulate_iterator(InputIterator position, 
		      value_type initial = value_type(0))
    : position(position), total(initial) {}
};




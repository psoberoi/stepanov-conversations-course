#ifndef VARIABLE_SIZE_TYPE_H
#define VARIABLE_SIZE_TYPE_H

/*
  concept VariableSizeTypeDescriptor<SemiRegular X>
  =  SemiRegular<X::value_type>
  // X::value_type is a type encoded by variable-size datum
  && IteratorCategory<X::iterator_category>
    // is std::bidirectional_iterator_tag if X satisfies BidirectionalVariableSizeTypeDescriptor
    // is std::forward_iterator_tag otherwise
  && requires(X a,
              X::value_type v,
              const uint8_t* src,
              const uint8_t* dst) {
       // to decode value from datum:
       value_type { a.decode(src) };
       // to compute the encoded size of datum at src:
       size_t     { a.size(src) };
       // to compute the encoded size of a given value:
       size_t     { a.encoded_size(v) };
       // to allow compile-time dispatch without concept support:
       bool { X::prefixed_size };
       // !X::prefixed_size => UnprefixedVariableSizeTypeDescriptor<X>
       bool { X::equality_preserving };
       bool { X::order_preserving };
       axiom { a.size(src) > 0 }
       axiom { a.encoded_size(v) > 0 }
       axiom { a.equality_preserving =>
                 (a.decode(p1) == a.decode(p2) <=>
                    (a.size(p1) == a.size(p2) &&
                     std::equal(p1, p1 + a.size(p1), p2))) }
       axiom { a.order_preserving =>
                 (a.decode(p1) < a.decode(p2) <=>
                    std::lexicographical_compare(p1, p1 + a.size(p1),
                                                 p2, p2 + a.size(p2))) }

      };

  concept UnprefixedVariableSizeTypeDescriptor<VariableSizeTypeDescriptor X>
  =  requires (X a, const uint8_t* p) {
       std::pair<X::value_type, size_t> { a.attributes(p) };
       axiom { X::prefixed_size == false }
       axiom { a.attributes(p) == std::make_pair(a.decode(p), a.size(p)) }
       // attributes may be faster than computing decode and size separately
     };

  concept BidirectionalVariableSizeTypeDescriptor<VariableSizeTypeDescriptor X>
  =  requires (X a, const uint8_t* origin, const uint8_t* current) {
       const uint8_t* { a.previous(origin, current) };
       std::pair<X::value_type, size_t> { a.attributes_backward(origin, current) };
       axiom { a.attributes_backward(origin, current) ==
          std::make_pair(a.decode(a.previous(origin, current)), a.size(a.previous(origin, current))) }
       // attributes_backward may be faster than computing decode and size separately
     };

  concept WritableVariableSizeTypeDescriptor<VariableSizeTypeDescriptor X>
  =  requires (X a, const value_type& v, const uint8_t* src, uint8_t* dst) {
       uint8_t* { a.encode(v, dst) }; // encodes v to dst
                                      // returning the position following
                                      // the written encoding
       std::pair<const uint8_t*, uint8_t*> { a.copy(src, dst) };
                                     // copies a single value at src to dst,
                                     // returning the pair of positions
                                     // following the read and written encodings
       // copy may be faster than decode followed by encode
     };

*/


#include <stddef.h>

template <typename InputIterator, typename VariableSizeTypeDescriptor>
std::pair<size_t, size_t>
total_encoded_size(InputIterator first, InputIterator last,
                   const VariableSizeTypeDescriptor& dsc) {
  size_t result(0);
  size_t n(0);
  while (first != last) {
    result += dsc.encoded_size(*first++);
    ++n;
  }
  return std::make_pair(result, n);
}


// Local Variables:
// mode: c++
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
#endif

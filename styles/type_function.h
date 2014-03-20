#ifndef _TYPE_FUNCTION_H_
#define _TYPE_FUNCTION_H_

#include <iterator>

template <typename I> // I is InputIterator
using IteratorCategory = typename std::iterator_traits<I>::iterator_category;

template <typename I> // I is InputIterator
using ValueType = typename std::iterator_traits<I>::value_type;

template <typename I> // I is InputIterator
using DifferenceType = typename std::iterator_traits<I>::difference_type;

template <typename I> // I is InputIterator
using PointerType = typename std::iterator_traits<I>::pointer;

template <typename I> // I is InputIterator
using ReferenceType = typename std::iterator_traits<I>::reference;

#endif

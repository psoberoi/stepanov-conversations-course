#ifndef EXTENT_H
#define EXTENT_H

#include <stdint.h>
#include <algorithm>
#include <new>

/*

concept Copier<typename X>
= Semiregular<X>
&& requires (X x, const uint8_t* first, const uint8_t* last, uint8_t* result, uint8_t* result_end) {
       void {  x.copy(first, last, result)  };
       void {  x.move(first, last, result)  };
       void {  x.move_backward(first, last, result) };
       void {  x.clean_up(result, result_end) };
   }
};

concept Metadata<typename X> = Semiregular<X>;

The copier concept is here to allow possible generealization in the future.
For example, we could use extent to re-implement std::vector replacing byte_copier
with a copier templatized on T

We can also re-architect extent to support different allocation policies - see EoP page 220

*/



struct byte_copier {
  void copy(const uint8_t* first, const uint8_t* last, uint8_t* result) {
    std::copy(first, last, result);
  }
  void move(const uint8_t* first, const uint8_t* last, uint8_t* result) {
    std::copy(first, last, result);
  }
  void move_backward(const uint8_t* first, const uint8_t* last, uint8_t* result) {
    std::copy_backward(first, last, result);
  }
  void clean_up(uint8_t*, uint8_t*) {}
};

template <typename Metadata, typename Copier = byte_copier>
struct extent {

private:
  typedef uint8_t* pointer;
  typedef const uint8_t* const_pointer;
  typedef extent self;

  pointer start; 

  pointer guarded_malloc(size_t n) {
    void* tmp = malloc(n);
    if (tmp == NULL) throw std::bad_alloc();
    return pointer(tmp);
  }

  struct header_t {
    size_t finish;
    size_t end_of_block;
    Metadata metadata;
  };

  header_t* header() { 
    return (header_t*)(start) - ptrdiff_t(1);
  }

  const header_t* header() const { 
    return (header_t*)(start) - ptrdiff_t(1);
  }

  size_t& end_of_block() { 
    return header()->end_of_block; 
  }

  const size_t& end_of_block() const { 
    return header()->end_of_block; 
  }

  size_t& finish() { 
    return header()->finish; 
  }

  const size_t& finish() const { 
    return header()->finish; 
  }

public:

  pointer storage() { return start; } 

  const_pointer storage() const { return start; }
 
  Metadata* metadata() {
    return start ? &(header()->metadata) : NULL;
  }

  const Metadata* metadata() const {
    return start ? &(header()->metadata) : NULL;
  }

  // returns the size of the contents in bytes
  size_t byte_size() const { 
    return start ? finish() : size_t(0); 
  }

  pointer content_end() { return storage() + byte_size(); } 

  const_pointer content_end() const { return storage() + byte_size(); }

  // returns the total current data-holding capacity of the tape in bytes
  size_t byte_capacity() const { 
    return start ? end_of_block() : size_t(0); 
  }

  // returns the size in bytes of the allocated extent
  size_t total_byte_size() const { 
    return start ? byte_capacity() + sizeof(header_t) : size_t(0); 
  }

  // returns the remaining capacity for data in bytes
  size_t remaining_byte_capacity() const {
    return byte_capacity() - byte_size();
  }
  
  // returns true if and only if the extent is empty
  bool empty() const { return !start; }

private:
  pointer new_block_start(size_t additional) {
    size_t increment = std::max(byte_capacity(), additional);
    size_t new_capacity = byte_capacity() + increment;
    pointer block = guarded_malloc(sizeof(header_t) + new_capacity);

    header_t* p_header = (header_t*)(block);
    p_header->metadata = start ? *metadata() : Metadata();   
    p_header->finish = byte_size();
    p_header->end_of_block = new_capacity;

    return block + sizeof(header_t);
  }

  void replace_start(pointer new_start) {
    pointer old_start = start;
    start = new_start;
    if (old_start) free((void*)(old_start - sizeof(header_t)));
  }

  void deallocate() { replace_start(NULL); }

  void reallocate(size_t additional) {
    pointer new_start = new_block_start(additional);
    if (start) Copier().move(storage(), content_end(), new_start);
    replace_start(new_start);
  }

  void reallocate(size_t additional, size_t offset) {
    if (additional == 0) return;
    pointer new_start = new_block_start(additional);
    if (start) {
      Copier().move(start, start + offset, new_start); 
      Copier().move(start + offset, content_end(), new_start + offset + additional);
    } 
    replace_start(new_start);
  }

public:

  void adjust_byte_capacity(size_t n) {
    if (remaining_byte_capacity() != n) {
      self tmp;
      tmp.reallocate(byte_size() + n);
      Copier().copy(start, content_end(), tmp.start);
      tmp.finish() = byte_size();
      *tmp.metadata() = start ? *metadata() : Metadata(); 
      std::swap(start, tmp.start);
    }
    // postcondition: assert(remaining_byte_capacity() == n)
  }


  template <typename Writer>
  pointer insert_space(pointer position, size_t inserted_byte_size, Writer writer) {
    if (!inserted_byte_size) return position;
    size_t offset(position - start);
    if (remaining_byte_capacity() < inserted_byte_size) {
      reallocate(inserted_byte_size, offset);
    } else {
      pointer new_finish_p = content_end() + inserted_byte_size;
      Copier().move_backward(start + offset, content_end(), new_finish_p);
    }
    writer(start + offset);
    finish() += inserted_byte_size;
    return start + offset;
  }

  template <typename Writer>
  pointer insert_space(size_t inserted_byte_size, Writer writer) {
    if (!inserted_byte_size) return content_end();
    if (remaining_byte_capacity() < inserted_byte_size) {
      reallocate(inserted_byte_size);
    } 
    pointer write_point = content_end();
    writer(write_point);
    finish() += inserted_byte_size;
    return write_point;
  }


  // Done in a way that is safe for concurrent readers if the erase is at the end.
  pointer erase_space(pointer first, size_t erased_byte_size) {
    if (erased_byte_size) {
      pointer last = first + erased_byte_size;
      size_t new_byte_size = byte_size() - erased_byte_size;
      pointer old_content_end = content_end();
      finish() = new_byte_size;
      if (!new_byte_size) {
        Copier().clean_up(first, last);
        deallocate();
        return NULL;
      } else {
        Copier().move(last, old_content_end, first);
        if (content_end() < last) Copier().clean_up(content_end(), last);
      }
    }
    return first;
  }

  ~extent() {  deallocate(); }

  extent() : start(NULL) {}

  extent(const self& x) : start(NULL) {
    if (x.start) {
      start = new_block_start(x.byte_size());
      Copier().copy(x.storage(), x.content_end(), start);
      finish() = x.byte_size();
      *metadata() = *x.metadata();
    }
  }

  friend
  void swap(self& x, self& y) {
    std::swap(x.start, y.start);
  }

  self& operator=(const self& x) {
    if (&x != this) {
      self tmp(x);
      swap(*this, tmp);
    }
    return *this;
  }
};

// Local Variables:
// mode: c++
// c-basic-offset: 2
// indent-tabs-mode: nil
// End:
#endif

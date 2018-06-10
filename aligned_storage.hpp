#ifndef ALIGNED_STORAGE_HPP
#define ALIGNED_STORAGE_HPP

#include "alignment_of.hpp"
#include "type_with_alignment.hpp"

namespace my {
namespace type_traits {

  template <std::size_t Size, std::size_t Alignment>
  struct aligned_storage
  {
    union dummy_u
    {
      char data[ Size ];
      typename type_with_alignment<Alignment>::type aligner;
    } dummy_;

    void* address() { return &dummy_.data[0]; }
    const void* address() const { return &dummy_.data[0]; }
    
    // TODO static assert on the storage is aligned property
  };

}}

#endif


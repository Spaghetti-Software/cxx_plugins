/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of cxx_plugins project.
 * License is available at
 *https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    mallocator.hpp
 * \author  Timur Kazhimuratov
 * \date    12 May 2020
 * \brief
 * This file contains the interface for mallocator class.
 *
 */
#pragma once

#include <cassert>
#include <cxx_plugins/memory/memory_common.hpp>
#include <cstdlib>
#include <cstddef>
#include <new>

namespace utility {

class Mallocator {
public:
  Mallocator() = default;

  ~Mallocator() = default;

  mem_block allocate(std::size_t n, std::size_t alignment = 4) {
    if (n == 0)
      throw std::bad_alloc(); // TODO: replace with out custom exception

    const auto alignedSize = roundLengthToAlignment(n, alignment);
#ifdef _MSC_VER
    void *ptr = _aligned_malloc(alignedSize, alignment);
#else
    void *ptr = std::aligned_alloc(alignment, alignedSize);
#endif
    
    if (ptr == nullptr)
      throw std::bad_alloc(); // TODO: replace with out custom exception

    return mem_block{ptr, alignedSize};
  }

  void deallocate(mem_block block) {
    assert(block.ptr != nullptr);

    if (!owns(block))
      return;
#ifdef _MSC_VER
    _aligned_free(block.ptr);
#else
    std::free(block.ptr);
#endif
  }

  void deallocateAll() {
    return;  // TODO: deallocate all pointers returned by this allocator.
             // Probably will involve going over the container of allocated blocks and deallocating the pointers.
  }

  bool owns(mem_block block) const noexcept {
    return true; // TODO: actually check if it was returned by malloc (check in the container of allocated blocks)
  }
private:

};

} // namespace utility 

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

  mem_block allocate(size_t n) {
    if (n == 0)
      throw std::bad_alloc(); // TODO: replace with out custom exception

    void *ptr = malloc(n);
    if (ptr == nullptr)
      throw std::bad_alloc(); // TODO: replace with out custom exception

    return mem_block{ptr, n};
  }

  void deallocate(mem_block block) {
    assert(block.ptr != nullptr);

    if (!owns(block))
      return;
    
    free(block.ptr);
  }

  void deallocateAll() {
    return;  // TODO: deallocate all pointers returned by this allocator.
             // Probably will involve going over the container of allocated blocks and deallocating the pointers.
  }

  bool owns(mem_block block) {
    return true; // TODO: actually check if it was returned by malloc (check in the container of allocated blocks)
  }
private:

};

} // namespace utility 

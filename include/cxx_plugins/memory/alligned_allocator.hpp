/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of cxx_plugins project.
 * License is available at
 *https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    stack_allocator.hpp
 * \author  Timur Kazhimuratov
 * \date    11 May 2020
 * \brief
 * This file contains the interface for an alligned allocator.
 *
 */
#pragma once

#include "memory_common.hpp"
#include <memory>

namespace utility {

template <class Allocator, size_t allignment = 4> class AllignedAllocator {
public:
  AllignedAllocator() = default;
  ~AllignedAllocator() = default;

  mem_block allocate(size_t n) {
    if (n == 0)
      return {nullptr, 0};
    if (n <= allignment)
      return allocator_.allocate(allignment);
    return allocator_.allocate(n + (allignment - n % allignment));
  }

  void deallocate(mem_block block) { allocator_.deallocate(block); }

  void deallocateAll() { allocator_.deallocateAll(); }

  bool owns(mem_block block) { return allocator_.owns(block); }
private:
  Allocator allocator_;
};

} // namespace utility 
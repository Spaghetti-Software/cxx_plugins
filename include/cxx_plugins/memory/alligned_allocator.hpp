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

#include <cxx_plugins/memory/memory_common.hpp>
#include <cstddef>
#include <memory>
#include <new>

namespace utility {

template <class Allocator, size_t allignment = 4> class AllignedAllocator {
public:
  AllignedAllocator() = default;
  ~AllignedAllocator() = default;

  mem_block allocate(size_t n) {
    if (n == 0)
      throw std::bad_alloc();
    mem_block block;
    if (n <= allignment) {
      block = m_allocator.allocate(allignment);
    }
    else {
      block = m_allocator.allocate(n + (allignment - n % allignment));
    }
      
    return block;
  }

  void deallocate(mem_block block) { m_allocator.deallocate(block); }

  void deallocateAll() { m_allocator.deallocateAll(); }

  bool owns(mem_block block) { return m_allocator.owns(block); }

private:
  Allocator m_allocator;
};

} // namespace utility 
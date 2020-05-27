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
 * This file contains the interface for a stack allocator.
 *
 */
#pragma once

#include <cassert>
#include <cxx_plugins/memory/memory_common.hpp>
#include <cstddef>
#include <cstdint>
#include <new>

namespace utility {

template <std::size_t S>
class StackAllocator {
public:
  constexpr StackAllocator() noexcept : m_stack(), m_p(m_stack) {}

  ~StackAllocator() = default;

  constexpr mem_block allocate(std::size_t n, std::size_t alignment = 4) {
    if (n == 0)
      throw std::bad_alloc(); // TODO: replace with out custom exception

    const auto alignedSize = roundLengthToAlignment(n, alignment);
    const auto mod = reinterpret_cast<std::uint64_t>(m_p) % alignment;
    const auto offset = mod ? alignment - mod : 0;
    auto *alignedPtr = m_p + offset;
    if (m_stack + S < alignedPtr + alignedSize) 
      throw std::bad_alloc(); // out of stack memory. TODO: replace with out custom exception

    m_p = alignedPtr + alignedSize;
    return mem_block{alignedPtr, alignedSize}; 
  }

  constexpr void deallocate(mem_block block) {
    assert(block.ptr != nullptr);

    if (!owns(block)) {
      return; // pointer provided was not allocated by this allocator
    }
    char *ptr = reinterpret_cast<char *>(block.ptr);
    if (ptr == m_p - block.size)
      m_p -= block.size; // deallocating last allocation
  }

  constexpr void deallocateAll() {
    m_p = m_stack;
  }

  constexpr bool owns(mem_block block) const noexcept {
    char *ptr = reinterpret_cast<char *>(block.ptr);
    return ptr >= m_stack && ptr < m_stack + S;
  }

private:
  char m_stack[S];
  char *m_p;
};

} // namespace utility

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
  constexpr StackAllocator() noexcept : stack_m(), p_m(stack_m) {}

  ~StackAllocator() = default;

  constexpr void* allocate(std::size_t bytes, std::size_t alignment = alignof(std::max_align_t)) {
    if (bytes == 0)
      throw std::bad_alloc(); // TODO: replace with out custom exception

    const auto mod = reinterpret_cast<std::uint64_t>(p_m) % alignment;
    const auto offset = mod ? alignment - mod : 0;
    auto *alignedPtr = p_m + offset;
    if (stack_m + S < alignedPtr + bytes) 
      throw std::bad_alloc(); // out of stack memory. TODO: replace with out custom exception

    p_m = alignedPtr + bytes;
    return alignedPtr; 
  }

  constexpr void deallocate(void* p, std::size_t bytes, std::size_t alignment = alignof(std::max_align_t)) {
    assert(p != nullptr);

    if (p < stack_m || p >= stack_m + max_size_s) {
      return; // pointer provided was not allocated by this allocator
    }
    char *ptr = reinterpret_cast<char *>(p);
    if (ptr == p_m - bytes)
      p_m -= bytes; // deallocating last allocation
  }

  bool is_equal(const StackAllocator &other) const noexcept {
    return stack_m == other.stack_m;
  }

private:
  static constexpr std::size_t max_size_s = S;

  char stack_m[S];
  char *p_m;
};

} // namespace utility

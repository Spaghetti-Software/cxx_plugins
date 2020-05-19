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

#include "memory_common.hpp"

namespace utility {

template <size_t S>
class StackAllocator {
public:
  constexpr StackAllocator() noexcept
    : stack_(), p_(stack_) {}

  ~StackAllocator() = default;

  constexpr mem_block allocate(size_t n) {
    if (n == 0)
      return {nullptr, 0};

    if (stack_ + S < p_ + n) 
      return {nullptr, 0}; // out of stack memory
    auto *ret = p_;
    p_ += n;
    return {ret, n}; 
  }

  constexpr void deallocate(mem_block block) {
    if (!owns(block)) {
      return; // pointer provided was not allocated by this allocator
    }
    char *ptr = reinterpret_cast<char *>(block.ptr);
    if (ptr == p_ - block.size)
      p_ -= block.size; // deallocating last allocation
  }

  constexpr void deallocateAll() {
    p_ = stack_;
  }

  constexpr bool owns(mem_block block) const noexcept {
    char *ptr = reinterpret_cast<char *>(block.ptr);
    return ptr >= stack_ && ptr < stack_ + S;
  }

private:
  char stack_[S];
  char *p_;
};

} // namespace utility

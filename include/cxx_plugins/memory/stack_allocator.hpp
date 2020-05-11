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

template <size_t s>
class stack_allocator
{
public:
    constexpr stack_allocator() noexcept : p(stack) {}
    constexpr ~stack_allocator() {}
    constexpr decltype(auto) allocate(size_t n)
    {
        return mem_block{nullptr, 0};
    }
    constexpr void deallocate(mem_block block)
    {
    }
    constexpr void deallocate_all()
    {
    }
    constexpr bool owns(mem_block block) const noexcept
    {
        return true;
    }
private:
    char stack[s];
    char* p;
};
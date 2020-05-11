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
 * This file contains the interface for an allocator that can use any other allocator.
 *
 */
#pragma once

#include "memory_common.hpp"

template<typename Allocator>
class any_allocator
{
public:
    template<typename... Args>
    constexpr any_allocator(Args... args) : a(std::forward<Args>(args)...) {}
    constexpr ~any_allocator() {}

    constexpr decltype(auto) allocate(size_t n)
    {
        return a.allocate(n);
    }
    constexpr void deallocate(mem_block block)
    {
        a.deallocate(block);
    }
    constexpr void deallocate_all()
    {
        a.deallocate_all();
    }
    constexpr bool owns(mem_block block) const noexcept
    {
        return a.owns(block);
    }
private:
    Allocator a;
};
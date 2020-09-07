/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev
 * This file is part of ECSFramework project.
 * License is available at
 * https://gitlab.com/andrey.ponomarev.1408/ecsframework
 *************************************************************************************************/
/*!
 * \file    set.hpp
 * \author  Andrey Ponomarev
 * \date    07 Sep 2020
 * \brief
 * Contains alias for std::set with our PolymorphicAllocator
 */
#pragma once

#include "cxx_plugins/polymorphic_allocator.hpp"

#include <set>

namespace plugins {

template <typename Key, typename Compare = std::less<Key>,
          typename Allocator = PolymorphicAllocator<Key>>
//! \brief Alias for std::set
using Set = std::set<Key, Compare, Allocator>;

template <typename Key, typename Compare = std::less<Key>,
    typename Allocator = PolymorphicAllocator<Key>>
//! \brief Alias for std::multiset
using Multiset = std::multiset<Key, Compare, Allocator>;

} // namespace plugins

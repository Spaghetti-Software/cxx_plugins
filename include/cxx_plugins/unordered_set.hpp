/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev
 * This file is part of ECSFramework project.
 * License is available at
 * https://gitlab.com/andrey.ponomarev.1408/ecsframework
 *************************************************************************************************/
/*!
 * \file    unordered_set.hpp
 * \author  Andrey Ponomarev
 * \date    07 Sep 2020
 * \brief
 * Contains alias for std::unordered_set with our PolymorphicAllocator
 */
#pragma once

#include "cxx_plugins/polymorphic_allocator.hpp"

#include <unordered_set>

namespace plugins {

template <typename Key, typename Hash = std::hash<Key>,
          typename KeyEqual = std::equal_to<Key>,
          typename Allocator = PolymorphicAllocator<Key>>
//! \brief Alias for std::unordered_set
using UnorderedSet = std::unordered_set<Key, Hash, KeyEqual, Allocator>;

template <typename Key, typename Hash = std::hash<Key>,
          typename KeyEqual = std::equal_to<Key>,
          typename Allocator = PolymorphicAllocator<Key>>
//! \brief Alias for std::unordered_multiset
using UnorderedMultiset =
    std::unordered_multiset<Key, Hash, KeyEqual, Allocator>;

} // namespace plugins

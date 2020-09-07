/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev
 * This file is part of ECSFramework project.
 * License is available at
 * https://gitlab.com/andrey.ponomarev.1408/ecsframework
 *************************************************************************************************/
/*!
 * \file    unordered_map.hpp
 * \author  Andrey Ponomarev
 * \date    07 Sep 2020
 * \brief
 * Contains alias for std::unordered_map with our PolymorphicAllocator
 */
#pragma once

#include "cxx_plugins/polymorphic_allocator.hpp"

#include <unordered_map>

namespace plugins {

template <typename Key, typename T, typename Hash = std::hash<Key>,
          typename KeyEqual = std::equal_to<Key>,
          typename Allocator = PolymorphicAllocator<std::pair<Key const, T>>>
//! \brief Alias for std::unordered_map
using UnorderedMap = std::unordered_map<Key, T, Hash, KeyEqual, Allocator>;

template <typename Key, typename T, typename Hash = std::hash<Key>,
          typename KeyEqual = std::equal_to<Key>,
          typename Allocator = PolymorphicAllocator<std::pair<Key const, T>>>
//! \brief Alias for std::unordered_multimap
using UnorderedMultiMap =
    std::unordered_multimap<Key, T, Hash, KeyEqual, Allocator>;

} // namespace plugins

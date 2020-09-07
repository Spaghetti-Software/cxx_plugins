/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev
 * This file is part of ECSFramework project.
 * License is available at
 * https://gitlab.com/andrey.ponomarev.1408/ecsframework
 *************************************************************************************************/
/*!
 * \file    map.hpp
 * \author  Andrey Ponomarev
 * \date    07 Sep 2020
 * \brief
 * Contains alias for std::map with our PolymorphicAllocator
 */
#pragma once

#include "cxx_plugins/polymorphic_allocator.hpp"

#include <map>

namespace plugins {

template <typename Key, typename T, typename Compare = std::less<Key>,
          typename Allocator = PolymorphicAllocator<std::pair<Key const, T>>>
//! \brief Alias for std::map
using Map = std::map<Key, T, Compare, Allocator>;

template <typename Key, typename T, typename Compare = std::less<Key>,
    typename Allocator = PolymorphicAllocator<std::pair<Key const, T>>>
//! \brief Alias for std::multimap
using Multimap = std::multimap<Key, T, Compare, Allocator>;

} // namespace plugins

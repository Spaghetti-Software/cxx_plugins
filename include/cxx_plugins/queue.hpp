/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev
 * This file is part of ECSFramework project.
 * License is available at
 * https://gitlab.com/andrey.ponomarev.1408/ecsframework
 *************************************************************************************************/
/*!
 * \file    queue.hpp
 * \author  Andrey Ponomarev
 * \date    07 Sep 2020
 * \brief
 * Contains alias for std::queue with our PolymorphicAllocator
 */
#pragma once

#include "cxx_plugins/deque.hpp"

#include <queue>

namespace plugins {

template <typename T, typename Container = Deque<T>>
//! \brief Alias for std::queue
using Stack = std::queue<T, Container>;

template <typename T, typename Container = Deque<T>, typename Compare = std::less<T>>
//! \brief Alias for std::queue
using PriorityQueue = std::priority_queue<T, Container, Compare>;

} // namespace plugins

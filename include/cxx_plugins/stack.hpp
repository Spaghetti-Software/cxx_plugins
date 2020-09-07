/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev
 * This file is part of ECSFramework project.
 * License is available at
 * https://gitlab.com/andrey.ponomarev.1408/ecsframework
 *************************************************************************************************/
/*!
 * \file    stack.hpp
 * \author  Andrey Ponomarev
 * \date    07 Sep 2020
 * \brief
 * Contains alias for std::stack with our PolymorphicAllocator
 */
#pragma once

#include "cxx_plugins/deque.hpp"

#include <stack>

namespace plugins {

template <typename T, typename Container = Deque<T>>
//! \brief Alias for std::stack
using Stack = std::stack<T, Container>;

} // namespace plugins
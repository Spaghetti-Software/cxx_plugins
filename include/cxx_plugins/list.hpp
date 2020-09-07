/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev
 * This file is part of ECSFramework project.
 * License is available at
 * https://gitlab.com/andrey.ponomarev.1408/ecsframework
 *************************************************************************************************/
/*!
 * \file    list.hpp
 * \author  Andrey Ponomarev
 * \date    07 Sep 2020
 * \brief
 * Contains alias for std::list with our PolymorphicAllocator
 */
#pragma once

#include "cxx_plugins/polymorphic_allocator.hpp"

#include <list>

namespace plugins {

template <typename T, typename Allocator = PolymorphicAllocator<T>>
//! \brief Alias for std::list
using List = std::list<T, Allocator>;

} // namespace plugins

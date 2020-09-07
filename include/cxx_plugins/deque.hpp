/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev
 * This file is part of ECSFramework project.
 * License is available at
 * https://gitlab.com/andrey.ponomarev.1408/ecsframework
 *************************************************************************************************/
/*!
 * \file    deque.hpp
 * \author  Andrey Ponomarev
 * \date    07 Sep 2020
 * \brief
 * Contains alias for std::deque with our PolymorphicAllocator
 */
#pragma once

#include "polymorphic_allocator.hpp"


#include <deque>

namespace plugins {
  template<typename T, typename Allocator = PolymorphicAllocator<T>>
  //! \brief Alias for std::deque
  using Deque = std::deque<T, Allocator>;
} // namespace plugins
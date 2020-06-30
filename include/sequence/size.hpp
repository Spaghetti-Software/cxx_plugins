/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    size.hpp
 * \author  Andrey Ponomarev
 * \date    08 Jun 2020
 * \brief
 * $BRIEF$
 */
#pragma once

#include <utility>

namespace Sequence {

template<typename S>
struct Size;

template<typename T, T... vals>
struct Size<std::integer_sequence<T,vals...>> {
  static constexpr std::size_t value = sizeof...(vals);
};

template<typename S>
static constexpr std::size_t size_v = Size<S>::value;

} // namespace Sequence
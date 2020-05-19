/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    type_traits.hpp
 * \author  Andrey Ponomarev
 * \date    15 May 2020
 * \brief
 * Contains helper functions/types to simplify template programming
 */
#pragma once

#include <type_traits>

//! \brief Contains helper functions/classes
namespace CxxPlugins::utility {
template <typename T, typename... Ts> struct Index;

template <typename T> struct Index<T> {
  static_assert(sizeof(T) != 0, "Index is out of bounds");
};

template <typename T, typename... Ts>
struct Index<T, T, Ts...> : std::integral_constant<std::size_t, 0> {};

template <typename T, typename U, typename... Ts>
struct Index<T, U, Ts...>
    : std::integral_constant<std::size_t, 1 + Index<T, Ts...>::value> {};

template <typename... Types> struct AreUnique;

template <typename First, typename... Rest> struct AreUnique<First, Rest...> {
  static constexpr bool value = (!std::is_same_v<First, Rest> && ...);
};

template <typename... Types>
static constexpr bool are_unique_v = AreUnique<Types...>::value;
} // namespace CxxPlugins::utility

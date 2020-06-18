/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    add.hpp
 * \author  Andrey Ponomarev
 * \date    08 Jun 2020
 * \brief
 * $BRIEF$
 */
#pragma once

#include <utility>

namespace Sequence {
template <typename T, T lhs_val, T rhs_val>
static constexpr auto
operator+(std::integral_constant<T, lhs_val> /*unused*/,
          std::integral_constant<T, rhs_val> /*unused*/) noexcept {
  return std::integral_constant<T, lhs_val + rhs_val>();
}

template <typename T, T lhs_val, T rhs_val>
static constexpr auto
operator-(std::integral_constant<T, lhs_val> /*unused*/,
          std::integral_constant<T, rhs_val> /*unused*/) noexcept {
  return std::integral_constant<T, lhs_val - rhs_val>();
}

template <typename T, T lhs_val, T rhs_val>
static constexpr auto
operator*(std::integral_constant<T, lhs_val> /*unused*/,
          std::integral_constant<T, rhs_val> /*unused*/) noexcept {
  return std::integral_constant<T, lhs_val * rhs_val>();
}

template <typename T, T lhs_val, T rhs_val>
static constexpr auto
operator/(std::integral_constant<T, lhs_val> /*unused*/,
          std::integral_constant<T, rhs_val> /*unused*/) noexcept {
  return std::integral_constant<T, lhs_val / rhs_val>();
}

template <typename T, T lhs_val, T rhs_val>
static constexpr auto
operator%(std::integral_constant<T, lhs_val> /*unused*/,
          std::integral_constant<T, rhs_val> /*unused*/) noexcept {
  return std::integral_constant<T, lhs_val % rhs_val>();
}

template <typename T, T... lhs_val, T... rhs_val>
static constexpr auto
operator+(std::integer_sequence<T, lhs_val...> /*unused*/,
          std::integer_sequence<T, rhs_val...> /*unused*/) noexcept {
  static_assert(sizeof...(lhs_val) == sizeof...(rhs_val),
                "Sizes of sequences should be same");
  return std::integer_sequence<T, (lhs_val + rhs_val)...>();
}

template <typename T, T... lhs_val, T... rhs_val>
static constexpr auto
operator-(std::integer_sequence<T, lhs_val...> /*unused*/,
          std::integer_sequence<T, rhs_val...> /*unused*/) noexcept {
  static_assert(sizeof...(lhs_val) == sizeof...(rhs_val),
                "Sizes of sequences should be same");
  return std::integer_sequence<T, (lhs_val - rhs_val)...>();
}

template <typename T, T... lhs_val, T... rhs_val>
static constexpr auto
operator*(std::integer_sequence<T, lhs_val...> /*unused*/,
          std::integer_sequence<T, rhs_val...> /*unused*/) noexcept {
  static_assert(sizeof...(lhs_val) == sizeof...(rhs_val),
                "Sizes of sequences should be same");
  return std::integer_sequence<T, (lhs_val * rhs_val)...>();
}

template <typename T, T... lhs_val, T... rhs_val>
static constexpr auto
operator/(std::integer_sequence<T, lhs_val...> /*unused*/,
          std::integer_sequence<T, rhs_val...> /*unused*/) noexcept {
  static_assert(sizeof...(lhs_val) == sizeof...(rhs_val),
                "Sizes of sequences should be same");
  return std::integer_sequence<T, (lhs_val / rhs_val)...>();
}

template <typename T, T... lhs_val, T... rhs_val>
static constexpr auto
operator%(std::integer_sequence<T, lhs_val...> /*unused*/,
          std::integer_sequence<T, rhs_val...> /*unused*/) noexcept {
  static_assert(sizeof...(lhs_val) == sizeof...(rhs_val),
                "Sizes of sequences should be same");
  return std::integer_sequence<T, (lhs_val % rhs_val)...>();
}

template <typename Lhs, typename Rhs> struct Plus {
  using type = decltype(std::declval<Lhs>() + std::declval<Rhs>());
};

template <typename Lhs, typename Rhs> struct Minus {
  using type = decltype(std::declval<Lhs>() - std::declval<Rhs>());
};

template <typename Lhs, typename Rhs> struct Multiply {
  using type = decltype(std::declval<Lhs>() * std::declval<Rhs>());
};

template <typename Lhs, typename Rhs> struct Divide {
  using type = decltype(std::declval<Lhs>() / std::declval<Rhs>());
};

template <typename Lhs, typename Rhs> struct Modulo {
  using type = decltype(std::declval<Lhs>() % std::declval<Rhs>());
};

template <typename Lhs, typename Rhs>
using PlusT = typename Plus<Lhs, Rhs>::type;

template <typename Lhs, typename Rhs>
using MinusT = typename Minus<Lhs, Rhs>::type;

template <typename Lhs, typename Rhs>
using MultiplyT = typename Multiply<Lhs, Rhs>::type;

template <typename Lhs, typename Rhs>
using DivideT = typename Divide<Lhs, Rhs>::type;

template <typename Lhs, typename Rhs>
using ModuloT = typename Modulo<Lhs, Rhs>::type;

} // namespace Sequence

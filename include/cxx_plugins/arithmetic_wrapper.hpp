/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    arithmetic_wrapper.hpp
 * \author  Andrey Ponomarev
 * \date    06 Jul 2020
 * \brief
 * $BRIEF$
 */
#pragma once

#include "cxx_plugins/type_traits.hpp"

#include <functional>
#include <iosfwd>

namespace plugins {

template <typename T, typename TagT = void>
// This template class exists to wrap existing arithmetic types
struct ArithmeticWrapper {
public:
  constexpr ArithmeticWrapper() noexcept = default;
  constexpr ArithmeticWrapper(ArithmeticWrapper const &) noexcept = default;
  constexpr ArithmeticWrapper(ArithmeticWrapper &&) noexcept = default;
  constexpr auto operator=(ArithmeticWrapper const &) noexcept
      -> ArithmeticWrapper & = default;
  constexpr auto operator=(ArithmeticWrapper &&) noexcept
      -> ArithmeticWrapper & = default;

  explicit constexpr ArithmeticWrapper(T &&val) noexcept
      : value_m(std::forward<T>(val)) {}

  explicit operator T const &() const noexcept { return value_m; }
  explicit operator T &() noexcept { return value_m; }
  explicit operator T() const noexcept { return value_m; }

  constexpr auto value() noexcept -> T & { return value_m; }
  constexpr auto value() const noexcept -> T const & { return value_m; }

  constexpr ArithmeticWrapper &assign(T &&val) noexcept {
    value_m = std::forward<T>(val);
    return *this;
  }

private:
  T value_m;
};

// Comparison operators

template <typename T, typename TagT>
constexpr auto operator==(ArithmeticWrapper<T, TagT> const &lhs,
                          ArithmeticWrapper<T, TagT> const &rhs) -> bool {
  return lhs.value() == rhs.value();
}
template <typename T, typename TagT>
constexpr auto operator!=(ArithmeticWrapper<T, TagT> const &lhs,
                          ArithmeticWrapper<T, TagT> const &rhs) -> bool {
  return lhs.value() != rhs.value();
}
template <typename T, typename TagT>
constexpr auto operator<(ArithmeticWrapper<T, TagT> const &lhs,
                         ArithmeticWrapper<T, TagT> const &rhs) -> bool {
  return lhs.value() < rhs.value();
}
template <typename T, typename TagT>
constexpr auto operator<=(ArithmeticWrapper<T, TagT> const &lhs,
                          ArithmeticWrapper<T, TagT> const &rhs) -> bool {
  return lhs.value() <= rhs.value();
}
template <typename T, typename TagT>
constexpr auto operator>(ArithmeticWrapper<T, TagT> const &lhs,
                         ArithmeticWrapper<T, TagT> const &rhs) -> bool {
  return lhs.value() > rhs.value();
}
template <typename T, typename TagT>
constexpr auto operator>=(ArithmeticWrapper<T, TagT> const &lhs,
                          ArithmeticWrapper<T, TagT> const &rhs) -> bool {
  return lhs.value() >= rhs.value();
}

// Unary arithmetic
template <typename T, typename TagT>
constexpr auto operator+(ArithmeticWrapper<T, TagT> const &val) {
  return val;
}
template <typename T, typename TagT>
constexpr auto operator-(ArithmeticWrapper<T, TagT> const &val) {
  return ArithmeticWrapper<T, TagT>{-val.value()};
}

template <typename T, typename TagT>
constexpr auto operator++(ArithmeticWrapper<T, TagT> &lhs)
    -> ArithmeticWrapper<T, TagT> & {
  ++lhs.value();
  return lhs;
}
template <typename T, typename TagT>
constexpr auto operator++(ArithmeticWrapper<T, TagT> &lhs, int)
    -> ArithmeticWrapper<T, TagT> & {
  ArithmeticWrapper<T, TagT> tmp = lhs;
  ++lhs;
  return tmp;
}
template <typename T, typename TagT>
constexpr auto operator--(ArithmeticWrapper<T, TagT> &lhs)
    -> ArithmeticWrapper<T, TagT> & {
  ++lhs.value();
  return lhs;
}
template <typename T, typename TagT>
constexpr auto operator--(ArithmeticWrapper<T, TagT> &lhs, int)
    -> ArithmeticWrapper<T, TagT> & {
  ArithmeticWrapper<T, TagT> tmp = lhs;
  ++lhs;
  return tmp;
}

// Unary logical
template <typename T, typename TagT>
constexpr auto operator~(ArithmeticWrapper<T, TagT> const &lhs)
    -> ArithmeticWrapper<T, TagT> & {
  return ArithmeticWrapper<T, TagT>{~lhs.value()};
}
template <typename T, typename TagT>
constexpr auto operator!(ArithmeticWrapper<T, TagT> const &lhs)
    -> ArithmeticWrapper<T, TagT> & {
  return ArithmeticWrapper<T, TagT>{~lhs.value()};
}

// Arithmetic operators

template <typename T, typename TagT>
constexpr auto operator+(ArithmeticWrapper<T, TagT> const &lhs,
                         ArithmeticWrapper<T, TagT> const &rhs)
    -> ArithmeticWrapper<T, TagT> {
  return ArithmeticWrapper<T, TagT>(lhs.value() + rhs.value());
}
template <typename T, typename TagT>
constexpr auto operator+=(ArithmeticWrapper<T, TagT> &lhs,
                          ArithmeticWrapper<T, TagT> const &rhs)
    -> ArithmeticWrapper<T, TagT> & {
  lhs.value() += rhs.value();
  return lhs;
}
template <typename T, typename TagT>
constexpr auto operator-(ArithmeticWrapper<T, TagT> const &lhs,
                         ArithmeticWrapper<T, TagT> const &rhs)
    -> ArithmeticWrapper<T, TagT> {
  return ArithmeticWrapper<T, TagT>(lhs.value() - rhs.value());
}
template <typename T, typename TagT>
constexpr auto operator-=(ArithmeticWrapper<T, TagT> &lhs,
                          ArithmeticWrapper<T, TagT> const &rhs)
    -> ArithmeticWrapper<T, TagT> & {
  lhs.value() -= rhs.value();
  return lhs;
}

template <typename T, typename TagT>
constexpr auto operator*(ArithmeticWrapper<T, TagT> const &lhs,
                         ArithmeticWrapper<T, TagT> const &rhs)
    -> ArithmeticWrapper<T, TagT> {
  return ArithmeticWrapper<T, TagT>(lhs.value() * rhs.value());
}
template <typename T, typename TagT>
constexpr auto operator*=(ArithmeticWrapper<T, TagT> &lhs,
                          ArithmeticWrapper<T, TagT> const &rhs)
    -> ArithmeticWrapper<T, TagT> & {
  lhs.value() *= rhs.value();
  return lhs;
}
template <typename T, typename TagT>
constexpr auto operator/(ArithmeticWrapper<T, TagT> const &lhs,
                         ArithmeticWrapper<T, TagT> const &rhs)
    -> ArithmeticWrapper<T, TagT> {
  return ArithmeticWrapper<T, TagT>(lhs.value() / rhs.value());
}
template <typename T, typename TagT>
constexpr auto operator/=(ArithmeticWrapper<T, TagT> &lhs,
                          ArithmeticWrapper<T, TagT> const &rhs)
    -> ArithmeticWrapper<T, TagT> & {
  lhs.value() -= rhs.value();
  return lhs;
}
template <typename T, typename TagT>
constexpr auto operator%(ArithmeticWrapper<T, TagT> const &lhs,
                         ArithmeticWrapper<T, TagT> const &rhs)
    -> ArithmeticWrapper<T, TagT> {
  return ArithmeticWrapper<T, TagT>(lhs.value() % rhs.value());
}
template <typename T, typename TagT>
constexpr auto operator%=(ArithmeticWrapper<T, TagT> &lhs,
                          ArithmeticWrapper<T, TagT> const &rhs)
    -> ArithmeticWrapper<T, TagT> & {
  lhs.value() %= rhs.value();
  return lhs;
}


// SHIFT OPERATORS ARE NOT DEFINED

// Logical binary operators

template <typename T, typename TagT>
constexpr auto operator|(ArithmeticWrapper<T, TagT> const &lhs,
                         ArithmeticWrapper<T, TagT> const &rhs)
    -> ArithmeticWrapper<T, TagT> {
  return ArithmeticWrapper<T, TagT>(lhs.value() | rhs.value());
}
template <typename T, typename TagT>
constexpr auto operator|=(ArithmeticWrapper<T, TagT> &lhs,
                          ArithmeticWrapper<T, TagT> const &rhs)
    -> ArithmeticWrapper<T, TagT> & {
  lhs.value() |= rhs.value();
  return lhs;
}
template <typename T, typename TagT>
constexpr auto operator&(ArithmeticWrapper<T, TagT> const &lhs,
                         ArithmeticWrapper<T, TagT> const &rhs)
    -> ArithmeticWrapper<T, TagT> {
  return ArithmeticWrapper<T, TagT>(lhs.value() | rhs.value());
}
template <typename T, typename TagT>
constexpr auto operator&=(ArithmeticWrapper<T, TagT> &lhs,
                          ArithmeticWrapper<T, TagT> const &rhs)
    -> ArithmeticWrapper<T, TagT> & {
  lhs.value() &= rhs.value();
  return lhs;
}

template<typename T, typename TagT>
constexpr auto operator&&(ArithmeticWrapper<T,TagT> const& lhs,
                          ArithmeticWrapper<T,TagT> const& rhs) {
  return lhs.value() && rhs.value();
}


template<typename T, typename TagT>
constexpr auto operator||(ArithmeticWrapper<T,TagT> const& lhs,
                          ArithmeticWrapper<T,TagT> const& rhs) {
  return lhs.value() && rhs.value();
}

template<typename T, typename TagT>
constexpr auto operator<<(std::ostream& os,
                          ArithmeticWrapper<T,TagT> const& val) {
  return os << val.value();
}

template<typename T, typename TagT>
constexpr auto operator>>(std::istream& is,
                          ArithmeticWrapper<T,TagT>& val) {
  return is >> val.value();
}



} // namespace CxxPlugins

template <typename T, typename TagT>
class std::hash<plugins::ArithmeticWrapper<T, TagT>> {
  constexpr auto operator()(plugins::ArithmeticWrapper<T, TagT> value)
      -> std::size_t {
    return std::hash<T>(static_cast<T>(value));
  }
};

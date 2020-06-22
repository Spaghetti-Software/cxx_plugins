/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    conversion.hpp
 * \author  Andrey Ponomarev
 * \date    19 Jun 2020
 * \brief
 * $BRIEF$
 */
#pragma once

#include <utility>
#include <array>

namespace Sequence {

namespace impl {
template <typename U, std::size_t size, typename T, T... values,
          std::size_t... indices>
constexpr void fill(U (&array)[size],
                    std::integer_sequence<T, values...> /*unused*/,
                    std::index_sequence<indices...> /*unused*/) noexcept {
  ((array[indices] = static_cast<U>(values)), ...);
}
} // namespace impl

template <typename U, std::size_t size, typename T, T... values>
constexpr void fill(U (&array)[size],
                    std::integer_sequence<T, values...> sequence) noexcept {
  static_assert(
      size <= sizeof...(values),
      "Size of array should be less or equal to the size of integer sequence.");
  impl::fill(array, sequence, std::make_index_sequence<sizeof...(values)>{});
}

template<typename Seq>
struct AsArray;

template<typename T, T... values>
struct AsArray<std::integer_sequence<T,values...>> {
  static constexpr T value[sizeof...(values)] = {values...};
};

template<typename Seq>
struct AsStdArray;

template<typename T, T... values>
struct AsStdArray<std::integer_sequence<T,values...>> {
  static constexpr std::array<T,sizeof...(values)> value = {values...};
};





} // namespace Sequence
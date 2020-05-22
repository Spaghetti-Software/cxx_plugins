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
#include <utility>

//! \brief Contains helper functions/classes
namespace CxxPlugins::utility {

template <typename... Types> struct AreUnique;

template <typename First, typename... Rest> struct AreUnique<First, Rest...> {
  static constexpr bool value = (!std::is_same_v<First, Rest> && ...);
};
template <> struct AreUnique<> { static constexpr bool value = true; };

template <typename... Types>
static constexpr bool are_unique_v = AreUnique<Types...>::value;

template <typename T, typename... U> struct IsInThePack {
  static constexpr bool value = (std::is_same_v<T, U> || ...);
};

template <typename T, typename... U>
static constexpr bool is_in_the_pack_v = IsInThePack<T, U...>::value;

namespace impl {
template <typename T, std::size_t I> struct IndexStorage {
  using Type = T;
  static constexpr std::size_t value = I;
};

template <typename Is, typename... Ts> struct Indexer;

template <std::size_t... Is, typename... Ts>
struct Indexer<std::integer_sequence<std::size_t, Is...>, Ts...>
    : IndexStorage<Ts, Is>... {
};

template <typename T, std::size_t I>
static constexpr auto select(IndexStorage<T, I>) -> IndexStorage<T, I>;

template<std::size_t I, typename T>
static constexpr auto select(IndexStorage<T,I>)->IndexStorage<T,I>;

} // namespace impl

template <typename T, typename... Ts>
static constexpr std::size_t index_of = decltype(impl::select<T>(
    impl::Indexer<std::make_integer_sequence<std::size_t, sizeof...(Ts)>,
                  Ts...>{}))::value;
template<std::size_t I, typename... Ts>
using ElementType = typename decltype(impl::select<I>(
    impl::Indexer<std::make_integer_sequence<std::size_t, sizeof...(Ts)>,
        Ts...>{}))::Type;

template <typename LeftSequence, typename RightSequence>
struct IntegerSequenceCat;

template <typename T, T... left_indices, T... right_indices>
struct IntegerSequenceCat<std::integer_sequence<T, left_indices...>,
                          std::integer_sequence<T, right_indices...>> {
  using Type = std::integer_sequence<T, left_indices..., right_indices...>;
};

template <typename T, T value, std::size_t n> struct IntegerSequenceRepeat {
  static_assert(n > 0, "n should be greater than 0");
  using Type = typename IntegerSequenceCat<
      std::integer_sequence<T, value>,
      typename IntegerSequenceRepeat<T, value, n - 1>::Type>::Type;
};

template <typename T, T value> struct IntegerSequenceRepeat<T, value, 0> {
  using Type = std::make_integer_sequence<T, 0>;
};

} // namespace CxxPlugins::utility

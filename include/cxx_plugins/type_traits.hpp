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
    : IndexStorage<Ts, Is>... {};

template <typename T, std::size_t I>
static constexpr auto select(IndexStorage<T, I>) -> IndexStorage<T, I>;

template <std::size_t I, typename T>
static constexpr auto select(IndexStorage<T, I>) -> IndexStorage<T, I>;

} // namespace impl

template <typename T, typename... Ts>
static constexpr std::size_t index_of = decltype(impl::select<T>(
    impl::Indexer<std::make_integer_sequence<std::size_t, sizeof...(Ts)>,
                  Ts...>{}))::value;
template <std::size_t I, typename... Ts>
using ElementType = typename decltype(impl::select<I>(
    std::declval<
        impl::Indexer<std::make_integer_sequence<std::size_t, sizeof...(Ts)>,
                      Ts...>>()))::Type;

template <std::size_t I, typename T, T... values>
constexpr auto sequenceAt(std::integer_sequence<T, values...> /*unused*/) {
  return ElementType<I, std::integral_constant<T, values>...>{};
}

template <typename FirstSequence, typename... Sequences>
struct IntegerSequenceCat;

template <typename... Ts>
using IntegerSequenceCatT = typename IntegerSequenceCat<Ts...>::Type;

template <typename T, T... left_indices, T... right_indices>
struct IntegerSequenceCat<std::integer_sequence<T, left_indices...>,
                          std::integer_sequence<T, right_indices...>> {
  using Type = std::integer_sequence<T, left_indices..., right_indices...>;
};

template <typename T, T... indices>
struct IntegerSequenceCat<std::integer_sequence<T, indices...>> {
  using Type = std::integer_sequence<T, indices...>;
};

template <typename FirstSequence, typename SecondSequence, typename... Rest>
struct IntegerSequenceCat<FirstSequence, SecondSequence, Rest...> {
  using Type =
      IntegerSequenceCatT<IntegerSequenceCatT<FirstSequence, SecondSequence>,
                          IntegerSequenceCatT<Rest...>>;
};

template <typename FirstSequence, typename... Rest> struct IntegerSequenceCat {
  using Type = typename IntegerSequenceCat<
      FirstSequence, typename IntegerSequenceCat<Rest...>::Type>::Type;
};

template <typename T> struct IsImplicitDefaultConstructible {
private:
  template <typename U>
  static constexpr void innerHelper(const U & /*unused*/) {}

  template <typename U>
  static constexpr auto
  check(std::nullptr_t /*unused*/,
        decltype(innerHelper<U>({})) * /*unused*/ = nullptr) {
    return std::true_type{};
  }
  template <typename U> static constexpr auto check(void * /*unused*/) {
    return std::false_type{};
  }

public:
  static constexpr bool value = decltype(check<T>(nullptr))::value;
};

template <typename T>
static constexpr bool is_implicit_default_constructible_v =
    IsImplicitDefaultConstructible<T>::value;

template <std::size_t start_with, typename TimesToRepeat>
struct RepeatingIndexSequence;

template <std::size_t start_with, typename TimesToRepeat>
using RepeatingIndexSequenceT =
    typename RepeatingIndexSequence<start_with, TimesToRepeat>::Type;

namespace impl {
template <std::size_t value, typename Sequence>
struct RepeatingIndexSequenceSingleImpl;

template <std::size_t value, std::size_t... indices>
struct RepeatingIndexSequenceSingleImpl<value,
                                        std::index_sequence<indices...>> {
  using Type = std::index_sequence<(value + indices - indices)...>;
};

template <std::size_t starting, typename RepeatSequence, typename Sequence>
struct RepeatingIndexSequenceMultiImpl;

template <std::size_t starting, std::size_t... repeat, std::size_t... indices>
struct RepeatingIndexSequenceMultiImpl<starting, std::index_sequence<repeat...>,
                                       std::index_sequence<indices...>> {
  static_assert(sizeof...(repeat) == sizeof...(indices),
                "Sizes should be same");

  using Type = IntegerSequenceCatT<RepeatingIndexSequenceT<
      starting + indices, std::integral_constant<std::size_t, repeat>>...>;
};

} // namespace impl

template <std::size_t value, std::size_t times_to_repeat>
struct RepeatingIndexSequence<
    value, std::integral_constant<std::size_t, times_to_repeat>> {
  using Type = typename impl::RepeatingIndexSequenceSingleImpl<
      value, std::make_index_sequence<times_to_repeat>>::Type;
};

template <std::size_t start_with, std::size_t... indices>
struct RepeatingIndexSequence<start_with, std::index_sequence<indices...>> {
  using Type = typename impl::RepeatingIndexSequenceMultiImpl<
      start_with, std::index_sequence<indices...>,
      std::make_index_sequence<sizeof...(indices)>>::Type;
};

} // namespace CxxPlugins::utility

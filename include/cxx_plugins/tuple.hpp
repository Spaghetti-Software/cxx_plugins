/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    tuple.hpp
 * \author  Andrey Ponomarev
 * \date    14 May 2020
 * \brief
 *
 * This file contains alias for tuple
 * It was created so we can easily switch tuple implementation
 * For example if we will want to make tuple to have standard layout
 */
#pragma once

#include "cxx_plugins/type_traits.hpp"

#include <tuple>

namespace CxxPlugins {
/*!
 * \brief Alias for tuple.
 * \details
 * We need to alias tuple as we might need to change tuple type to be
 * a pod. This might be required to be able to transfer tuples between shared
 * libraries compiled with different compilers/versions of compilers.
 */
template <typename... TArgs> using Tuple = std::tuple<TArgs...>;

template <std::size_t I, typename T> struct TupleElement;

template <std::size_t I, typename... Ts> struct TupleElement<I, Tuple<Ts...>> {
  using Type = utility::ElementType<I, Ts...>;
};
//! Alias for TupleElement::Type
template <std::size_t I, typename T>
using TupleElementType = typename TupleElement<I, T>::Type;

//! Alias for std::get(<tuple>)
template <std::size_t id, typename T>
static constexpr auto get(T &&val) -> auto & {
  return std::get<id>(std::forward<T>(val));
}

//! Alias for std::get(<tuple>)
template <std::size_t id, typename T>
static constexpr auto get(T &val) -> auto & {
  return std::get<id>(val);
}

template <typename... Tuples> constexpr auto tupleCat(Tuples &&... args) {
  return std::tuple_cat(std::forward<Tuples>(args)...);
}

template <typename... T>
using TupleCatType = decltype(tupleCat(std::declval<T>()...));

template <typename T, typename U> using Pair = std::pair<T, U>;

template <typename T, typename U> constexpr auto makePair(T &&t, U &&u) {
  return Pair<std::decay_t<T>, std::decay_t<U>>{std::forward<T>(t),
                                                std::forward<U>(u)};
}

template <typename... Types>
constexpr auto forwardAsTuple(Types &&... args) noexcept -> Tuple<Types &&...> {
  return std::forward_as_tuple(std::forward<Types>(args)...);
}

template <typename T> struct TupleSize;

template <typename... Types> struct TupleSize<Tuple<Types...>> {
  static constexpr std::size_t value = sizeof...(Types);
};

template <typename T>
static constexpr std::size_t tuple_size_v = TupleSize<T>::value;

template <class F, class Tuple>
constexpr decltype(auto) apply(F &&f, Tuple &&t) {
  return std::apply(std::forward<F>(f), std::forward<Tuple>(t));
}

namespace impl {

template <typename... TArgs>
using AlignmentsOf = std::integer_sequence<std::size_t, alignof(TArgs)...>;

template <typename... TArgs>
using SizesOf = std::integer_sequence<std::size_t, sizeof(TArgs)...>;

template <std::size_t CurrentOffset, typename T>
using RequiredOffset =
    std::integral_constant<std::size_t,
                           (CurrentOffset % alignof(T) == 0
                                ? CurrentOffset
                                : (CurrentOffset / alignof(T) + 1) *
                                      alignof(T))>; // round up

/*
 * First, Second, Third
 *
 * size = required_offset<required_offset<sizeof(First), Second> +
 * sizeof(Second), Third>;
 *
 *
 *
 */

template <std::size_t current, typename T>
static constexpr std::size_t my_offset = RequiredOffset<current, T>::value +
                                         sizeof(T);

template <std::size_t I, typename Seq> struct SequenceAt;

template <std::size_t I, typename T, T... values>
struct SequenceAt<I, std::integer_sequence<T, values...>> {
  using Type = std::integral_constant<
      T,
      TupleElementType<I, Tuple<std::integral_constant<T, values>...>>::value>;
};

template<std::size_t I, typename Seq>
using SequenceTypeAt = typename SequenceAt<I,Seq>::Type;


template <typename T, T... values, T to_append>
constexpr auto append(std::integer_sequence<T, values...> /*unused*/,
                      std::integral_constant<T, to_append> /*unused*/) {
  return std::integer_sequence<T, values..., to_append>{};
}

template <typename T, T... values, T... to_append>
constexpr auto append(std::integer_sequence<T, values...> /*unused*/,
                      std::integer_sequence<T, to_append...> /*unused*/) {
  return std::integer_sequence<T, values..., to_append...>{};
}

template <typename... TArgs> struct AlignedTupleStorageSize {
  using sizes = SizesOf<TArgs...>;
  using alignments = AlignmentsOf<TArgs...>;
};

template <std::size_t previous_offset, typename... TArgs> struct AlignedOffsets;

template <std::size_t previous_offset, typename CurrentType, typename... Rest>
struct AlignedOffsets<previous_offset, CurrentType, Rest...> {

  static constexpr std::size_t current_offset =
      RequiredOffset<previous_offset, CurrentType>::value;

  using Type = decltype(
      append(std::integer_sequence<std::size_t, current_offset>{},
             typename AlignedOffsets<current_offset + sizeof(CurrentType),
                                     Rest...>::Type{}));
};

template <std::size_t previous_offset> struct AlignedOffsets<previous_offset> {
  using Type = std::integer_sequence<std::size_t>;
};

template <typename... TArgs>
using AlignedOffsetsSequence = typename AlignedOffsets<0, TArgs...>::Type;

template <std::size_t I, typename T, T... values>
constexpr auto sequenceAt(std::integer_sequence<T, values...> /*unused*/) {
  using tuple = Tuple<std::integral_constant<T, values>...>;
  return TupleElementType<I, tuple>{};
}

template <typename... TArgs> struct PackedTupleStorage {

  using Offsets = AlignedOffsetsSequence<TArgs...>;
  using Sizes = SizesOf<TArgs...>;

  static constexpr std::size_t size =
     SequenceTypeAt<sizeof...(TArgs) - 1, Offsets>::value +
     SequenceTypeAt<sizeof...(TArgs) - 1, Sizes>::value;

  alignas(alignof(utility::ElementType<0, TArgs...>)) char data_m[size];
};

template<> struct PackedTupleStorage<> {
  using Offsets = std::integer_sequence<std::size_t>;
  using Sizes = std::integer_sequence<std::size_t>;
  static constexpr std::size_t size = 0;

  char data_m[size];
};


struct primitive{
  char data_m[0];
};

static_assert(sizeof(primitive) == 0,"a");

struct simple {
  int i;
  int j;
  int k;
};

static_assert(sizeof(simple) == sizeof(PackedTupleStorage<int, int, int>),
              "Sizes are different");

struct more_complex {
  int i;
  char ch;
  int j;
};

 static_assert(sizeof(more_complex) == sizeof(simple),
              "more_complex and simple are of different size");
 static_assert(sizeof(PackedTupleStorage<int, char, int>) ==
                  sizeof(more_complex),
              "Sizes are different");

struct worst {
  double d;
  char ch;
  int i;
  int j;
  double d1;
};

 static_assert(sizeof(worst) ==
                  sizeof(PackedTupleStorage<double, char, int, int, double>),
              "dough");
 static_assert(alignof(worst) ==
                  alignof(PackedTupleStorage<double, char, int, int, double>),
              "dough");

} // namespace impl

template <typename... Ts>
struct PackedTuple {
public:
private:
//  impl::PackedTupleStorage<Ts...>
};

} // namespace CxxPlugins
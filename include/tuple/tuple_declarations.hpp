/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    tuple_declarations.hpp
 * \author  Andrey Ponomarev
 * \date    08 Jun 2020
 * \brief
 * $BRIEF$
 */
#pragma once

#include <cstdint>
#include <type_traits>

namespace plugins {

template <typename... Ts> struct Tuple;
template <typename... Ts> struct TupleMap;
template <typename Tag, typename T> struct TaggedValue;

// Traits

template <typename T> struct TupleSize;
template <typename T>
static constexpr std::size_t tuple_size_v = TupleSize<T>::value;
template <std::size_t I, typename T> struct TupleElement;

template <std::size_t I, typename T>
using TupleElementT = typename TupleElement<I, T>::Type;


template <typename T> struct IsTuple : public std::false_type {};
template <typename... Ts>
struct IsTuple<Tuple<Ts...>> : public std::true_type {};

template <typename T> static constexpr bool is_tuple_v = IsTuple<T>::value;

template <typename T> struct IsTupleMap : public std::false_type {};

template <typename... Ts>
struct IsTupleMap<TupleMap<Ts...>> : public std::true_type {};

template <typename T>
static constexpr bool is_tuple_map_v = IsTupleMap<T>::value;

template <typename OuterSeq, typename InnerSeq, typename TupleOfTuple,
          typename Enabler = void>
struct TupleCatResult;

// Getters

template <std::size_t I, typename... Us>
constexpr TupleElementT<I, Tuple<Us...>> const &
get(Tuple<Us...> const &tuple) noexcept;

template <std::size_t I, typename... Us>
constexpr TupleElementT<I, Tuple<Us...>> &get(Tuple<Us...> &tuple) noexcept;

template <std::size_t I, typename... Us>
constexpr TupleElementT<I, Tuple<Us...>> &&get(Tuple<Us...> &&tuple) noexcept;

template <std::size_t I, typename... Us>
constexpr TupleElementT<I, Tuple<Us...>> const &&
get(Tuple<Us...> const &&tuple) noexcept;

template <std::size_t I, typename... Us>
constexpr TupleElementT<I, TupleMap<Us...>> const &
get(TupleMap<Us...> const &tuple) noexcept;

template <std::size_t I, typename... Us>
constexpr TupleElementT<I, TupleMap<Us...>> &
get(TupleMap<Us...> &tuple) noexcept;

template <std::size_t I, typename... Us>
constexpr TupleElementT<I, TupleMap<Us...>> &&
get(TupleMap<Us...> &&tuple) noexcept;

template <std::size_t I, typename... Us>
constexpr TupleElementT<I, TupleMap<Us...>> const &&
get(TupleMap<Us...> const &&tuple) noexcept;

// Creation of tuples

template <typename... Ts>
constexpr auto makeTuple(Ts &&... vals) noexcept -> Tuple<std::decay_t<Ts>...>;

template <typename... TaggedValues>
constexpr auto makeTupleMap(TaggedValues &&... vals) noexcept
    -> TupleMap<std::decay_t<TaggedValues>...>;

template <typename... Tuples>
constexpr decltype(auto) tupleCat(Tuples &&... tuples);

template <typename Fn, typename... Tuples>
void apply(Fn &&fn, Tuples &&... tuples);

} // namespace CxxPlugins
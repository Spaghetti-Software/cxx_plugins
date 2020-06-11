/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    map.hpp
 * \author  Andrey Ponomarev
 * \date    08 Jun 2020
 * \brief
 * $BRIEF$
 */
#pragma once

#include "sequence/size.hpp"

#include <utility>

namespace Sequence {

namespace impl {

template <std::size_t I, typename T, T value>
struct IndexStorage : public std::integral_constant<T, value> {};

template <typename Seq, typename Is = std::make_index_sequence<size_v<Seq>>>
struct Indexer;

template <typename T, T... values, std::size_t... indices>
struct Indexer<std::integer_sequence<T, values...>,
               std::index_sequence<indices...>>
    : public IndexStorage<indices, T, values>... {};

template <std::size_t I, typename T, T value>
auto select(IndexStorage<I, T, value>) -> IndexStorage<I, T, value>;

template <std::size_t I, typename Seq>
static constexpr auto select_v =
    decltype(impl::select<I>(std::declval<impl::Indexer<Seq>>()))::value;

template <std::size_t I, typename Seq>
using select_t = std::integral_constant<decltype(select_v<I, Seq>), select_v<I, Seq>>;

} // namespace impl

template <std::size_t I, typename T, T... vals> struct AtIndex {
  using type =  impl::select_t<I,std::integer_sequence<T,vals...>>;
};

template <std::size_t I, typename T, T... vals>
struct AtIndex<I, std::integer_sequence<T, vals...>> {
  using type = impl::select_t<I, std::integer_sequence<T, vals...>>;
};

template <std::size_t I, typename T, T... vals>
using AtIndexT = typename AtIndex<I, T, vals...>::type;

template <std::size_t I, typename T, T... vals>
static constexpr auto at_index_v = AtIndexT<I, T, vals...>::value;

template<typename T, T... vals>
struct Last {
  static_assert(sizeof...(vals) > 0, "Sequence is empty");
  using type = AtIndexT<sizeof...(vals) - 1, T, vals...>;
};

template<typename T, T... vals>
struct Last<std::integer_sequence<T,vals...>> {
  static_assert(sizeof...(vals) > 0, "Sequence is empty");
  using type = AtIndexT<sizeof...(vals) - 1, std::integer_sequence<T,vals...>>;
};

template<typename T, T... vals>
using LastT = typename Last<T,vals...>::type ;

template<typename T, T... vals>
static constexpr auto last_v = LastT<T,vals...>::value;


template <typename KeysSeq, typename ValuesSeq> struct Map;

template <std::size_t... keys, typename ValuesSeq>
struct Map<std::index_sequence<keys...>, ValuesSeq> {
  using type = std::integer_sequence<typename ValuesSeq::value_type,
                                     at_index_v<keys, ValuesSeq>...>;
};

template<typename KeysSeq, typename ValuesSeq>
using MapT = typename Map<KeysSeq, ValuesSeq>::type;

} // namespace Sequence
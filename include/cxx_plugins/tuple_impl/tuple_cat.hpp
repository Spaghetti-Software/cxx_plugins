/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    tuple_cat.hpp
 * \author  Andrey Ponomarev
 * \date    08 Jun 2020
 * \brief
 * $BRIEF$
 */
#pragma once

#include "cxx_plugins/tuple_impl/tuple_declarations.hpp"
#include "cxx_plugins/type_traits.hpp"
#include "sequence/exclusive_scan.hpp"
#include "sequence/inclusive_scan.hpp"
#include "sequence/map.hpp"

#include <utility>

namespace CxxPlugins {

template <typename OuterSeq, typename InnerSeq, typename TupleOfTuple>
using TupleCatResultT =
    typename TupleCatResult<OuterSeq, InnerSeq, TupleOfTuple>::type;

template <std::size_t... outer_indices, std::size_t... inner_indices,
          typename... Tuples>
struct TupleCatResult<std::index_sequence<outer_indices...>,
                      std::index_sequence<inner_indices...>, Tuple<Tuples...>,
                      std::enable_if_t<(is_tuple_v<Tuples> && ...)>> {
  using type = Tuple<TupleInnerElementT<
      inner_indices,
      CxxPlugins::utility::ElementType<outer_indices, Tuples...>>...>;
};

template <std::size_t... outer_indices, std::size_t... inner_indices,
          typename... Tuples>
struct TupleCatResult<std::index_sequence<outer_indices...>,
                      std::index_sequence<inner_indices...>, Tuple<Tuples...>,
                      std::enable_if_t<(is_tuple_map_v<Tuples> && ...)>> {
  using type = TupleMap<TupleInnerElementT<
      inner_indices,
      CxxPlugins::utility::ElementType<outer_indices, Tuples...>>...>;
};

namespace impl {
template <std::size_t max, std::size_t... vals>
struct CountLessOrEqual
    : std::integral_constant<std::size_t, ((vals <= max ? 1 : 0) + ...)> {};

template <typename ValuesSeq, typename SizesSeq> struct Expand;

template <std::size_t... values, std::size_t... sizes>
struct Expand<std::integral_constant<std::size_t, values...>,
              std::integral_constant<std::size_t, sizes...>> {
  template <std::size_t I> using LessOrEq = CountLessOrEqual<I, sizes...>;

  using type = std::index_sequence<LessOrEq<values>::value...>;
};

template <typename ValuesSeq, typename SizesSeq>
using ExpandT = typename Expand<ValuesSeq, SizesSeq>::type;

template <typename... Tuples> struct TupleCatHelper {
  using SizeSequence = std::index_sequence<TupleSize<Tuples>::value...>;
  using ResultSequence =
      std::make_index_sequence<(TupleSize<Tuples>::value + ...)>;

  using OuterSequence =
      Expand<ResultSequence,
             ExpandT<ResultSequence, Sequence::InclusiveScanT<SizeSequence>>>;
  using InnerSequence = Sequence::MinusT<
      ResultSequence,
      Sequence::MapT<OuterSequence, Sequence::ExclusiveScan<SizeSequence>>>;
  using ResultType =
      TupleCatResultT<OuterSequence, InnerSequence, Tuple<Tuples...>>;
};

template <typename Return, std::size_t... outer_indices,
          std::size_t... inner_indices, typename TupleOfTuples>
constexpr Return tupleCat(std::index_sequence<outer_indices...> /*unused*/,
                          std::index_sequence<inner_indices...> /*unused*/,
                          TupleOfTuples values) {
  return Return(get<inner_indices>(get<outer_indices>(values))...);
}

} // namespace impl

template <typename... Tuples>
constexpr decltype(auto) tupleCat(Tuples &&... tuples) {
  static_assert(sizeof...(tuples) > 0,
                "There should be at least 1 tuple to cat.");
  using helper = impl::TupleCatHelper<Tuples...>;

  return impl::tupleCat<typename helper::ResultType>(
      helper::OuterSequence(), helper::InnerSequence(),
      forwardAsTuple(std::forward<Tuples>(tuples)...));
}

} // namespace CxxPlugins
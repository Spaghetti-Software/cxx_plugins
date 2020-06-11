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

#include "cxx_plugins/type_traits.hpp"
#include "sequence/exclusive_scan.hpp"
#include "sequence/inclusive_scan.hpp"
#include "sequence/map.hpp"
#include "tuple/forward.hpp"
#include "tuple/tuple_helpers.hpp"
#include "tuple/tuple_impl.hpp"

#include <utility>

namespace CxxPlugins {

template <template <typename...> class TupleTemplate, typename... FirstParam,
          typename... RestTuples>
constexpr auto tupleCat(TupleTemplate<FirstParam...> &&first,
                        RestTuples &&... rest);

/*!
 * \brief Same thing as TupleElement but used in tupleCat
 *
 * \details This template is required for support of TupleMap
 * As TupleElement is equal to the underlying type,
 * But We can't do TupleMap<Values...>, instead we need to do
 * TupleMap<TaggedValue<Tags,Values>...>
 *
 * \note Default implementation is using TupleElement, but you can always add
 * partial specialization.
 */
template <std::size_t I, typename Tuple> struct TupleCatElement {
  using Type = TupleElementT<I, Tuple>;
};

template <std::size_t I, typename Tuple>
using TupleCatElementT = typename TupleCatElement<I, Tuple>::Type;

namespace impl {
template <typename... Tuples> struct TupleCatHelper;

template <typename Return, std::size_t... outer_indices,
          std::size_t... inner_indices, typename TupleOfTuples>
constexpr auto tupleCat(std::index_sequence<outer_indices...> /*unused*/,
                        std::index_sequence<inner_indices...> /*unused*/,
                        TupleOfTuples values) -> Return;

template <typename T, template <typename...> class Template>
struct IsInstantiationOf : std::false_type {};
template <typename... Ts, template <typename...> class Template>
struct IsInstantiationOf<Template<Ts...>, Template> : std::true_type {};

template <typename T, template <typename...> class Template>
static constexpr auto is_instantiation_of_v =
    IsInstantiationOf<T, Template>::value;

} // namespace impl

template <template <typename...> class TupleTemplate, typename... FirstParam,
          typename... RestTuples>
constexpr auto tupleCat(TupleTemplate<FirstParam...> const &first,
                        RestTuples &&... rest) {
  constexpr bool are_same =
      (impl::is_instantiation_of_v<std::decay_t<RestTuples>, TupleTemplate> &&
       ...);
  static_assert(are_same,
                "All tuples should be instantiations of the same template");
  using FirstT = TupleTemplate<FirstParam...>;

  using helper = impl::TupleCatHelper<FirstT, std::decay_t<RestTuples>...>;

  return impl::tupleCat<typename helper::ResultType>(
      helper::outer_indices, helper::inner_indices,
      forwardAsTuple(first, std::forward<RestTuples>(rest)...));
}

template <template <typename...> class TupleTemplate, typename... FirstParam,
          typename... RestTuples>
constexpr auto tupleCat(TupleTemplate<FirstParam...> &&first,
                        RestTuples &&... rest) {
  constexpr bool are_same =
      (impl::is_instantiation_of_v<std::decay_t<RestTuples>, TupleTemplate> &&
       ...);
  static_assert(are_same,
                "All tuples should be instantiations of the same template");
  using FirstT = TupleTemplate<FirstParam...>;

  using helper = impl::TupleCatHelper<FirstT, std::decay_t<RestTuples>...>;

  return impl::tupleCat<typename helper::ReturnT>(
      helper::outer_indices, helper::inner_indices,
      forwardAsTuple(std::move(first), std::forward<RestTuples>(rest)...));
}

namespace impl {

template <typename OuterSeq, typename InnerSeq, typename TupleOfTuples>
struct TupleCatResult;

template <typename OuterSeq, typename InnerSeq, typename TupleOfTuple>
using TupleCatResultT =
    typename TupleCatResult<OuterSeq, InnerSeq, TupleOfTuple>::Type;

template <std::size_t... outer_indices, std::size_t... inner_indices,
          template <typename...> class TupleTemplate, typename... FirstParam,
          typename... RestTuples>
struct TupleCatResult<std::index_sequence<outer_indices...>,
                      std::index_sequence<inner_indices...>,
                      Tuple<TupleTemplate<FirstParam...>, RestTuples...>> {
private:
  using FirstTuple = TupleTemplate<FirstParam...>;

public:
  using Type = TupleTemplate<TupleCatElementT<
      inner_indices, CxxPlugins::utility::ElementType<outer_indices, FirstTuple,
                                                      RestTuples...>>...>;
};

template <std::size_t max, std::size_t... vals>
struct CountLessOrEqual
    : std::integral_constant<std::size_t, ((vals <= max ? 1 : 0) + ...)> {};

template <typename ValuesSeq, typename SizesSeq> struct Expand;

template <std::size_t... values, std::size_t... sizes>
struct Expand<std::index_sequence<values...>, std::index_sequence<sizes...>> {
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
      ExpandT<ResultSequence, Sequence::InclusiveScanT<SizeSequence>>;
  using InnerSequence = Sequence::MinusT<
      ResultSequence,
      Sequence::MapT<OuterSequence, Sequence::ExclusiveScanT<SizeSequence>>>;
  using ResultType =
      TupleCatResultT<OuterSequence, InnerSequence, Tuple<Tuples...>>;

  static constexpr OuterSequence outer_indices = {};
  static constexpr InnerSequence inner_indices = {};
};

template <typename Return, std::size_t... outer_indices,
          std::size_t... inner_indices, typename TupleOfTuples>
constexpr auto tupleCat(std::index_sequence<outer_indices...> /*unused*/,
                        std::index_sequence<inner_indices...> /*unused*/,
                        TupleOfTuples values) -> Return {
  return Return(get<inner_indices>(get<outer_indices>(values))...);
}

} // namespace impl

} // namespace CxxPlugins
/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    exclusive_scan.hpp
 * \author  Andrey Ponomarev
 * \date    08 Jun 2020
 * \brief
 * $BRIEF$
 */
#pragma once

#include "sequence/partial_sum.hpp"

namespace Sequence {

namespace impl {
template <typename Seq,
          typename Indices = std::make_index_sequence<size_v<Seq>>>
struct ExclusiveScan;

template <typename Seq, std::size_t... indices>
struct ExclusiveScan<Seq, std::index_sequence<indices...>> {
  using type = std::integer_sequence<typename Seq::value_type,
                                     PartialSum<indices, Seq>::value...>;
};

} // namespace impl

template <typename T, T... values>
struct ExclusiveScan
    : public impl::ExclusiveScan<std::integer_sequence<T, values...>> {};

template <typename T, T... values>
struct ExclusiveScan<std::integer_sequence<T, values...>>
    : public impl::ExclusiveScan<std::integer_sequence<T, values...>> {};

template<typename T, T... values>
using ExclusiveScanT = typename ExclusiveScan<T,values...>::type;

} // namespace Sequence
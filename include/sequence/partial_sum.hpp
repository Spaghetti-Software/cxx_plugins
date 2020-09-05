/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    partial_sum.hpp
 * \author  Andrey Ponomarev
 * \date    08 Jun 2020
 * \brief
 * $BRIEF$
 */
#pragma once

#include "sequence/size.hpp"

namespace Sequence {

namespace impl {
template <std::size_t Size, typename Seq,
          typename = std::make_index_sequence<size_v<Seq>>>
struct PartialSum;

template <std::size_t Size, typename T, T... vals, std::size_t... indices>
struct PartialSum<Size, std::integer_sequence<T, vals...>,
                  std::index_sequence<indices...>>
    : std::integral_constant<T, ((indices < Size ? vals : 0) + ...)> {
  static_assert(Size <= sizeof...(indices),
                "Sequence::PartialSum<Size,Seq>: Size is out of range");
};
} // namespace impl

template <std::size_t Size, typename T, T... Ns>
struct PartialSum
    : public impl::PartialSum<Size, std::integer_sequence<T, Ns...>> {};

template <std::size_t Size, typename T, T... Ns>
struct PartialSum<Size, std::integer_sequence<T, Ns...>>
    : public impl::PartialSum<Size, std::integer_sequence<T, Ns...>> {};

} // namespace Sequence
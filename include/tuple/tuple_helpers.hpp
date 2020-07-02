/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    helper_classes.hpp
 * \author  Andrey Ponomarev
 * \date    10 Jun 2020
 * \brief
 * $BRIEF$
 */
#pragma once

#include <cxx_plugins/type_traits.hpp>

#include <cstdint>
#include <array>

namespace CxxPlugins {
template <typename T> struct TupleSize;

template <template <typename...> class TupleTemplate, typename... Ts>
struct TupleSize<TupleTemplate<Ts...>> {
  static constexpr std::size_t value = sizeof...(Ts);
};

template<typename T, std::size_t size>
struct TupleSize<std::array<T,size>> {
  static constexpr std::size_t value = size;
};


template <typename T>
static constexpr std::size_t tuple_size_v = TupleSize<T>::value;

template <std::size_t I, typename TupleT> struct TupleElement;

template <std::size_t I, template <typename...> class TupleTemplate,
          typename... Ts>
struct TupleElement<I, TupleTemplate<Ts...>> {
  using Type = utility::ElementType<I,Ts...>;
};

template <std::size_t I, typename T, std::size_t size>
struct TupleElement<I, std::array<T,size>> {
  using Type = T;
};

template <std::size_t I, typename TupleT>
using TupleElementT = typename TupleElement<I, TupleT>::Type;

} // namespace CxxPlugins
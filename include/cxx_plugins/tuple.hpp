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

//! Alias for std::tuple_element
template <size_t I, typename T>
using TupleElement = typename std::tuple_element<I, T>::type;

//! Alias for std::get(<tuple>)
template <size_t id, typename T> static constexpr auto get(T &&val) -> auto & {
  return std::get<id>(std::forward<T>(val));
}

//! Alias for std::get(<tuple>)
template <size_t id, typename T> static constexpr auto get(T &val) -> auto & {
  return std::get<id>(val);
}
} // namespace CxxPlugins
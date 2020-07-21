/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    variant.hpp
 * \author  Andrey Ponomarev
 * \date    07 Jul 2020
 * \brief
 * $BRIEF$
 */
#pragma once

#include <variant>

namespace CxxPlugins {
template <typename... Ts>
using Variant [[deprecated(
    "CxxPlugins variant is not implemented yet. Using std::variant.")]] =
    std::variant<Ts...>;

using Monostate = std::monostate;

template <class T, class... Types>
constexpr auto holdsAlternative(const std::variant<Types...> &v) noexcept
    -> bool {
  return std::holds_alternative<T>(v);
}

} // namespace CxxPlugins
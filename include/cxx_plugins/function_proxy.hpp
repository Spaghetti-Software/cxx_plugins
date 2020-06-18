/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    function_proxy.hpp
 * \author  Andrey Ponomarev
 * \date    18 Jun 2020
 * \brief
 * $BRIEF$
 */
#pragma once

#include <utility>

namespace CxxPlugins {
template <typename Signature> struct FunctionProxy;

template <typename Return, typename... Args>
struct FunctionProxy<Return(Args...)> {
public:
  constexpr FunctionProxy(Return (*fn_p)(void *, Args...), void *obj_p) noexcept
      : fn_p_m(fn_p), obj_p_m(obj_p) {}

  constexpr auto operator()(Args... args) const -> Return {
    return fn_p_m(obj_p_m, std::forward<Args>(args)...);
  }

private:
  Return (*fn_p_m)(void *, Args...) = nullptr;
  void *obj_p_m = nullptr;
};

template <typename Return, typename... Args>
struct FunctionProxy<Return(Args...) const> {
public:
  constexpr FunctionProxy(Return (*fn_p)(void const *, Args...),
                          void const *obj_p) noexcept
      : fn_p_m(fn_p), obj_p_m(obj_p) {}

  constexpr auto operator()(Args... args) const -> Return {
    return fn_p_m(obj_p_m, std::forward<Args>(args)...);
  }

private:
  Return (*fn_p_m)(void const *, Args...) = nullptr;
  void const *obj_p_m = nullptr;
};

template <typename Return, typename... Args>
FunctionProxy(Return (*)(void *, Args...), void *)
    -> FunctionProxy<Return(Args...)>;

template <typename Return, typename... Args>
FunctionProxy(Return (*)(void const *, Args...), void const *)
    -> FunctionProxy<Return(Args...) const>;

} // namespace CxxPlugins
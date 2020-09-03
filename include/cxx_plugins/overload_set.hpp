/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    overloaded_callable.hpp
 * \author  Andrey Ponomarev
 * \date    07 Jul 2020
 * \brief
 * $BRIEF$
 */
#pragma once

#include "cxx_plugins/functionref.hpp"

#include <functional>

namespace plugins {

template <typename Signature> using Function = std::function<Signature>;

template <typename... Signatures>
class OverloadSet : public Function<Signatures>... {
public:
  static_assert(sizeof...(Signatures) != 0,
                "You should have at least one signature in OverloadSet");
  using Function<Signatures>::operator()...;

  template <typename... Callables,
            typename =
                std::enable_if_t<sizeof...(Callables) == sizeof...(Signatures)>>
  constexpr explicit OverloadSet(Callables &&... callables) noexcept
      : Function<Signatures>(std::forward<Callables>(callables))... {}

  template <typename... Callables,
            typename =
                std::enable_if_t<sizeof...(Callables) == sizeof...(Signatures)>>
  constexpr auto assign(Callables &&... callables) -> OverloadSet & {
    (Function<Signatures>::operator=(callables), ...);
    return *this;
  }
};

template <typename... Callables>
OverloadSet(Callables...) -> OverloadSet<
    utility::SignatureRemoveConstT<utility::SignatureOfT<Callables>>...>;

template <typename... Callables>
constexpr auto makeOverloadSet(Callables &&... callables) {
  return OverloadSet<utility::SignatureRemoveConstT<
      utility::SignatureOfT<std::decay_t<Callables>>>...>{
      std::forward<Callables>(callables)...};
}

template <typename... Signatures>
class OverloadSetRef : public FunctionRef<Signatures>... {
public:
  static_assert(sizeof...(Signatures) != 0,
                "You should have at least one signature in OverloadSet");

  using FunctionRef<Signatures>::operator()...;

  template <typename... Callables,
            typename =
                std::enable_if_t<sizeof...(Callables) == sizeof...(Signatures)>>
  constexpr explicit OverloadSetRef(Callables const &... callables) noexcept
      : FunctionRef<Signatures>(callables)... {}

  template <typename... Callables,
            typename =
                std::enable_if_t<sizeof...(Callables) == sizeof...(Signatures)>>
  constexpr auto assign(Callables const &... callables) -> OverloadSetRef & {
    (FunctionRef<Signatures>::operator=(callables), ...);
    return *this;
  }
};

template <typename... Callables>
OverloadSetRef(Callables...) -> OverloadSetRef<
    utility::SignatureRemoveConstT<utility::SignatureOfT<Callables>>...>;

template <typename... Callables>
constexpr auto makeOverloadSetRef(Callables &&... callables) {
  return OverloadSetRef<utility::SignatureRemoveConstT<
      utility::SignatureOfT<std::decay_t<Callables>>>...>{
      std::forward<Callables>(callables)...};
}

} // namespace CxxPlugins
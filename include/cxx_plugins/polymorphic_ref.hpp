/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    polymorphic_ref.hpp
 * \author  Andrey Ponomarev
 * \date    18 Jun 2020
 * \brief
 * $BRIEF$
 */
#pragma once

#include "cxx_plugins/function_proxy.hpp"
#include "cxx_plugins/vtable.hpp"

namespace CxxPlugins {

template <typename... Tags> class PolymorphicRef;

template <typename... Tags, typename... FunctionSignatures>
class PolymorphicRef<TaggedValue<Tags, FunctionSignatures>...> {
private:
  template <typename U>
  static constexpr bool is_same =
      std::is_same_v<std::decay_t<U>, PolymorphicRef>;

  static constexpr bool is_const =
      (utility::FunctionTraits<FunctionSignatures>::is_const && ...);

  template <typename T> struct underlying;

  template <typename T> struct underlying<T const &> {
    using type = T const &;
  };
  template <typename T> struct underlying<T &> {
    using type = std::conditional_t<is_const, T const &, T &>;
  };
  template <typename T> struct underlying<T &&> {
    using type = std::conditional_t<is_const, T const &, T &&>;
  };
  template <typename T> struct underlying<T const &&> {
    using type = T const &&;
  };

  template <typename T> using underlying_t = typename underlying<T>::type;

public:
  constexpr PolymorphicRef() noexcept = default;
  constexpr PolymorphicRef(PolymorphicRef const &) noexcept = default;
  constexpr PolymorphicRef(PolymorphicRef &&) noexcept = default;
  constexpr auto operator=(PolymorphicRef const &) noexcept
      -> PolymorphicRef & = default;
  constexpr auto operator=(PolymorphicRef &&) noexcept
      -> PolymorphicRef & = default;

  template <typename T, typename = std::enable_if_t<!is_same<T>>>
  constexpr PolymorphicRef(T &&obj) noexcept
      : function_table_p_m(&vtable_v<underlying_t<decltype(obj)>,
                                     TaggedValue<Tags, FunctionSignatures>...>),
        data_p_m(&obj) {}

  template <typename T, typename = std::enable_if_t<!is_same<T>>>
  constexpr PolymorphicRef &operator=(T &&obj) noexcept {
    function_table_p_m = &vtable_v<underlying_t<decltype(obj)>,
                                   TaggedValue<Tags, FunctionSignatures>...>;
    data_p_m = &obj;
  }

  template <typename TagT> constexpr auto operator[](TagT&& t) noexcept {
    return FunctionProxy((*function_table_p_m)[std::forward<TagT>(t)], data_p_m);
  }

  template <typename TagT> constexpr auto operator[](TagT&& t) const noexcept {
    return FunctionProxy((*function_table_p_m)[std::forward<TagT>(t)],
                         const_cast<void const *>(data_p_m));
  }

private:
  VTableT<TaggedValue<Tags, FunctionSignatures>...> *function_table_p_m =
      nullptr;
  std::conditional_t<is_const, void const *, void *> data_p_m = nullptr;
};

} // namespace CxxPlugins

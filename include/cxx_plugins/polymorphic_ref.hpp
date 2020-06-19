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

namespace impl {
template <typename... TaggedSignatures> class PolymorphicRef;
} // namespace impl

template <typename... Ts>
using PolymorphicRef = std::conditional_t<
    (is_tagged_value_v<Ts> && ...), impl::PolymorphicRef<Ts...>,
    impl::PolymorphicRef<TaggedValue<Ts, PolymorphicTagSignatureT<Ts>>...>>;

namespace impl {
template <typename... TaggedSignatures> class PolymorphicRef;

template <typename... Tags, typename... FunctionSignatures>
class PolymorphicRef<TaggedValue<Tags, FunctionSignatures>...> {
private:
  template <typename U>
  static constexpr bool is_self =
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

  template <typename T, typename = std::enable_if_t<!is_self<T>>>
  constexpr PolymorphicRef(T &&obj) noexcept
      : function_table_m(std::in_place_type_t<T>{}), data_p_m(&obj) {}

  template <typename T, typename = std::enable_if_t<!is_self<T>>>
  constexpr PolymorphicRef &operator=(T &&obj) noexcept {
    function_table_m = std::in_place_type_t<T>{};
    data_p_m = &obj;
    return *this;
  }

  template <typename TagT> constexpr auto operator[](TagT &&t) noexcept {
    return FunctionProxy(function_table_m[std::forward<TagT>(t)], data_p_m);
  }

  template <typename TagT> constexpr auto operator[](TagT &&t) const noexcept {
    return FunctionProxy(function_table_m[std::forward<TagT>(t)],
                         const_cast<void const *>(data_p_m));
  }

private:
  VTable<TaggedValue<Tags, FunctionSignatures>...> function_table_m;
  std::conditional_t<is_const, void const *, void *> data_p_m = nullptr;
};
} // namespace impl

} // namespace CxxPlugins

/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugings project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    vtable.hpp
 * \author  Andrey Ponomarev
 * \date    13 May 2020
 * \brief
 * This file contains declaration and implementation for VTable class.
 *
 * \details This class allows storing and accessing functions by tags.
 *
 */
#pragma once

#include "cxx_plugins/function_traits.hpp"
#include "cxx_plugins/polymorphic_traits.hpp"
#include "tuple/tuple_map.hpp"

namespace CxxPlugins {

template <typename T, typename... Ts> struct VTable;
template <typename T, typename... Ts>
static inline auto vtable_v = VTable<T, Ts...>::value;

namespace impl {

template <typename Signature> struct PolymorphicTrampolineType;

template <typename Return, typename... Args>
struct PolymorphicTrampolineType<Return(Args...)> {
  using type = Return (*)(void *, Args...);
};

template <typename Return, typename... Args>
struct PolymorphicTrampolineType<Return(Args...) const> {
  using type = Return (*)(void const *, Args...);
};

template <typename Tag, typename T, typename Signature>
struct PolymorphicTrampoline;

template <typename Tag, typename T, typename Return, typename... Args>
struct PolymorphicTrampoline<Tag, T, Return(Args...)> {
  static constexpr Return call(void *obj_p, Args... args) {

    using underlying_t = std::remove_reference_t<T>;

    if constexpr (std::is_const_v<underlying_t>) {
      static_assert(sizeof(T) == 0,
                    "Specified type is const, but the signature is not."
                    "Leave only const methods for const objects.");
    } else {
      if constexpr (std::is_reference_v<T>) {
        return polymorphicExtend(Tag{},
                                 static_cast<T>(*static_cast<underlying_t *>(obj_p)),
                                 std::forward<Args>(args)...);
      } else {
        return polymorphicExtend(Tag{}, *static_cast<underlying_t *>(obj_p),
                                 std::forward<Args>(args)...);
      }
    }
  }
};

template <typename Tag, typename T, typename Return, typename... Args>
struct PolymorphicTrampoline<Tag, T, Return(Args...) const> {
  static constexpr Return call(void const *obj_p, Args... args) {

    using underlying_t = std::remove_reference_t<T> const;

    if constexpr (std::is_reference_v<T>) {
      using reference_type =
          std::conditional_t<std::is_lvalue_reference_v<T>,
                             std::remove_reference_t<T> const &,
                             std::remove_reference_t<T> const &&>;
      return polymorphicExtend(
          Tag{},
          static_cast<reference_type>(*static_cast<underlying_t *>(obj_p)),
          std::forward<Args>(args)...);
    } else {
      return polymorphicExtend(Tag{}, *static_cast<underlying_t *>(obj_p),
                               std::forward<Args>(args)...);
    }
  }
};

template <typename Tag, typename T, typename Signature>
static constexpr auto polymorphic_trampoline_v =
    &PolymorphicTrampoline<Tag, T, Signature>::call;

template <typename... Ts> struct VTableType;

template <typename... Tags, typename... FunctionSignatures>
struct VTableType<TaggedValue<Tags, FunctionSignatures>...> {
  using type = TupleMap<TaggedValue<
      Tags, typename PolymorphicTrampolineType<FunctionSignatures>::type>...>;
};

} // namespace impl

template <typename T, typename... Tags, typename... FunctionSignatures>
struct VTable<T, TaggedValue<Tags, FunctionSignatures>...> {
  static inline auto value = TupleMap(TaggedValue(
      Tags{}, impl::polymorphic_trampoline_v<Tags, T, FunctionSignatures>)...);
};

template <typename... TaggedValues>
using VTableT = typename impl::VTableType<TaggedValues...>::type;

template <typename Signature>
using PolymorphicTrampolineT =
    typename impl::PolymorphicTrampolineType<Signature>::type;

} // namespace CxxPlugins
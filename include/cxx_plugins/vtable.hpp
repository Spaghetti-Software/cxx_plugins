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

#include "cxx_plugins/definitions.hpp"
#include "cxx_plugins/function_traits.hpp"
#include "cxx_plugins/polymorphic_traits.hpp"
#include "cxx_plugins/type_traits.hpp"
#include "sequence/conversion.hpp"
#include "sequence/map.hpp"

#include <limits>

namespace plugins {

namespace impl {

template <typename Signature> struct PolymorphicTrampolineType;
template <typename Signature>
using PolymorphicTrampolineTypeT =
    typename PolymorphicTrampolineType<Signature>::Type;

template <typename Return, typename... Args>
struct PolymorphicTrampolineType<Return(Args...)> {
  using Type = Return (*)(void *, Args...);
};

template <typename Return, typename... Args>
struct PolymorphicTrampolineType<Return(Args...) const> {
  using Type = Return (*)(void const *, Args...);
};

template <typename Tag, typename T, typename Signature>
struct PolymorphicTrampoline;

template <typename Tag, typename T, typename Return, typename... Args>
struct PolymorphicTrampoline<Tag, T, Return(Args...)> {

  static constexpr Return call(void *obj_p, Args... args) {
    static_assert(!std::is_rvalue_reference_v<T>,
                  "T can't be rvalue reference");
    using underlying_t = std::remove_reference_t<T>;

    if constexpr (std::is_const_v<underlying_t>) {
      static_assert(sizeof(T) == 0,
                    "Specified type is const, but the signature is not."
                    "Leave only const methods for const objects.");
    } else {
      if constexpr (std::is_reference_v<T>) {
        return polymorphicExtend(
            Tag{}, static_cast<T>(*static_cast<underlying_t *>(obj_p)),
            std::forward<Args>(args)...);
      } else {
        return polymorphicExtend(Tag{}, *static_cast<underlying_t *>(obj_p),
                                 std::forward<Args>(args)...);
      }
    }
  }

  static constexpr Return (*value)(void *, Args... args) = &call;
};

template <typename Tag, typename T, typename Return, typename... Args>
struct PolymorphicTrampoline<Tag, T, Return(Args...) const> {
  static constexpr Return call(void const *obj_p, Args... args) {
    static_assert(!std::is_rvalue_reference_v<T>,
                  "Ta can't be rvalue reference");
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
  static constexpr Return (*value)(void const *, Args... args) = &call;
};

template <typename Tag, typename T, typename Signature>
static constexpr auto polymorphic_trampoline_v =
    PolymorphicTrampoline<Tag, T, Signature>::value;

} // namespace impl

template <typename Signature>
using PolymorphicTrampolineT =
    typename impl::PolymorphicTrampolineType<Signature>::type;

template <typename T, typename... TaggedValues> struct VTableStorage;

template <typename T, typename... Tags, typename... Signatures>
struct VTableStorage<T, TaggedSignature<Tags, Signatures>...> {
  static constexpr std::size_t size =
      sizeof...(Tags) == 0 ? 1 : sizeof...(Tags);

  using FunctionPtrT = utility::FunctionPointer<void()>;

  static inline const FunctionPtrT value[size] = {
      reinterpret_cast<FunctionPtrT>(
          impl::polymorphic_trampoline_v<Tags, T, Signatures>)...};
};

template <typename... TaggedValues> struct VTable;

template <typename... Tags, typename... Signatures>
struct VTable<TaggedSignature<Tags, Signatures>...> {
public:
  template <typename TagT>
  using FunctionTypeAt =
      utility::ElementType<utility::index_of<TagT, Tags...>,
                           impl::PolymorphicTrampolineTypeT<Signatures>...>;
  using FunctionTablePtrT = utility::FunctionPointer<void()> const *;

  static_assert(sizeof...(Signatures) <=
                    std::numeric_limits<std::uint8_t>::max(),
                "Too many functions. VTable supports up to 256 functions only."
                "Implementation can be changed to support more with "
                "std::conditional_t<size < max, uint8_t, uint16_t>");

  // All tables should be friends for easier construction
  template <typename... Us> friend struct VTable;

  static_assert(utility::are_unique_v<Tags...>, "All tags should be unique");

  constexpr VTable() noexcept = default;
  constexpr VTable(VTable const &) noexcept = default;
  constexpr VTable(VTable &&) noexcept = default;
  constexpr VTable &operator=(VTable const &) noexcept = default;
  constexpr VTable &operator=(VTable &&) noexcept = default;

  constexpr bool isEmpty() const noexcept {
    return function_table_p_m == nullptr;
  }

  void reset() noexcept { function_table_p_m = nullptr; }

  template <typename T>
  constexpr explicit VTable(std::in_place_type_t<T> /*unused*/) noexcept
      : function_table_p_m{VTableStorage<
            T, TaggedSignature<Tags, Signatures>...>::value},
        permutations_m{Sequence::AsStdArray<DefaultSequenceT>::value} {}

  template <
      typename... OtherTags, typename... OtherSignatures,
      bool size_constraint = sizeof...(Tags) >= 1 &&
                             sizeof...(Tags) < sizeof...(OtherTags),
      std::enable_if_t<size_constraint, int> = 0,
      bool other_constraints =
          // Every tag from `this` should be represented in rhs
      (utility::is_in_the_pack_v<Tags, OtherTags...> &&...) &&
      // Every tag from `this` should have the same function type as in
      // rhs
      (std::is_same_v<
          FunctionTypeAt<Tags>,
          typename VTable<TaggedSignature<OtherTags, OtherSignatures>...>::
              template FunctionTypeAt<Tags>> &&...),
      std::enable_if_t<other_constraints, int> = 0>
  constexpr VTable(VTable<TaggedSignature<OtherTags, OtherSignatures>...> const
                       &rhs) noexcept
      : function_table_p_m{rhs.function_table_p_m},
        permutations_m{Sequence::AsStdArray<std::integer_sequence<
            std::uint8_t, utility::index_of<Tags, OtherTags...>...>>::value} {}

  template <
      typename... OtherTags, typename... OtherSignatures,
      bool size_constraint = sizeof...(Tags) >= 1 &&
                             sizeof...(Tags) < sizeof...(OtherTags),
      std::enable_if_t<size_constraint, int> = 0,
      bool other_constraints =
          // Every tag from `this` should be represented in rhs
      (!utility::is_in_the_pack_v<Tags, OtherTags...> || ...) ||
      // Every tag from `this` should have the same function type as in
      // rhs
      (!std::is_same_v<
           FunctionTypeAt<Tags>,
           typename VTable<TaggedSignature<OtherTags, OtherSignatures>...>::
               template FunctionTypeAt<Tags>> ||
       ...),
      std::enable_if_t<other_constraints, unsigned> = 0>
  constexpr VTable(VTable<TaggedSignature<OtherTags, OtherSignatures>...> const
                       & /*unused*/) noexcept {

    static_assert((utility::is_in_the_pack_v<Tags, OtherTags...> && ...),
                  "Every tag from `this` should be represented in rhs");
    static_assert(
        (std::is_same_v<
             FunctionTypeAt<Tags>,
             typename VTable<TaggedSignature<OtherTags, OtherSignatures>...>::
                 template FunctionTypeAt<Tags>> &&
         ...),
        "Every tag from `this` should have the same function signature as in "
        "rhs");
    static_assert(
        sizeof(decltype(*this)) == 0,
        "Please check previous messages. This assert exists to ensure that "
        "implementation is correct and this constructor is never called.");
  }

  template <
      typename... OtherTags, typename... OtherSignatures,
      bool size_constraint = sizeof...(Tags) >= 1 &&
                             sizeof...(Tags) == sizeof...(OtherTags),
      std::enable_if_t<size_constraint, unsigned> = 0,
      bool other_constraints =
          // tags should have different order
      (!std::is_same_v<Tags, OtherTags> || ...) &&
      // Every tag from `this` should be represented in rhs
      (utility::is_in_the_pack_v<Tags, OtherTags...> &&...) &&
      // Every tag from `this` should have the same function type as in
      // rhs
      (std::is_same_v<
          FunctionTypeAt<Tags>,
          typename VTable<TaggedSignature<OtherTags, OtherSignatures>...>::
              template FunctionTypeAt<Tags>> &&...),
      std::enable_if_t<other_constraints, int> = 0>
  constexpr VTable(VTable<TaggedSignature<OtherTags, OtherSignatures>...> const
                       &rhs) noexcept
      : function_table_p_m{rhs.function_table_p_m},
        permutations_m{Sequence::AsStdArray<std::integer_sequence<
            std::uint8_t, utility::index_of<Tags, OtherTags...>...>>::value} {}

  template <
      typename... OtherTags, typename... OtherSignatures,
      bool size_constraint = sizeof...(Tags) >= 1 &&
                             sizeof...(Tags) == sizeof...(OtherTags),
      std::enable_if_t<size_constraint, unsigned> = 0,
      bool other_constraints =
          // tags should have different order
      (!std::is_same_v<Tags, OtherTags> || ...) &&
      (
          // Every tag from `this` should be represented in rhs
          (!utility::is_in_the_pack_v<Tags, OtherTags...> || ...) ||
          // Every tag from `this` should have the same function type as in
          // rhs
          (!std::is_same_v<
               FunctionTypeAt<Tags>,
               typename VTable<TaggedSignature<OtherTags, OtherSignatures>...>::
                   template FunctionTypeAt<Tags>> ||
           ...)),
      std::enable_if_t<other_constraints, unsigned> = 0>
  constexpr VTable(VTable<TaggedSignature<OtherTags, OtherSignatures>...> const
                       & /*unused*/) noexcept {
    static_assert((utility::is_in_the_pack_v<Tags, OtherTags...> && ...),
                  "Every tag from `this` should be represented in rhs");
    static_assert(
        (std::is_same_v<
             FunctionTypeAt<Tags>,
             typename VTable<TaggedSignature<OtherTags, OtherSignatures>...>::
                 template FunctionTypeAt<Tags>> &&
         ...),
        "Every tag from `this` should have the same function signature as in "
        "rhs");
    static_assert(
        sizeof(decltype(*this)) == 0,
        "Please check previous messages. This assert exists to ensure that "
        "implementation is correct and this constructor is never called.");
  }

  template <typename T>
  constexpr VTable &operator=(std::in_place_type_t<T> /*unused*/) noexcept {
    function_table_p_m =
        VTableStorage<T, TaggedSignature<Tags, Signatures>...>::value;
    permutations_m = Sequence::AsStdArray<DefaultSequenceT>::value;

    return *this;
  }

  template <typename... OtherTags, typename... OtherSignatures,
            bool constraints =
                // Tags >= 1, because otherwise it is a default constructor
            sizeof...(Tags) >= 1 &&
            // Number of this tags should be strictly less then rhs
            // if equal copy assignment operator should be called instead
            (sizeof...(Tags) < sizeof...(OtherTags)),
            std::enable_if_t<constraints, int> = 0>
  constexpr VTable &
  operator=(VTable<TaggedSignature<OtherTags, OtherSignatures>...> const
                &rhs) noexcept {
    static_assert((utility::is_in_the_pack_v<Tags, OtherTags...> && ...),
                  "Every tag from `this` should be represented in rhs");
    static_assert(
        (std::is_same_v<
             FunctionTypeAt<Tags>,
             typename VTable<TaggedSignature<OtherTags, OtherSignatures>...>::
                 template FunctionTypeAt<Tags>> &&
         ...),
        "Every tag from `this` should have the same function signature as in "
        "rhs");
    function_table_p_m = rhs.function_table_p_m;
    permutations_m = Sequence::AsStdArray<std::integer_sequence<
        std::uint8_t, utility::index_of<Tags, OtherTags...>...>>::value;
    return *this;
  }

  template <typename TagT>
  inline auto operator[](TagT && /*unused*/) const noexcept
      -> FunctionTypeAt<std::decay_t<TagT>> {

    cxxPluginsAssert(!isEmpty(), "Trying to get function for empty VTable");

    using tag_t = std::decay_t<TagT>;
    static_assert(utility::is_in_the_pack_v<tag_t, Tags...>,
                  "Tag should be in the pack");
    constexpr auto index = utility::index_of<tag_t, Tags...>;
    return reinterpret_cast<FunctionTypeAt<tag_t> const>(
        function_table_p_m[permutations_m[index]]);
  }

private:
  template <typename TagT>
  static constexpr unsigned index = utility::index_of<TagT, Tags...>;

  using DefaultSequenceT = std::make_integer_sequence<uint8_t, sizeof...(Tags)>;
  static constexpr std::size_t perm_size =
      sizeof...(Tags) == 0 ? 1 : sizeof...(Tags);

  FunctionTablePtrT function_table_p_m = nullptr;
  std::array<std::uint8_t, sizeof...(Tags)> permutations_m = {};
};

template <typename... TaggedSignatures> struct PrimitiveVTable;

template <typename... Tags, typename... Signatures>
/*!
 * \brief
 * SimpleVtable is the same thing as VTable, but it doesn't allow upcasting,
 * because it doesn't have permutation tables.
 */
struct PrimitiveVTable<TaggedSignature<Tags, Signatures>...> {
public:
  template <typename TagT>
  using FunctionTypeAt =
      utility::ElementType<utility::index_of<TagT, Tags...>,
                           impl::PolymorphicTrampolineTypeT<Signatures>...>;
  using FunctionTablePtrT = utility::FunctionPointer<void()> const *;

  static_assert(sizeof...(Signatures) <=
                    std::numeric_limits<std::uint8_t>::max(),
                "Too many functions. VTable supports up to 256 functions only."
                "Implementation can be changed to support more with "
                "std::conditional_t<size < max, uint8_t, uint16_t>");

  static_assert(utility::are_unique_v<Tags...>, "All tags should be unique");

  constexpr PrimitiveVTable() noexcept = default;
  constexpr PrimitiveVTable(PrimitiveVTable const &) noexcept = default;
  constexpr PrimitiveVTable(PrimitiveVTable &&) noexcept = default;
  constexpr PrimitiveVTable &
  operator=(PrimitiveVTable const &) noexcept = default;
  constexpr PrimitiveVTable &operator=(PrimitiveVTable &&) noexcept = default;

  constexpr bool isEmpty() const noexcept {
    return function_table_p_m == nullptr;
  }

  void reset() noexcept { function_table_p_m = nullptr; }

  template <typename T>
  constexpr explicit PrimitiveVTable(
      std::in_place_type_t<T> /*unused*/) noexcept
      : function_table_p_m{
            VTableStorage<T, TaggedSignature<Tags, Signatures>...>::value} {}

  template <typename T>
  constexpr PrimitiveVTable &
  operator=(std::in_place_type_t<T> /*unused*/) noexcept {
    function_table_p_m =
        VTableStorage<T, TaggedSignature<Tags, Signatures>...>::value;
    return *this;
  }

  template <typename TagT>
  inline auto operator[](TagT && /*unused*/) const noexcept
      -> FunctionTypeAt<std::decay_t<TagT>> {

    cxxPluginsAssert(!isEmpty(), "Trying to get function for empty VTable");

    using tag_t = std::decay_t<TagT>;
    static_assert(utility::is_in_the_pack_v<tag_t, Tags...>,
                  "Tag should be in the pack");
    constexpr auto index = utility::index_of<tag_t, Tags...>;
    return reinterpret_cast<FunctionTypeAt<tag_t> const>(
        function_table_p_m[index]);
  }

private:
  template <typename TagT>
  static constexpr unsigned index = utility::index_of<TagT, Tags...>;

  using DefaultSequenceT = std::make_integer_sequence<uint8_t, sizeof...(Tags)>;
  static constexpr std::size_t perm_size =
      sizeof...(Tags) == 0 ? 1 : sizeof...(Tags);

  FunctionTablePtrT function_table_p_m = nullptr;
};

} // namespace CxxPlugins
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
 *
 * \todo Finish documentation
 * \todo Split file(too many templates related only to implementation)
 */
#pragma once

#include "cxx_plugins/definitions.hpp"
#include "cxx_plugins/function_traits.hpp"
#include "cxx_plugins/tuple.hpp"

namespace CxxPlugins {

template <typename Tag, typename Fn> struct Entry {
  static_assert(
      std::is_pod_v<Tag>,
      "Tag should be a simple type(preferably without members at all)");
  static_assert(utility::is_callable_v<Fn>, "Fn should be callable.");
  Tag tag_m;
  Fn fn_m;
};

template <typename Tag, typename Return, typename... Args>
struct Entry<Tag, Return(Args...)> : public Entry<Tag, Return (*)(Args...)> {};

template <typename T> struct IsEntry : public std::false_type {};
template <typename Tag, typename Fn>
struct IsEntry<Entry<Tag, Fn>> : public std::true_type {};

template <typename T> static constexpr bool is_entry = IsEntry<T>::value;

template <typename... Entries> class VTable;

template <typename T, typename... Ts> struct Index;

template <typename T> struct Index<T> {
  static_assert(sizeof(T) != 0, "Index is out of bounds");
};

template <typename T, typename... Ts>
struct Index<T, T, Ts...> : std::integral_constant<std::size_t, 0> {};

template <typename T, typename U, typename... Ts>
struct Index<T, U, Ts...>
    : std::integral_constant<std::size_t, 1 + Index<T, Ts...>::value> {};

template <typename... Types> struct AreUnique;

template <typename First, typename... Rest> struct AreUnique<First, Rest...> {
  static constexpr bool value = (!std::is_same_v<First, Rest> && ...);
};

template <typename... Types>
static constexpr bool are_unique_v = AreUnique<Types...>::value;

template <typename... Tags, typename... FunctionTypes>
class VTable<Entry<Tags, FunctionTypes>...> {
public:
  static_assert(are_unique_v<Tags...>, "All tags should be unique");
  static_assert((utility::is_callable_v<FunctionTypes> && ...),
                "All functions should be callable");
  using TableType = Tuple<FunctionTypes...>;

private:
  template <typename T>
  static constexpr size_t index_v = Index<T, Tags...>::value;

  template <typename T>
  using FunctionTypeAt = TupleElement<index_v<T>, TableType>;

public:
  constexpr VTable() = default;

  constexpr VTable(VTable const &) noexcept = default;
  constexpr VTable(VTable &&) noexcept = default;

  constexpr explicit VTable(FunctionTypes... functions) noexcept
      : functions_m(std::move(functions)...) {}

  /*!
   * "Upcast" conversion constructor
   *
   * Allows conversion from VTable<fn0,fn1,fn3> to VTable<fn0,fn1>
   * @tparam UTags          "derived" tags
   * @tparam UFunctionTypes "derived" functions
   * @param rhs             "base" vtable
   */
  template <typename... UTags, typename... UFunctionTypes>
  constexpr explicit VTable(
      VTable<Entry<UTags, UFunctionTypes>...> const &rhs) noexcept
      : functions_m(rhs.template getFunction<Tags>()...) {}
  template <typename... UTags, typename... UFunctionTypes>
  constexpr explicit VTable(
      VTable<Entry<UTags, UFunctionTypes>...> &&
          rhs) noexcept((std::is_nothrow_constructible_v<FunctionTypes> && ...))
      : functions_m(rhs.template getFunction<Tags>()...) {}

  constexpr auto operator=(VTable const &) noexcept -> VTable & = default;
  constexpr auto operator=(VTable &&) noexcept -> VTable & = default;

  /*!
   * "Upcast" conversion assignment
   *
   * Allows conversion from VTable<fn0,fn1,fn3> to VTable<fn0,fn1>
   * @tparam UTags          "derived" tags
   * @tparam UFunctionTypes "derived" functions
   * @param  rhs             "base" vtable
   */
  template <typename... UTags, typename... UFunctionTypes>
  constexpr auto
  operator=(VTable<Entry<UTags, UFunctionTypes>...> const &rhs) noexcept
      -> VTable & {
    assign(rhs.template getFunction<Tags>()...);
    return *this;
  }
  template <typename... UTags, typename... UFunctionTypes>
  constexpr auto
  operator=(VTable<Entry<UTags, UFunctionTypes>...> &&rhs) noexcept
      -> VTable & {
    assign(rhs.template getFunction<Tags>()...);
    return *this;
  }

  template <typename... UFunctionTypes>
  constexpr void assign(UFunctionTypes &&... functions) noexcept {
    static_assert((std::is_convertible_v<UFunctionTypes, FunctionTypes> && ...),
                  "Functions should be convertible");
    functions_m = {std::forward<UFunctionTypes>(functions)...};
  }

  template <typename TagType>
  constexpr auto getFunction() const noexcept -> FunctionTypeAt<TagType> {
    return get<index_v<TagType>>(functions_m);
  }
  template <typename TagType>
  constexpr auto getFunction() noexcept -> FunctionTypeAt<TagType> & {
    return get<index_v<TagType>>(functions_m);
  }

  template <typename Tag, typename... Args>
  auto call(Args &&... args) const ->
      typename utility::FunctionTraits<FunctionTypeAt<Tag>>::ReturnType {
    cxxPluginsAssert(getFunction<Tag>() != nullptr,
                     "Trying to call function that is nullptr");
    return getFunction<Tag>()(std::forward<Args>(args)...);
  }

  template <typename TagType>
  auto operator[]([[maybe_unused]] const TagType &tag) const noexcept
      -> decltype(getFunction<TagType>()) {
    return getFunction<std::decay_t<TagType>>();
  }

  template <typename TagType>
  auto operator[]([[maybe_unused]] const TagType &tag) noexcept
      -> decltype(getFunction<TagType>()) {
    return getFunction<std::decay_t<TagType>>();
  }

  void reset() noexcept(std::is_nothrow_default_constructible_v<TableType>
                            &&std::is_nothrow_copy_assignable_v<TableType>) {
    functions_m = TableType();
  }

private:
  TableType functions_m = {};
};

template <typename T> struct IsVTable : std::false_type {};

template <typename... TArgs>
struct IsVTable<VTable<TArgs...>> : std::true_type {};

template <typename T> static constexpr bool is_vtable = IsVTable<T>::value;

template <typename Tag, typename FunctionType>
constexpr auto makeEntry(Tag &&tag, FunctionType &&functionType) {
  return Entry<std::decay_t<Tag>, std::decay_t<FunctionType>>{
      std::forward<Tag>(tag), std::forward<FunctionType>(functionType)};
}

template <typename... Tags, typename... FunctionTypes>
constexpr auto makeVTable(Entry<Tags, FunctionTypes> &&... entries) {
  using VTableT = VTable<std::decay_t<Entry<Tags, FunctionTypes>>...>;
  return VTableT(std::forward<Entry<Tags, FunctionTypes>>(entries).fn_m...);
}

template <typename... Tags, typename... FunctionTypes>
constexpr auto makeVTable(VTable<Entry<Tags, FunctionTypes>...> rhs) {
  return rhs;
}

template <typename... Tags, typename... FunctionTypes, typename... NewTags>
constexpr auto makeVTableSubset(VTable<Entry<Tags, FunctionTypes>...> &&another,
                                NewTags... newTags) {
  return makeVTable(makeEntry(newTags, another[newTags])...);
}

template <typename... Tags, typename... FunctionTypes, typename... NewTags>
constexpr auto
makeVTableSubset(VTable<Entry<Tags, FunctionTypes>...> const &another,
                 NewTags... newTags) {
  return makeVTable(makeEntry(newTags, another[newTags])...);
}

template <typename... TTags, typename... TFunctionTypes, typename... UTags,
          typename... UFunctionTypes>
auto makeVTableSuperset(VTable<Entry<TTags, TFunctionTypes>...> const &lhs,
                        VTable<Entry<UTags, UFunctionTypes>...> const &rhs) {
  static_assert((are_unique_v<TTags..., UTags...>),
                "All tags should be unique, please remove similar tags before "
                "concatenation");
  using NewVtableType =
      VTable<Entry<TTags, TFunctionTypes>..., Entry<UTags, UFunctionTypes>...>;
  return NewVtableType(lhs.template getFunction<TTags>()...,
                       rhs.template getFunction<UTags>()...);
}

template <typename First, typename Second, typename Third, typename... Rest>
auto makeVTableSuperset(First const &first, Second const &second,
                        Third const &third, Rest const &... rest) {
  static_assert(
      is_vtable<std::decay_t<First>> && is_vtable<std::decay_t<Second>> &&
          is_vtable<std::decay_t<Third>> &&
          (is_vtable<std::decay_t<Rest>> && ...),
      "All types passed in this function should be specializations of VTable");
  return makeVTableSuperset(makeVTableSuperset(first, second), third, rest...);
}

} // namespace CxxPlugins

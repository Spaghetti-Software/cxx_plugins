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
#include "cxx_plugins/tuple.hpp"
#include "cxx_plugins/type_traits.hpp"

namespace CxxPlugins {

/*!
 * \brief  Entry struct for VTable
 * \tparam Tag  Tag type. Preferably should be a pod type. As it passed by
 * value.
 * \tparam Fn   Function type.
 * \note   Use makeEntry() to create `Entry<tag_t,fn_ptr_t>`
 * \details # Example
 * ```cpp
 * struct Foo {};
 * static constexpr Foo foo_tag;
 * void fooFn() {}
 * ...
 * void bar() {
 *  auto entry = makeEntry(foo_tag, fooFn);
 * }
 * ```
 */
template <typename Tag, typename Fn> struct Entry {
  static_assert(utility::is_callable_v<Fn>, "Fn should be callable.");
  //! \brief Tag type passed into class
  using TagType = Tag;
  //! \brief Function type passed into class
  using FnType = Fn;
  //! \brief Function value
  FnType fn_m;
};

template <typename Tag, typename Return, typename... Args>
struct Entry<Tag, Return(Args...)> : public Entry<Tag, Return (*)(Args...)> {};

template <typename... Entries> class VTable;

/*!
 * \brief  Storage for functions.
 * \tparam Tags  Tag type. Preferably should be a pod type. As it passed by
 * value.
 * \tparam FunctionTypes  Can be functors(e.g. `std::function` or function
 * pointers)
 * \details
 * The main purpose of this class is creating custom vtable variables/pointers.
 * Or creating tables of global functions.
 * For example you have your C API:
 * ```cpp
 * // API
 * extern "C" {
 * void foo(void* object);
 * void bar(void* object, int value);
 * }
 * ...
 *
 * // Implementation
 * extern "C" {
 * void fooImpl(void* object);
 * void bar(void* object, int value);
 * }
 * ```
 *
 * Then you can do following steps to simplify usage of your api:
 * ```cpp
 * // Extension to API
 * extern "C" {
 * struct FooTag{};
 * struct BarTag{};
 * }
 *
 * #ifdef __cplusplus
 * static constexpr FooTag foo_tag;
 * static constexpr BarTag bar_tag;
 * #endif
 * ...
 *
 * // Generating table
 * auto createMyApiImpl() {
 *  return makeVTable(
 *      return makeVTable(makeEntry(foo_tag, fooFn), makeEntry(bar_tag, barFn));
 *  );
 * }
 * ...
 *
 * // using API
 * void usage() {
 *  auto some_object;
 *  auto my_table = createMyApiImpl();
 *  my_table[foo_tag](&some_object);
 *  my_table[bar_tag](&some_object, 4);
 * }
 * ```
 */
#ifdef DOXYGEN
template <typename... Entries> class VTable {
#else
template <typename... Tags, typename... FunctionTypes>
class VTable<Entry<Tags, FunctionTypes>...> {
#endif
public:
  static_assert(utility::are_unique_v<Tags...>, "All tags should be unique");
  static_assert((utility::is_callable_v<FunctionTypes> && ...),
                "All functions should be callable");
  //! \brief Underlying type of the table
  using TableType = Tuple<FunctionTypes...>;

private:
  template <typename T>
  static constexpr size_t index_v = utility::index_of<T, Tags...>;

  template <typename T>
  using FunctionTypeAt = TupleElementType<index_v<T>, TableType>;

public:
  //! \brief Default ctor
  constexpr VTable() = default;

  //! \brief Copy ctor
  constexpr VTable(VTable const &) noexcept = default;
  //! Move ctor
  constexpr VTable(VTable &&) noexcept = default;

  //! \brief Conversion constructor from functions
  constexpr explicit VTable(FunctionTypes... functions) noexcept
      : functions_m(std::move(functions)...) {}

  /*!
   * \brief "Upcast" conversion constructor
   *
   * Allows conversion from VTable<fn0,fn1,fn3> to VTable<fn0,fn1>
   * \tparam UEntries  Other entries
   * \param rhs        "base" vtable
   */
  template <typename... UEntries>
  constexpr explicit VTable(VTable<UEntries...> const &rhs) noexcept
      : functions_m(rhs.template getFunction<Tags>()...) {}

  //! \overload
  template <typename... UEntries>
  constexpr explicit VTable(VTable<UEntries...> &&rhs) noexcept
      : functions_m(rhs.template getFunction<Tags>()...) {}

  //! \brief Copy assignment
  constexpr auto operator=(VTable const &) noexcept -> VTable & = default;
  //! \brief Move assignment
  constexpr auto operator=(VTable &&) noexcept -> VTable & = default;

  /*!
   * \brief "Upcast" conversion assignment
   *
   * Allows conversion from VTable<fn0,fn1,fn3> to VTable<fn0,fn1>
   * \tparam UEntries   Other Entries
   * \param  rhs        "base" vtable
   */
  template <typename... UEntries>
  constexpr auto operator=(VTable<UEntries...> const &rhs) noexcept
      -> VTable & {
    assign(rhs.template getFunction<Tags>()...);
    return *this;
  }
  //! \overload
  template <typename... UEntries>
  constexpr auto operator=(VTable<UEntries...> &&rhs) noexcept -> VTable & {
    assign(rhs.template getFunction<Tags>()...);
    return *this;
  }

  /*!
   * \brief
   * Conversion assignment from functions.
   * (works like conversion constructor)
   */
  template <typename... UFunctionTypes>
  constexpr auto assign(UFunctionTypes &&... functions) noexcept -> VTable & {
    static_assert((std::is_convertible_v<UFunctionTypes, FunctionTypes> && ...),
                  "Functions should be convertible");
    functions_m = {std::forward<UFunctionTypes>(functions)...};
    return *this;
  }

  //! \brief Get function (you can use subscript operator instead)
  template <typename TagType>
  constexpr auto getFunction() const noexcept -> FunctionTypeAt<TagType> {
    return get<index_v<TagType>>(functions_m);
  }
  //! \overload
  template <typename TagType>
  constexpr auto getFunction() noexcept -> FunctionTypeAt<TagType> & {
    return get<index_v<TagType>>(functions_m);
  }

  //! \brief Call function (you can use subscript operator instead)
  template <typename Tag, typename... Args>
  auto call(Args &&... args) const ->
      typename utility::FunctionTraits<FunctionTypeAt<Tag>>::ReturnType {
    cxxPluginsAssert(getFunction<Tag>() != nullptr,
                     "Trying to call function that is nullptr");
    return getFunction<Tag>()(std::forward<Args>(args)...);
  }

  /*!
   * \brief Get function. Allows assignment of functions too.
   * \param tag Tag that is one of the tags provided into the class
   * \details
   * Usage:
   *
   * ```cpp
   * table[my_tag_value](function_args...);
   * ```
   * `my_tag_value` can be constexpr
   */
  template <typename TagType>
  auto operator[]([[maybe_unused]] const TagType &tag) const noexcept
      -> decltype(getFunction<TagType>()) {
    return getFunction<std::decay_t<TagType>>();
  }
  //! \overload
  template <typename TagType>
  auto operator[]([[maybe_unused]] const TagType &tag) noexcept
      -> decltype(getFunction<TagType>()) {
    return getFunction<std::decay_t<TagType>>();
  }

  /*!
   * \brief Calls default constructor for all function types
   * \note You can create functional objects that have default functions.
   * That will allow you to have VTable that is never empty.
   */
  void reset() noexcept(std::is_nothrow_default_constructible_v<TableType>
                            &&std::is_nothrow_copy_assignable_v<TableType>) {
    functions_m = TableType();
  }

private:
  TableType functions_m = {};
};

/*!
 * \brief Helper class that allows to detect
 * VTable instantions in template code.
 */
template <typename T> struct IsVTable : std::false_type {};

template <typename... TArgs>
struct IsVTable<VTable<TArgs...>> : std::true_type {};

/*!
 * \brief   Alias for the value of IsVTable<T>
 * \relates IsVTable
 */
template <typename T> static constexpr bool is_vtable = IsVTable<T>::value;

/*!
 * \brief Simplifies creation of Entries
 * \param tag   Simple type that represents tag
 * \param function Function pointer or functor
 * \return New Entry object
 * \relates Entry
 */
template <typename Tag, typename FunctionType>
constexpr auto makeEntry([[maybe_unused]] Tag &&tag, FunctionType &&function) {
  return Entry<std::decay_t<Tag>, std::decay_t<FunctionType>>{
      std::forward<FunctionType>(function)};
}

/*!
 * \brief  Simplifies creation of VTable
 * \param entries List of entries(can be created using makeEntry)
 * \return New VTable object
 * \relates VTable
 * \details
 * Usage:
 *
 * ```cpp
 * auto table = makeVTable(
 *      makeEntry(my_tag, my_function)
 *      ...
 * );
 * ```
 */
template <typename... Tags, typename... FunctionTypes>
constexpr auto makeVTable(Entry<Tags, FunctionTypes> const &... entries) {
  using VTableT = VTable<std::decay_t<Entry<Tags, FunctionTypes>>...>;
  return VTableT(entries.fn_m...);
}

template <typename... Tags, typename... FunctionTypes>
constexpr auto makeVTable(Entry<Tags, FunctionTypes> &&... entries) {
  using VTableT = VTable<std::decay_t<Entry<Tags, FunctionTypes>>...>;
  return VTableT(std::move(entries.fn_m)...);
}

/*!
 * \brief  Copy vtable into new object
 * \relates VTable
 */
template <typename... Entries>
constexpr auto makeVTable(VTable<Entries...> rhs) {
  return rhs;
}

/*!
 * \brief Creates vtable that has some tags from the given one.
 * \param old_table The table from which we will get our functions
 * \param newTags   Tags that we want to leave
 * \return new VTable object
 * \relates VTable
 * \details
 * Usage:
 *
 * ```cpp
 * VTable<foo_tag, FooFnType> new_table = makeVTableSubset(table, foo_tag);
 * // or
 * auto new_table = makeVTableSubset(table, foo_tag);
 * ```
 */
template <typename... Entries, typename... NewTags>
constexpr auto makeVTableSubset(VTable<Entries...> const &another,
                                NewTags... newTags) {
  return makeVTable(makeEntry(newTags, another[newTags])...);
}

template <typename... Entries, typename... NewTags>
constexpr auto makeVTableSubset(VTable<Entries...> &&old_table,
                                NewTags... newTags) {
  return makeVTable(makeEntry(newTags, std::move(old_table[newTags]))...);
}

/*!
 * \brief  Combines given vtables into one bigger one
 * \return new VTable object
 * \relates VTable
 * \note   VTables should have unique tags between each other
 * \details
 * Usage:
 *
 * ```cpp
 * auto bigger_table = makeVTableSuperset(table0, table1);
 * ```
 */
#if defined(DOXYGEN)
template <typename FirstTableT, typename SecondTableT, typename... RestVtables>
auto makeVtableSuperset(FirstTableT const &first, SecondTableT const &second,
                        RestVtables const &... rest);
#else
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
#endif

template <typename... TTags, typename... TFunctionTypes, typename... UTags,
          typename... UFunctionTypes>
auto makeVTableSuperset(VTable<Entry<TTags, TFunctionTypes>...> const &lhs,
                        VTable<Entry<UTags, UFunctionTypes>...> const &rhs) {
  static_assert((utility::are_unique_v<TTags..., UTags...>),
                "All tags should be unique, please remove similar tags before "
                "concatenation");
  using NewVtableType =
      VTable<Entry<TTags, TFunctionTypes>..., Entry<UTags, UFunctionTypes>...>;
  return NewVtableType(lhs.template getFunction<TTags>()...,
                       rhs.template getFunction<UTags>()...);
}

} // namespace CxxPlugins

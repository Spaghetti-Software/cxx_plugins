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

#include "tuple/tuple_storage.hpp"

#include <algorithm>
#include <cstddef>
#if defined(__cpp_lib_three_way_comparison)
#include <compare>
#endif

namespace CxxPlugins {

template <typename... Ts> struct Tuple;

template <typename T, typename U> using Pair = std::pair<T, U>;

template <typename T, typename U> constexpr auto makePair(T &&t, U &&u);

template <typename... Types>
constexpr auto forwardAsTuple(Types &&... vals) noexcept -> Tuple<Types &&...>;

template <typename... Types> struct TupleSize<Tuple<Types...>> {
  static constexpr std::size_t value = sizeof...(Types);
};

template <typename U0, typename U1> struct TupleSize<Pair<U0, U1>> {
  static constexpr std::size_t value = 2;
};

template <typename T, typename Fn>
constexpr void tupleForEach(T &&fn, Fn &&tuple);

template <std::size_t I, typename... Us>
constexpr TupleElementT<I, Tuple<Us...>> const &
get(Tuple<Us...> const &tuple) noexcept;

template <std::size_t I, typename... Us>
constexpr TupleElementT<I, Tuple<Us...>> &get(Tuple<Us...> &tuple) noexcept;

template <std::size_t I, typename... Us>
constexpr TupleElementT<I, Tuple<Us...>> &&get(Tuple<Us...> &&tuple) noexcept;

template <std::size_t I, typename... Us>
constexpr TupleElementT<I, Tuple<Us...>> const &&
get(Tuple<Us...> const &&tuple) noexcept;

template <std::size_t I, typename U0, typename U1>
constexpr std::conditional_t<I == 0, U0, U1> const &
get(Pair<U0, U1> const &pair) noexcept {
  static_assert(I <= 1, "I should be 0 or 1");
  if constexpr (I == 0) {
    return pair.first;
  } else {
    return pair.second;
  }
}

template <std::size_t I, typename U0, typename U1>
constexpr std::conditional_t<I == 0, U0, U1> &get(Pair<U0, U1> &pair) noexcept {
  static_assert(I <= 1, "I should be 0 or 1");
  if constexpr (I == 0) {
    return pair.first;
  } else {
    return pair.second;
  }
}

template <std::size_t I, typename U0, typename U1>
constexpr std::conditional_t<I == 0, U0, U1> &&
get(Pair<U0, U1> &&pair) noexcept {
  static_assert(I <= 1, "I should be 0 or 1");
  if constexpr (I == 0) {
    return std::move(pair.first);
  } else {
    return std::move(pair.second);
  }
}

template <std::size_t I, typename U0, typename U1>
constexpr std::conditional_t<I == 0, U0, U1> const &&
get(Pair<U0, U1> const &&pair) noexcept {
  static_assert(I <= 1, "I should be 0 or 1");
  if constexpr (I == 0) {
    return std::move(pair.first);
  } else {
    return std::move(pair.second);
  }
}

/*!
 * \brief
 * PackedTuple is a drop-in replacement for std::tuple.
 * Documentation details were taken from
 * [cppreference.com](https://en.cppreference.com/w/cpp/utility/tuple).
 *
 * \details
 * PackedTuple satisfies
 * [StandardLayoutType](https://en.cppreference.com/w/cpp/named_req/StandardLayoutType)
 * property. Which means that you can use this tuple inside of your
 * \code{cpp} extern 'C' \endcode sections. For example(.cpp file):
 * ```cpp
 * template <typename... T> void print(CxxPlugins::PackedTuple<T...> t) {
 *   tupleForEach(t, [](auto& val){
 *     std::cout << val;
 *   });
 * }
 *
 * extern "C" {
 * auto print_tuple_int_int = print<int, int>;
 * }
 *
 * ```
 *
 *
 * \attention
 * `std::is_standarad_layout_v<PackedTuple<Ts...>>` will give `true` for any
 * template parameters `Ts...`, because internal implementation uses `char`
 * array. In order to determine if `PackedTuple` can safely be used in C context
 * use `CXXPlugins::is_standard_layout_v<T>` instead. It will check if
 * all types of PackedTuple satisfy StandardLayoutType property.
 *
 * \details
 * \note
 * We wanted to specialize `std::is_standard_layout` for our tuple. But
 * the standard
 * [states](https://en.cppreference.com/w/cpp/types/is_standard_layout):
 * > The behavior of a program that adds specializations for
 * > `std::is_standard_layout` or `std::is_standard_layout_v` (since C++17)
 * > is undefined.
 *
 * \details
 * ## Features
 * ### Member functions
 *   + ☑ Regular constructors
 *        + ☑ Default constructor
 *        + ☑ Direct constructor
 *        + ☑ Converting constructor
 *        + ☑ Converting copy-constructor
 *        + ☑ Converting move-constructor
 *        + ☑ Pair copy constructor
 *        + ☑ Pair move Constructor
 *        + ☑ Copy constructor
 *        + ☑ Move constructor
 *   + ☐ `uses_allocator` constructors
 *        + ☐ Default constructor
 *        + ☐ Direct constructor
 *        + ☐ Converting constructor
 *        + ☐ Converting copy-constructor
 *        + ☐ Converting move-constructor
 *        + ☐ Pair copy constructor
 *        + ☐ Pair move Constructor
 *        + ☐ Copy constructor
 *        + ☐ Move constructor
 *   + ☑ Assignment operators
 *        + ☑ Copy assignment operator
 *        + ☑ Move assignment operator
 *        + ☑ Conversion copy assignment operator
 *        + ☑ Conversion move assignment operator
 *        + ☑ Pair copy assignment operator
 *        + ☑ Pair move assignment operator
 *   + ☑ swap
 * ### Non-member functions
 *   + ☐ makeFromTuple
 *   + ☑ apply
 *   + ☑ makeTuple
 *   + ☑ tie
 *   + ☑ forwardAsTuple
 *   + ☑ tupleCat
 *   + ☑ get
 *   + ☐ Comparison operators
 *        + ☐ Three-way comparison
 *        + ☑ Equality operator
 *        + ☑ Not-equality operator
 *        + ☑ Less operator
 *        + ☑ Less equal operator
 *        + ☑ Greater operator
 *        + ☑ Greater equal operator
 *   + ☑ swap
 * ### Helper classes
 *   + ☑ tuple_size (TupleSize)
 *   + ☑ tuple_element (TupleElement)
 *   + ☐ uses_allocator (UsesAllocator)
 *   + ☑ ignore (Ignore)
 * ### Non-standard extensions of tuple
 *   + ☑ tupleForEach
 *   + ☑ apply with multiple tuples
 *   + ☑ tupleForEach with multiple tuples
 */
template <typename... Ts> struct Tuple : private TupleStorage<Ts...> {
private:
  using Parent = TupleStorage<Ts...>;

  template <bool Condition, typename... TArgs> struct Constraints {

    template <typename... Us>
    using IsImplicitConstructible =
        std::bool_constant<(std::is_constructible_v<TArgs, Us> && ...) &&
                           (std::is_convertible_v<Us, TArgs> && ...)>;
    template <typename... Us>
    using IsExplicitConstructible =
        std::bool_constant<(std::is_constructible_v<TArgs, Us> && ...) &&
                           !(std::is_convertible_v<Us, TArgs> && ...)>;

    using IsImplicitDefaultConstructible = std::bool_constant<
        (std::is_default_constructible_v<TArgs> && ...) &&
        (utility::is_implicit_default_constructible_v<TArgs> && ...)>;
    using IsExplicitDefaultConstructible = std::bool_constant<
        (std::is_default_constructible_v<TArgs> && ...) &&
        !(utility::is_implicit_default_constructible_v<TArgs> && ...)>;
  };
  template <typename... TArgs> struct Constraints<false, TArgs...> {
    template <typename... Us>
    using IsImplicitConstructible = std::bool_constant<false>;
    template <typename... Us>
    using IsExplicitConstructible = std::bool_constant<false>;

    using IsImplicitDefaultConstructible = std::bool_constant<false>;
    using IsExplicitDefaultConstructible = std::bool_constant<false>;
  };

  // Noexcept specifiers
  static constexpr bool is_nothrow_default_ctor =
      Parent::is_nothrow_default_ctor;
  static constexpr bool is_nothrow_copy_ctor = Parent::is_nothrow_copy_ctor;
  static constexpr bool is_nothrow_move_ctor = Parent::is_nothrow_move_ctor;
  template <typename... Args>
  static constexpr bool is_nothrow_ctor =
      Parent::template is_nothrow_ctor<Args...>;

  static constexpr bool is_nothrow_copy_assign =
      (std::is_nothrow_copy_assignable_v<Ts> && ...);
  static constexpr bool is_nothrow_move_assign =
      (std::is_nothrow_move_assignable_v<Ts> && ...);
  template <typename... Us>
  static constexpr bool
      is_nothrow_assignable = (std::is_nothrow_assignable_v<Ts, Us> && ...);

  template <typename... Us>
  static constexpr bool is_assignable = (std::is_assignable_v<Ts, Us> && ...);

  template <typename... Us>
  static constexpr bool is_same = (std::is_same_v<Ts, Us> && ...);

  static constexpr bool is_nothrow_swappable =
      (std::is_nothrow_swappable_v<Ts> && ...);

  static constexpr auto sequence = Parent::sequence;

  template <bool Dummy>
  using ImplicitDefaultCtor = std::enable_if_t<
      Constraints<Dummy, Ts...>::IsImplicitDefaultConstructible::value, bool>;

  template <bool Dummy>
  using ExplicitDefaultCtor = std::enable_if_t<
      Constraints<Dummy, Ts...>::IsExplicitDefaultConstructible::value, bool>;

  template <bool Dummy, typename... Args>
  using ImplicitCtor =
      std::enable_if_t<Constraints<Dummy, Ts...>::
                           template IsImplicitConstructible<Args...>::value,
                       bool>;

  template <bool Dummy, typename... Args>
  using ExplicitCtor =
      std::enable_if_t<Constraints<Dummy, Ts...>::
                           template IsExplicitConstructible<Args...>::value,
                       bool>;

public:
  template <std::size_t I, typename... Us>
  friend constexpr TupleElementT<I, Tuple<Us...>> const &
  get(Tuple<Us...> const &tuple) noexcept;

  template <std::size_t I, typename... Us>
  friend constexpr TupleElementT<I, Tuple<Us...>> &
  get(Tuple<Us...> &tuple) noexcept;

  template <std::size_t I, typename... Us>
  friend constexpr TupleElementT<I, Tuple<Us...>> &&
  get(Tuple<Us...> &&tuple) noexcept;

  template <std::size_t I, typename... Us>
  friend constexpr TupleElementT<I, Tuple<Us...>> const &&
  get(Tuple<Us...> const &&tuple) noexcept;

  template <typename... Us> friend struct Tuple;

  /*!
   * \brief  Default constructor. Value-initializes all elements.
   *
   * + This overload only participates in overload resolution if
   * `std::is_default_constructible<Ti>::value` is true for all `i`
   * + The constructor is explicit if and only if `Ti` is not
   * copy-list-initializable from `{}` for at least one `i`.
   */
#ifndef DOXYGEN
  template <typename Dummy = void,
            ImplicitDefaultCtor<std::is_void_v<Dummy>> = true>
#endif
  constexpr Tuple() noexcept(is_nothrow_default_ctor) : Parent() {
  }

  template <typename Dummy = void,
            ExplicitDefaultCtor<std::is_void_v<Dummy>> = true>
  constexpr explicit Tuple() noexcept(is_nothrow_default_ctor) : Parent() {}

  /*!
   * \brief Direct constructor. Initializes each element of the tuple with the
   * corresponding parameter.
   *
   * + This overload only participates in overload resolution if
   * `sizeof...(Types) >= 1` and `std::is_copy_constructible<Ti>::value` is true
   * for all `i`.
   * + This constructor is explicit if and only if `std::is_convertible<const*
   * Ti&, Ti>::value` is false for at least one `i`.
   */
#ifndef DOXYGEN
  template <bool size_constraint = (sizeof...(Ts) >= 1),
            ImplicitCtor<size_constraint, Ts const &...> = true>
#endif
  constexpr Tuple(Ts const &... vals) noexcept(is_nothrow_copy_ctor)
      : Parent(vals...) {
  }

  template <bool size_constraint = (sizeof...(Ts) >= 1),
            ExplicitCtor<size_constraint, Ts const &...> = true>
  constexpr explicit Tuple(Ts const &... vals) noexcept(is_nothrow_copy_ctor)
      : Parent(vals...) {}

  /*!
   * \brief
   * Converting constructor. Initializes each element of the tuple with the
   * corresponding value in `std::forward<UTypes>(args)`.
   *
   * + This overload only participates in overload resolution if
   * `sizeof...(Types)
   * == sizeof...(UTypes)` and `sizeof...(Types) >= 1` and
   * `std::is_constructible<Ti, Ui&&>::value` is true for all `i`.
   * + The constructor is explicit if and only if `std::is_convertible<Ui&&,
   * Ti>::value` is false for at least one `i`.
   */
#ifdef DOXYGEN
  template <typename... Us>
#else
  template <typename... Us,
            bool size_constraint = (sizeof...(Ts) >= 1 &&
                                    sizeof...(Ts) == sizeof...(Us)),
            ImplicitCtor<size_constraint, Us &&...> = true>
#endif
  constexpr Tuple(Us &&... vals) noexcept(is_nothrow_ctor<Us &&...>)
      : Parent(std::forward<Us>(vals)...) {
  }

  template <typename... Us,
            bool size_constraint = (sizeof...(Ts) >= 1 &&
                                    sizeof...(Ts) == sizeof...(Us)),
            ExplicitCtor<size_constraint, Us &&...> = true>
  constexpr explicit Tuple(Us &&... vals) noexcept(is_nothrow_ctor<Us &&...>)
      : Parent(std::forward<Us>(vals)...) {}

  /*!
   * \brief
   * Converting copy-constructor. For all `i` in `sizeof...(UTypes)`,
   * initializes ith element of the tuple with `std::get<i>(other)`.
   * + This overload only participates in overload resolution if
   *   + `sizeof...(Types) == sizeof...(UTypes)` and
   *   + `std::is_constructible_v<Ti, const Ui&>` is `true` for all `i` and
   *   + either
   *     + `sizeof...(Types) != 1` or
   *     + (when `Types...` expands to `T` and `UTypes...` expands to `U`) \n
   *       `std::is_convertible_v<const tuple<U>&, T>`,\n
   *       `std::is_constructible_v<T, const tuple<U>&>`, and `std::is_same_v<T,
   *        U>` are all `false`.
   * + The constructor is `explicit` if and only if `std::is_convertible<const
   *   Ui&, Ti>::value` is `false` for at least one `i`.
   */
#ifdef DOXYGEN
  template <typename... Us>
#else
  template <typename... Us,
            bool requirements =
                (sizeof...(Ts) == sizeof...(Us) &&
                 (sizeof...(Ts) != 1 ||
                  ((!std::is_convertible_v<Tuple<Us...> const &, Ts> && ...) &&
                   (!std::is_constructible_v<Ts, Tuple<Us...> const &> &&
                    ...) &&
                   (!std::is_same_v<Ts, Us> && ...)))),
            ImplicitCtor<requirements, Us const &...> = true>
#endif
  constexpr Tuple(Tuple<Us...> const &other) noexcept(
      is_nothrow_ctor<Us const &...>)
      : Parent(other) {
  }

  template <typename... Us,
            bool requirements =
                (sizeof...(Ts) == sizeof...(Us) &&
                 (sizeof...(Ts) != 1 ||
                  ((!std::is_convertible_v<Tuple<Us...> const &, Ts> && ...) &&
                   (!std::is_constructible_v<Ts, Tuple<Us...> const &> &&
                    ...) &&
                   (!std::is_same_v<Ts, Us> && ...)))),
            ExplicitCtor<requirements, Us const &...> = true>
  constexpr explicit Tuple(Tuple<Us...> const &other) noexcept(
      is_nothrow_ctor<Us const &...>)
      : Parent(other) {}

  /*!
   * \brief
   * Converting move-constructor. For all `i` in `sizeof...(UTypes)`,
   * initializes ith element of the tuple with
   * `std::forward<Ui>(std::get<i>(other))`.
   * + This overload only participates in overload resolution if
   *   + `sizeof...(Types) == sizeof...(UTypes)` and
   *   + `std::is_constructible_v<Ti, Ui&&>` is `true` for all `i` and
   *   + either
   *     + `sizeof...(Types) != 1` or
   *     + (when `Types...` expands to `T` and `UTypes...` expands to `U`) \n
   *       `std::is_convertible_v<tuple<U>, T>`, \n
   *       `std::is_constructible_v<T, tuple<U>>`, and `std::is_same_v<T, U>`
   *        are all `false`.
   * + The constructor is explicit if and only if `std::is_convertible<Ui&&,
   * Ti>::value` is `false` for at least one `i`.
   */
#ifdef DOXYGEN
  template <typename... Us>
#else
  template <typename... Us,
            bool requirements =
                (sizeof...(Ts) == sizeof...(Us) &&
                 (sizeof...(Ts) != 1 ||
                  ((!std::is_convertible_v<Tuple<Us...>, Ts> && ...) &&
                   (!std::is_constructible_v<Ts, Tuple<Us...>> && ...) &&
                   (!std::is_same_v<Ts, Us> && ...)))),
            ImplicitCtor<requirements, Us const &...> = true>
#endif
  constexpr Tuple(Tuple<Us...> &&other) noexcept(is_nothrow_ctor<Us &&...>)
      : Parent(std::move(other)) {
  }

  template <typename... Us,
            bool requirements =
                (sizeof...(Ts) == sizeof...(Us) &&
                 (sizeof...(Ts) != 1 ||
                  ((!std::is_convertible_v<Tuple<Us...>, Ts> && ...) &&
                   (!std::is_constructible_v<Ts, Tuple<Us...>> && ...) &&
                   (!std::is_same_v<Ts, Us> && ...)))),
            ExplicitCtor<requirements, Us const &...> = true>
  constexpr explicit Tuple(Tuple<Us...> &&other) noexcept(
      is_nothrow_ctor<Us &&...>)
      : Parent(std::move(other)) {}

  /*!
   * \brief
   * Pair copy constructor. Constructs a 2-element tuple with the first element
   * constructed from `p.first` and the second element from `p.second`
   * + This overload only participates in overload resolution if
   * `sizeof...(Types) == 2` and \n `std::is_constructible<T0,const U1&>::value`
   * and \n `std::is_constructible<T1, const U2&>::value` are both `true`
   * + The constructor is explicit if and only if `std::is_convertible<const
   * U1&, T0>::value` or \n `std::is_convertible<const U2&, T1>::value` is
   * `false`.
   */
#ifdef DOXYGEN
  template <typename U1, typename U2>
#else
  template <typename U1, typename U2,
            bool size_requirement = sizeof...(Ts) == 2,
            ImplicitCtor<size_requirement, U1 const &, U2 const &> = true>
#endif
  constexpr Tuple(Pair<U1, U2> const &p) noexcept(
      is_nothrow_ctor<U1 const &, U2 const &>)
      : Parent(p) {
  }

  template <typename U1, typename U2,
            bool size_requirement = sizeof...(Ts) == 2,
            ExplicitCtor<size_requirement, U1 const &, U2 const &> = true>
  constexpr explicit Tuple(Pair<U1, U2> const &p) noexcept(
      is_nothrow_ctor<U1 &&, U2 &&>)
      : Parent(p) {}

/*!
 * \brief
 * Pair move constructor. Constructs a 2-element tuple with the first element
 * constructed from `std::forward<U1>(p.first)` and the second element from
 * `std::forward<U2>(p.second)`
 * + This overload only participates in overload resolution if `sizeof...(Types)
 * == 2` and \n `std::is_constructible<T0, U1&&>::value` and
 * `std::is_constructible<T1, U2&&>::value` are both `true`
 * + The constructor is `explicit` if and only if `std::is_convertible<U1&&,
 * T0>::value` or \n `std::convertible<U2&&, T1>::value` is `false`.
 */
#ifdef DOXYGEN
  template <typename U1, typename U2>
#else
  template <typename U1, typename U2,
            bool size_requirement = sizeof...(Ts) == 2,
            ImplicitCtor<size_requirement, U1 &&, U2 &&> = true>
#endif
  constexpr Tuple(Pair<U1, U2> &&p) noexcept(is_nothrow_ctor<U1 &&, U2 &&>)
      : Parent(std::move(p)) {
  }

  template <typename U1, typename U2,
            bool size_requirement = sizeof...(Ts) == 2,
            ExplicitCtor<size_requirement, U1 &&, U2 &&> = true>
  constexpr explicit Tuple(Pair<U1, U2> &&p) noexcept(
      is_nothrow_ctor<U1 &&, U2 &&>)
      : Parent(std::move(p)) {}

  /*!
   * \brief
   * Copy constructor. Initializes each element of the tuple
   * with the corresponding element of other.
   * + This constructor is `constexpr` if every operation it performs is
   * `constexpr`. For the empty tuple `std::tuple<>`, it is constexpr.
   * + `std::is_copy_constructible<Ti>::value` must be true for all i, otherwise
   * the behavior is undefined (until C++20)the program is ill-formed (since
   * C++20).
   */
  constexpr Tuple(Tuple const &) noexcept(is_nothrow_copy_ctor) = default;
  /*!
   * \brief
   * Implicitly-defined move constructor. Initializes each ith element of the
   * tuple with `std::forward<Ui>(std::get<i>(other))`.
   * + This constructor is `constexpr` if every operation it performs is
   * `constexpr`. For the empty tuple `std::tuple<>`, it is `constexpr`.
   * + `std::is_move_constructible<Ti>::value` must be true for all i, otherwise
   * the behavior is undefined (until C++20)this overload does not participate
   * in overload resolution (since C++20).
   */
  constexpr Tuple(Tuple &&) noexcept(is_nothrow_move_ctor) = default;

/*!
 * \brief
 * Copy assignment operator. Assigns each element of other to the corresponding
 * element of `*this`.
 */
#ifndef DOXYGEN
//  template <bool enable = (std::is_copy_assignable_v<Ts> && ...),
//            std::enable_if_t<enable, bool> = true>
#endif
  constexpr Tuple &
  operator=(Tuple const &other) noexcept(is_nothrow_copy_assign) = default;

  /*!
   * \brief
   * Move assignment operator. For all `i`, assigns
   * `std::forward<Ti>(get<i>(other))` to `get<i>(*this)`.
   */
#ifndef DOXYGEN
//  template <bool enable = (std::is_move_assignable_v<Ts> && ...),
//            std::enable_if_t<enable, bool> = true>
#endif
  constexpr Tuple &
  operator=(Tuple &&other) noexcept(is_nothrow_move_assign) = default;

  /*!
   * \brief
   * For all `i`, assigns `std::get<i>(other)` to `std::get<i>(*this)`
   */
#ifdef DOXYGEN
  template <typename... Us>
#else
  template <typename... Us,
            bool enable = sizeof...(Ts) == sizeof...(Us) &&
                          is_assignable<Us const &...> && !is_same<Us...>,
            std::enable_if_t<enable, bool> = true>
#endif
  constexpr Tuple &operator=(Tuple<Us...> const &rhs) noexcept(
      is_nothrow_assignable<Us const &...>) {
    assign(sequence, rhs);
    return *this;
  }

  /*!
   * \brief
   *  For all `i`, assigns `std::forward<Ui>(std::get<i>(other))` to
   * `std::get<i>(*this)`.
   */
#ifdef DOXYGEN
  template <typename... Us>
#else
  template <typename... Us,
            bool enable = sizeof...(Ts) == sizeof...(Us) &&
                          is_assignable<Us &&...> && !is_same<Us...>,
            std::enable_if_t<enable, bool> = true>
#endif
  constexpr Tuple &
  operator=(Tuple<Us...> &&rhs) noexcept(is_nothrow_assignable<Us &&...>) {
    assign(sequence, std::move(rhs));
    return *this;
  }

  /*!
   * \brief
   * Pair copy assignment.
   */
#ifdef DOXYGEN
  template <typename U1, typename U2>
#else
  template <typename U1, typename U2,
            bool enable = sizeof...(Ts) == 2 &&
                          is_assignable<U1 const &, U2 const &>,
            std::enable_if_t<enable, bool> = true>
#endif
  constexpr Tuple &operator=(Pair<U1, U2> const &p) noexcept(
      is_nothrow_assignable<U1 const &, U2 const &>) {
    get<0>(*this) = p.first;
    get<1>(*this) = p.second;
    return *this;
  }

  /*!
   * \brief
   * Pair move assignment.
   */
#ifdef DOXYGEN
  template <typename U1, typename U2>
#else
  template <typename U1, typename U2,
            bool enable = sizeof...(Ts) == 2 && is_assignable<U1 &&, U2 &&>,
            std::enable_if_t<enable, bool> = true>
#endif
  constexpr Tuple &
  operator=(Pair<U1, U2> &&p) noexcept(is_nothrow_assignable<U1 &&, U2 &&>) {
    get<0>(*this) = std::forward<U1>(p.first);
    get<1>(*this) = std::forward<U2>(p.second);
    return *this;
  }

  /*!
   * \brief
   * Calls swap (which might be `std::swap`, or might be found via ADL) for each
   * element in `*this` and its corresponding element in other.
   */
  constexpr void swap(Tuple &other) noexcept(is_nothrow_swappable) {
    swapImpl(other, sequence);
  }

private:
  template <std::size_t... indices>
  constexpr void
  swapImpl(Tuple &other, std::index_sequence<indices...> /*unused*/) noexcept(
      is_nothrow_swappable) {
    using std::swap;
    ((swap(get<indices>(*this), get<indices>(other))), ...);
  }

  template <typename... Us, std::size_t... indices>
  constexpr void
  assign(std::index_sequence<indices...> /*unused*/,
         Tuple<Us...> const
             &other) noexcept(is_nothrow_assignable<Us const &...>) {
    ((get<indices>(*this) = get<indices>(other)), ...);
  }
  template <typename... Us, std::size_t... indices>
  constexpr void
  assign(std::index_sequence<indices...> /*unused*/,
         Tuple<Us...> &&other) noexcept(is_nothrow_assignable<Us &&...>) {
    ((get<indices>(*this) = std::forward<Us>(get<indices>(other))), ...);
  }
  template <typename U1, typename U2>
  constexpr void assign(Pair<U1, U2> const &p) noexcept(
      is_nothrow_assignable<U1 const &, U2 const &>) {
    get<0>(*this) = p.first;
    get<1>(*this) = p.second;
  }
  template <typename U1, typename U2, std::size_t... indices>
  constexpr void
  assign(Pair<U1, U2> &&p) noexcept(is_nothrow_assignable<U1 &&, U2 &&>) {
    get<0>(*this) = std::forward<U1>(p.first);
    get<1>(*this) = std::forward<U2>(p.second);
  }
};

template <typename... Ts> Tuple(Ts...) -> Tuple<Ts...>;

template <class T1, class T2> Tuple(Pair<T1, T2>) -> Tuple<T1, T2>;

template <std::size_t I, typename... Us>
constexpr TupleElementT<I, Tuple<Us...>> const &
get(Tuple<Us...> const &tuple) noexcept {
  return get<I>(static_cast<TupleStorage<Us...> const &>(tuple));
}

template <std::size_t I, typename... Us>
constexpr TupleElementT<I, Tuple<Us...>> &get(Tuple<Us...> &tuple) noexcept {
  return get<I>(static_cast<TupleStorage<Us...> &>(tuple));
}

template <std::size_t I, typename... Us>
constexpr TupleElementT<I, Tuple<Us...>> &&get(Tuple<Us...> &&tuple) noexcept {
  return get<I>(static_cast<TupleStorage<Us...> &&>(tuple));
}

template <std::size_t I, typename... Us>
constexpr TupleElementT<I, Tuple<Us...>> const &&
get(Tuple<Us...> const &&tuple) noexcept {
  return get<I>(static_cast<TupleStorage<Us...> const &&>(tuple));
}

//! \todo Test three-way comparison when compilers will be more feature complete
//! with c++20
#if 0 && defined(__cpp_lib_three_way_comparison)

namespace impl {
template <typename T, typename U>
constexpr auto synthThreeWay(T const &lhs, U const &rhs) {
  if constexpr (std::three_way_comparable_with<T, U>) {
    return lhs <=> rhs;
  } else {
    if constexpr (lhs < rhs) {
      return std::weak_ordering::less;
    } else if constexpr (rhs < lhs) {
      return std::weak_ordering::greater;
    }
    return std::weak_ordering::equivalent;
  }
}

template <typename T, typename U> struct Compare;

template <typename... Ts, typename... Us>
struct Compare<Tuple<Ts...>, Tuple<Us...>> {
  static_assert(sizeof...(Ts) == sizeof...(Us),
                "Sizes of tuples should be same");

  template <typename Seq> struct CommonComparisonCategory;

  template <std::size_t... indices>
  struct CommonComparisonCategory<std::index_sequence<indices...>> {
    using Type = std::common_comparison_category_t<decltype(
        synthThreeWay(std::declval<Ts>(), std::declval<Us>()))...>;
  };

  using ReturnType =
      typename CommonComparisonCategory<std::index_sequence_for<Ts...>>::Type;

  template <std::size_t i = 0>
  static constexpr ReturnType threeWay(Tuple<Ts...> const &lhs,
                                 Tuple<Us...> const &rhs) {

    if constexpr (sizeof...(Ts) == 0) {
      return std::strong_ordering::equivalent;
    } else {
      if constexpr (i + 1 < sizeof...(Ts)) {
        if constexpr (auto c = impl::synthThreeWay(get<i>(lhs), get<i>(rhs));
            c != 0) {
          return c;
        }
        return threeWay<i + 1>(lhs, rhs);
      }
      return impl::synthThreeWay(get<i>(lhs), get<i>(rhs));
    }


  }
};

} // namespace impl

template<typename... Ts, typename... Us>
constexpr auto operator<=>(Tuple<Ts...> const& lhs, Tuple<Us...> const& rhs) {
  return impl::Compare<Tuple<Ts...>, Tuple<Us...>>::threeWay(lhs, rhs);
}

#else
namespace impl {

template <typename T, typename U> struct Compare;

template <typename... Ts, typename... Us>
struct Compare<Tuple<Ts...>, Tuple<Us...>> {
  static_assert(sizeof...(Ts) == sizeof...(Us), "Sizes should be same");

  template <std::size_t i = 0>
  static constexpr bool equal(Tuple<Ts...> const &lhs,
                              Tuple<Us...> const &rhs) {
    if constexpr (i < sizeof...(Ts)) {
      return get<i>(lhs) == get<i>(rhs) && equal<i + 1>(lhs, rhs);
    }
    return true;
  }

  template <std::size_t i = 0>
  static constexpr bool less(Tuple<Ts...> const &lhs, Tuple<Us...> const &rhs) {
    if constexpr (i < sizeof...(Ts)) {
      if (get<i>(lhs) < get<i>(rhs)) {
        return true;
      } else if (get<i>(rhs) < get<i>(lhs)) {
        return false;
      }
      return less<i + 1>(lhs, rhs);
    }
    return false;
  }
};

template <> struct Compare<Tuple<>, Tuple<>> {
  template <std::size_t i = 0>
  static constexpr bool equal(Tuple<> const & /*unused*/,
                              Tuple<> const & /*unused*/) {
    return true;
  }

  template <std::size_t i = 0>
  static constexpr bool less(Tuple<> const & /*unused*/,
                             Tuple<> const & /*unused*/) {
    return false;
  }
};

} // namespace impl

template <typename... Ts, typename... Us>
constexpr bool operator==(Tuple<Ts...> const &lhs, Tuple<Us...> const &rhs) {
  return impl::Compare<Tuple<Ts...>, Tuple<Us...>>::equal(lhs, rhs);
}
template <typename... Ts, typename... Us>
constexpr bool operator<(Tuple<Ts...> const &lhs, Tuple<Us...> const &rhs) {
  return impl::Compare<Tuple<Ts...>, Tuple<Us...>>::less(lhs, rhs);
}
template <typename... Ts, typename... Us>
constexpr bool operator!=(Tuple<Ts...> const &lhs, Tuple<Us...> const &rhs) {
  return !(lhs == rhs);
}
template <typename... Ts, typename... Us>
constexpr bool operator<=(Tuple<Ts...> const &lhs, Tuple<Us...> const &rhs) {
  return !(rhs < lhs);
}
template <typename... Ts, typename... Us>
constexpr bool operator>(Tuple<Ts...> const &lhs, Tuple<Us...> const &rhs) {
  return rhs < lhs;
}
template <typename... Ts, typename... Us>
constexpr bool operator>=(Tuple<Ts...> const &lhs, Tuple<Us...> const &rhs) {
  return !(lhs < rhs);
}
#endif

template <typename... Ts>
constexpr auto makeTuple(Ts &&... vals) noexcept(
    std::is_nothrow_constructible_v<Tuple<std::decay_t<Ts>...>, Ts &&...>) {
  return Tuple<std::decay_t<Ts>...>(std::forward<Ts>(vals)...);
}

template <typename... Types>
constexpr auto forwardAsTuple(Types &&... vals) noexcept -> Tuple<Types &&...> {
  return Tuple<decltype(vals)...>(std::forward<Types>(vals)...);
}

struct IgnoredType {
  template <typename... TArgs>
  constexpr IgnoredType(TArgs &&... /*unused*/) noexcept {}

  template <typename T>
  constexpr IgnoredType &operator=(T && /*unused*/) noexcept {
    return *this;
  }
};

static constexpr IgnoredType ignore = {};

template <class... Ts> constexpr Tuple<Ts &...> tie(Ts &... args) noexcept {
  return Tuple<Ts &...>(args...);
}

template <typename... Ts>
void swap(Tuple<Ts...> &lhs,
          Tuple<Ts...> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
  lhs.swap(rhs);
}

template <typename T, typename U> constexpr auto makePair(T &&t, U &&u) {
  return std::make_pair(std::forward<T>(t), std::forward<U>(u));
}

namespace impl {
template <typename Fn, typename Tuples, std::size_t... inner_indices,
          std::size_t... outer_indices>
constexpr decltype(auto)
applyComplexImpl(Fn &&fn, Tuples &&tuples,
                 std::index_sequence<inner_indices...> /*unused*/,
                 std::index_sequence<outer_indices...> /*unused*/) {
  static_assert(sizeof...(inner_indices) == sizeof...(outer_indices),
                "Apply implementation is wrong.");
  return std::invoke(
      std::forward<Fn>(fn),
      get<outer_indices>(get<inner_indices>(std::forward<Tuples>(tuples)))...);
}

template <typename Fn, typename Tuple, std::size_t... indices>
constexpr decltype(auto)
applySimpleImpl(Fn &&fn, Tuple &&tuple,
                std::index_sequence<indices...> /*unused*/) {
  return std::invoke(std::forward<Fn>(fn),
                     get<indices>(std::forward<Tuple>(tuple))...);
}

} // namespace impl

template <typename Fn, typename Tuple>
constexpr decltype(auto) apply(Fn &&fn, Tuple &&tuple) {
  return impl::applySimpleImpl(
      std::forward<Fn>(fn), std::forward<Tuple>(tuple),
      std::make_index_sequence<tuple_size_v<std::decay_t<Tuple>>>());
}

template <typename Fn, typename FirstTuple, typename... RestTuples>
constexpr decltype(auto) apply(Fn &&fn, FirstTuple &&first,
                               RestTuples &&... rest) {

  using Sizes = std::index_sequence<tuple_size_v<std::decay_t<FirstTuple>>,
                                    tuple_size_v<std::decay_t<RestTuples>>...>;
  using inner_indices = utility::RepeatingIndexSequenceT<0, Sizes>;
  using outer_indices = utility::IntegerSequenceCatT<
      std::make_index_sequence<tuple_size_v<std::decay_t<FirstTuple>>>,
      std::make_index_sequence<tuple_size_v<std::decay_t<RestTuples>>>...>;

  decltype(auto) tuple = forwardAsTuple(std::forward<FirstTuple>(first),
                                        std::forward<RestTuples>(rest)...);

  return impl::applyComplexImpl(std::forward<Fn>(fn), tuple, inner_indices(),
                                outer_indices());
}

namespace impl {
template <typename Fn, typename T, std::size_t... indices>
constexpr void tupleForEachSimple(Fn &&fn, T &&t,
                                  std::index_sequence<indices...> /*unused*/) {
  ((std::invoke(std::forward<Fn>(fn), get<indices>(std::forward<T>(t)))), ...);
}

template <std::size_t I, typename Fn, typename T, std::size_t... indices>
constexpr void tupleForEachComplex(Fn &&fn, T &&t,
                                   std::index_sequence<indices...> /*unused*/) {
  // extract every tuple in tuple
  // and invoke with all ith members
  std::invoke(std::forward<Fn>(fn),
              get<I>(get<indices>(std::forward<T>(t)))...);
}

template <typename Fn, typename T, std::size_t... tuples_indices,
          std::size_t... tuple_member_indices>
constexpr void
tupleForEachComplex(Fn &&fn, T &&t,
                    std::index_sequence<tuple_member_indices...> /*unused*/,
                    std::index_sequence<tuples_indices...> tuples_indices_v) {
  (tupleForEachComplex<tuple_member_indices>(
       std::forward<Fn>(fn), std::forward<T>(t), tuples_indices_v),
   ...);
}

} // namespace impl

template <typename Fn, typename T>
constexpr void tupleForEach(Fn &&fn, T &&tuple) {
  impl::tupleForEachSimple(
      std::forward<Fn>(fn), std::forward<T>(tuple),
      std::make_index_sequence<tuple_size_v<std::decay_t<T>>>());
}

template <typename Fn, typename... Ts>
constexpr void tupleForEach(Fn &&fn, Ts &&... tuples) {

  static_assert(sizeof...(Ts) >= 1, "You should provide at least one tuple");
  static_assert(((tuple_size_v<utility::ElementType<0, std::decay_t<Ts>...>> ==
                  tuple_size_v<std::decay_t<Ts>>)&&...),
                "All tuples should have the same size");

  using tuple_member_indices = std::make_index_sequence<
      tuple_size_v<utility::ElementType<0, std::decay_t<Ts>...>>>;
  using tuple_indices = std::index_sequence_for<Ts...>;

  impl::tupleForEachComplex(std::forward<Fn>(fn),
                            forwardAsTuple(std::forward<Ts>(tuples)...),
                            tuple_member_indices(), tuple_indices());
}

} // namespace CxxPlugins

// Enable structured binding

template <typename... Ts> class std::tuple_size<CxxPlugins::Tuple<Ts...>> {
public:
  static constexpr std::size_t value =
      CxxPlugins::tuple_size_v<CxxPlugins::Tuple<Ts...>>;
};

template <std::size_t I, typename... Ts>
class std::tuple_element<I, CxxPlugins::Tuple<Ts...>> {
public:
  using type = CxxPlugins::TupleElementT<I, CxxPlugins::Tuple<Ts...>>;
};

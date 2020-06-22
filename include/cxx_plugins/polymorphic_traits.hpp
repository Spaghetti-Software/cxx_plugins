/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    polymorphic_traits.hpp
 * \author  Andrey Ponomarev
 * \date    16 Jun 2020
 * \brief
 * $BRIEF$
 */
#pragma once

#include "tuple/tuple_map.hpp"

#include <type_traits>

namespace CxxPlugins {

template <typename T>
/*!
 * \brief
 * This trait exists to disable implicit conversions to/from your type with
 * Tag<Type>.
 */
struct TagTraits {
  static constexpr bool allow_implicit_conversion_ctor = true;
  static constexpr bool allow_implicit_conversion_operator = true;
};

template <typename TagType>
/*!
 * \brief  Tag allows creating wrapper around existing types.
 * \details
 * For example you want to create tag to int, but you want to allocate
 * minimal space for it, then use Tag<int>.
 *
 * Or if you want to automatically create constexpr variables. Then you can use
 * Tag<T> to declare type and tag<T> to use value.
 * ```cpp
 * struct action {};
 *
 * void foo(polymorphic<Tag<action>> const&obj) {
 *    obj[tag<action>]();
 * }
 * ```
 *
 */
struct Tag {
  constexpr Tag() noexcept = default;
  template <typename U,
            typename = std::enable_if_t<
                std::is_same_v<std::decay_t<U>, TagType> &&
                TagTraits<std::decay_t<U>>::allow_implicit_conversion_ctor>>
  constexpr Tag(U && /*unused*/) noexcept {}

  template <typename U, typename = std::enable_if_t<
                            std::is_same_v<U, TagType> &&
                            TagTraits<U>::allow_implicit_conversion_operator>>
  constexpr operator U() const noexcept {
    return TagType{};
  }
};

template <typename TagType> static constexpr Tag<TagType> tag = {};

template <typename TagT>
/*!
 * \brief
 * PolymorphicTagSignature is a trait that can be specialized to specify
 * Signature for a specific tag
 *
 * \details
 * This trait simplifies creation of PolymorphicRef and Polymorphic.
 * Instead of providing signature manually every time like this:
 * ```cpp
 * struct add{};
 *
 * void foo(PolymorphicRef<TaggedValue<add, void(int)>> ref);
 *
 * void bar(PolymorphicRef<TaggedValue<add, void(int)>> lhs,
 *          PolymorphicRef<TaggedValue<add, void(int)>> rhs );
 * //...
 * ```
 * You can do this instead:
 * ```cpp
 * struct add{};
 *
 * template<>
 * struct PolymorphicTagSignature<add> {
 *  using Type = void(int);
 * };
 *
 * void foo(PolymorphicRef<add> ref);
 *
 * void bar(PolymorphicRef<add> lhs, PolymorphicRef<add> rhs);
 *
 * ```
 *
 * \note
 * By default `PolymorphicTagSignature<Tag<TagType>>::Type` references
 * `PolymorphicTagSignature<TagType>::Type`. This is done, so you can define
 * trait just for your type and then use `Tag<Type>`:
 *
 * \details
 * ```cpp
 * struct add{};
 * template<>
 * struct CxxPlugins::PolymorphicTagSignature<add> {
 *      using Type = void(int);
 * };
 * ```
 */
struct PolymorphicTagSignature;

template <typename TagT> struct PolymorphicTagSignature<Tag<TagT>> {
  using Type = typename PolymorphicTagSignature<TagT>::Type;
};
template <typename TagT, typename ValueT>
struct PolymorphicTagSignature<TaggedValue<TagT,ValueT>> {
  using Type = ValueT;
};

template <typename TagT>
using PolymorphicTagSignatureT = typename PolymorphicTagSignature<TagT>::Type;


namespace impl {
  template<typename... TaggedSignatures>
  class PolymorphicRef;
} // namespace impl

template <typename T> struct IsPolymorphicRef : public std::false_type {};
template <typename... TaggedSignatures>
struct IsPolymorphicRef<impl::PolymorphicRef<TaggedSignatures...>>
: public std::true_type {};
template <typename T>
static constexpr bool is_polymorphic_ref = IsPolymorphicRef<T>::value;


template<typename T>
struct IsPolymorphic : public std::false_type {};

template<typename T>
static constexpr bool is_polymorphic = IsPolymorphic<T>::value;



} // namespace CxxPlugins
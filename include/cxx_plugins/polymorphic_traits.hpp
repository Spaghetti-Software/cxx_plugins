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

#include "cxx_plugins/function_traits.hpp"
#include "cxx_plugins/definitions.hpp"

#include <type_traits>

namespace CxxPlugins {


template<typename TagT, typename Signature>
struct TaggedSignature {
  using TagType = TagT;
  using SignatureType = Signature;
  [[gnu::noinline]]
  constexpr TaggedSignature() noexcept {
    cxxPluginsUnreachable("This constructor should never be called");
  }
  [[gnu::noinline]]
  TaggedSignature(TaggedSignature const&) noexcept {
    cxxPluginsUnreachable("This constructor should never be called");
  }
  [[gnu::noinline]]
  TaggedSignature(TaggedSignature &&) noexcept {
    cxxPluginsUnreachable("This constructor should never be called");
  }
  [[gnu::noinline]]
  TaggedSignature& operator=(TaggedSignature const&) noexcept {
    cxxPluginsUnreachable("This constructor should never be called");
  }
  [[gnu::noinline]]
  TaggedSignature& operator=(TaggedSignature&&) noexcept {
    cxxPluginsUnreachable("This constructor should never be called");
  }


};

template<typename T>
struct IsTaggedSignature : public std::false_type {};

template<typename Tag, typename Signature>
struct IsTaggedSignature<TaggedSignature<Tag,Signature>> : public std::true_type {};

template<typename T>
static constexpr bool is_tagged_signature = IsTaggedSignature<T>::value;


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
struct PolymorphicTagSignature<TaggedSignature<TagT, ValueT>> {
  using Type = ValueT;
};

template <typename TagT>
using PolymorphicTagSignatureT = typename PolymorphicTagSignature<TagT>::Type;

namespace impl {
template <typename... TaggedSignatures> class PolymorphicPtr;
template <typename... TaggedSignatures> class PrimitivePolymorphicPtr;
template <std::size_t size, typename... TaggedSignatures> class GenericPolymorphic;
} // namespace impl

template <typename T> struct IsPolymorphicRef : public std::false_type {};
template <typename... TaggedSignatures>
struct IsPolymorphicRef<impl::PolymorphicPtr<TaggedSignatures...>>
    : public std::true_type {};
template <typename... TaggedSignatures>
struct IsPolymorphicRef<impl::PrimitivePolymorphicPtr<TaggedSignatures...>>
    : std::true_type {};

template <typename T>
static constexpr bool is_polymorphic_ref_v = IsPolymorphicRef<T>::value;

template <typename T> struct IsPolymorphic : public std::false_type {};

template <std::size_t size, typename... TaggedSignatures>
struct IsPolymorphic<impl::GenericPolymorphic<size, TaggedSignatures...>>
    : public std::true_type {};

template <typename T>
static constexpr bool is_polymorphic_v = IsPolymorphic<T>::value;

/*!
 * \brief
 * Special type that can be declared in the signature of function
 * of Polymorphic or PolymorphicRef, that needs to take an argument of the same
 * Polymorphic or PolymorphicRef
 */
struct SelfType {};

template <typename PolymorphicType, typename Signature>
struct PolymorphicSignature {
  using Type =
      utility::SignatureReplaceTypeWithT<SelfType, PolymorphicType, Signature>;
};
template <typename PolymorphicType, typename Signature>
using PolymorphicSignatureT =
    typename PolymorphicSignature<PolymorphicType, Signature>::Type;

} // namespace CxxPlugins
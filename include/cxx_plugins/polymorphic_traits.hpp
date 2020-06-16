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

namespace CxxPlugins {

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
struct Tag {};

template <typename TagType> static constexpr Tag<TagType> tag = {};

template <typename TagT>
/*!
 * \brief
 * PolymorphicFunction is a trait that simplifies creation of polymorphic
 * templates. It allows to bind certain tag to specific function type.
 * \details
 * For example you want your tag `add` be associated with signature
 * `void(obj, int)`.
 * \attention
 * First parameter to your function should always be `void*` or `void const*`
 * \details
 * ```cpp
 * struct add {};
 * }
 *
 * template<>
 * struct CxxPlugins::PolymorphicFunction<add> {
 *      using Type = void (*)(void*, int);
 * };
 *
 * // now we can declare our polymorphic like this:
 * using poly = polymorphic<add>;
 * ```
 * \note
 * PolymorphicFunction has partial specialization for
 * PolymorphicFunction<Tag<T>> that uses Type of PolymorphicFunction<T>
 *
 *
 * \attention READ THIS TIMUR!!!
 * Delete this section after reading.
 * Idk how you implemented polymorphic so far. But my assumption is that it
 * looks like this:
 * ```cpp
 * template<typename... Ts>
 * class polymorphic;
 *
 * template<typename... Tags, typename... FunctionTypes>
 * class polymorphic<TaggedValue<Tags,FunctionTypes>...> {
 *  //...
 * };
 * ```
 * In order to support this trait you don't need to change above code(almost)
 * you only need to add this:
 *
 * 1. tuple_map.hpp (You don't have this file on the branch yet, MERGE!!!)
 * ```cpp
 * // After definition of TaggedValue
 * template<typename T>
 * struct IsTaggedValue : public std::false_type {};
 * template<typename Tag, typename ValueT>
 * struct IsTaggedValue<TaggedValue<Tag,ValueT>> : public std::true_type {};
 *
 * template<typename T>
 * static constexpr bool is_tagged_value_v = IsTaggedValue<T>::value;
 * ```
 *
 * 2. polymorphic.hpp
 * ```cpp
 * // rename polymorphic to PolymorphicImpl
 * namespace impl {
 * template<typename...>
 * class PolymorphicImpl<...> {};
 * }
 *
 * // Creating conditional alias
 * template<typename... Ts>
 * // fold expression bellow checks if all Ts are TaggedValues
 * // with empty parameter pack will return true
 * //                                               ▼
 * using Polymorphic = std::conditional_t<(is_tagged_value_v<Ts>... &&),
 *  impl::PolymorphicImpl<Ts...>,
 *  impl::PolymorphicImpl<TaggedValue<Ts, PolymorphicFunctionT<Tags>>...>
 *  >;
 *
 * ```
 * You can instead add 2 partial template specializations where one inherits
 * from another, but I think this approach is better as it creates less types,
 * which leads to smaller executable size.
 *
 *
 *
 */
struct PolymorphicFunction {
  static_assert(sizeof(TagT) == 0,
                "PolymorphicFunction is not specialized for given Tag");
  using Type = void (*)(void *obj_p, int arg);
};
template <typename TagT> struct PolymorphicFunction<Tag<TagT>> {
  using Type = typename PolymorphicFunction<TagT>::Type;
};

template <typename TagT>
using PolymorphicFunctionT = typename PolymorphicFunction<TagT>::Type;

template <typename Tag, class T>
/*!
 * \brief
 * PolymorphicFunctionGenerator is your default implementation for
 * different function tags.
 * \details
 * For example you want to implement following interface for your polymorphic:
 * ```cpp
 * class Base {
 *  virtual add(int);
 * };
 * ```
 * Instead you will write these traits:
 * ```cpp
 * struct add{};
 *
 * template<>
 * struct CxxPlugins::PolymorphicFunction<add> {
 *      using Type = void (*)(void*, int);
 * };
 *
 * template<typename T>
 * struct CxxPlugins::PolymorphicFunctionGenerator<add,T> {
 *      static constexpr auto value =
 *          CxxPlugins:utility::generateTrampoline<&T::add>();
 * };
 * ```
 * In user code now you can do this:
 * ```cpp
 * struct my_impl {
 *   void add(int i);
 * }
 *
 * void do_something(Polymorphic<Tag<add>>& obj) {
 *  obj[tag<add>](4);
 * }
 * ```
 * \note
 * PolymorphicFunctionGenerator has partial specialization for
 * PolymorphicFunctionGenerator<Tag<TagT>, T> that uses value of
 * PolymorphicFunctionGenerator<TagT, T>
 *
 * \attention TIMUR READ THIS!!!
 * Delete this section after reading.
 * Use this trait to get your functions into map, like this:
 * ```cpp
 * template<typename... Ts>
 * struct polymorphic;
 *
 * template<typename T>
 * struct IsPolymorphic : public std::false_type{};
 *
 * template<typename... Ts>
 * struct IsPolymorphic<polymorphic<Ts...>> : public std::true_type{};
 *
 * template<typename T>
 * static constexpr auto is_polymorphic_v = IsPolymorphic<T>::value;
 *
 *
 * template<typename...Tags, typename... FunctionTypes>
 * class polymorphic<TaggedValue<Tags,FunctionTypes>...> {
 * private:
 *
 * public:
 *  //...
 *  template<typename U, typename =
 *  // without enable_if it will hide a bunch of constructors
 *  std::enable_if_t<!is_polymorphic_v<std::decay_t<U>> >
 *  polymorphic(U&& obj) :
 *      function_table(
 * polymorphic_function_generator_v<Tags,std::decay_t<U>>...)
 *  {
 *      //...
 *  }
 *  // You need to do same thing for the assignment operator
 *
 * };
 * ```
 *
 */
struct PolymorphicFunctionGenerator {
  static_assert(sizeof(T) == 0, "PolymorphicFunctionGenerator is not "
                                "specialized for neither Tag, neither type T");
  static constexpr void (*value)(void *obj_p, int) = nullptr;
};

template <typename TagT, class T>
struct PolymorphicFunctionGenerator<Tag<TagT>, T> {
  static constexpr auto value = PolymorphicFunctionGenerator<TagT, T>::value;
};

template <typename Tag, class T>
static constexpr auto polymorphic_function_generator_v =
    PolymorphicFunctionGenerator<Tag, T>::value;

} // namespace CxxPlugins
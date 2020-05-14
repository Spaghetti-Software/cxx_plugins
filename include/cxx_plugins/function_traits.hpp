/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of cxx_plugins project.
 * License is available at
 *https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    function_traits.hpp
 * \author  Andrey Ponomarev
 * \date    09 May 2020
 *
 * \brief Provides helper classes/functions to work with methods and functions.
 *
 * \todo Finish documentation in function_traits.hpp
 */
#pragma once

#include <utility>

namespace utility {

namespace impl {

template <typename Signature, typename Class = void>
struct SignatureToFunctionPointerImpl;

template <typename T, bool IsClass = std::is_class_v<std::decay_t<T>>>
struct IsCallableImpl;
} // namespace impl

template <typename T> struct FunctionTraits;

/*!
 *
 * \tparam Signature    Should be used in the way you will do it in
 *                      std::function
 * \tparam Class        Optional parameter if given function is method of
 *                      this class.
 * \brief Converts signature into pointer type.
 *        Simplifies function pointer declaration.
 * \subsubsection ex Example
 * \code
 * void foo();
 * void foo(int);
 *
 * struct bar {
 *  void call();
 *  void call() const;
 * };
 *
 * void baz() {
 *
 *  FunctionPointer<void()> foo_empty = foo;
 *  foo_empty();
 *
 *  // take const method
 *  FunctionPointer<void()const,bar> bar_call = &bar::call;
 *  bar b;
 *  (b->*bar_call)();
 * }
 * \endcode
 * \attention
 * Don't use FunctionPointer like this:
 * \code
 * auto ptr = FunctionPointer<Signature>(fn);
 * \endcode
 * As it is essentially a c cast. And compiler will not warn you about something like this:
 * \code
 * void foo();
 *
 * void bar() {
 *  auto ptr = FunctionPointer<void(int)>(foo);
 *  ptr(4); // undefined behavior sanitizer might help detect this
 * }
 * \endcode
 * Use this instead:
 * \code
 * FunctionPointer<Signature> ptr = fn;
 * \endcode
 * Or use functionPointerCast<Signature,Class>() for safe cast:
 * \code
 * auto ptr = functionPointerCast<Signature>(fn);
 * \endcode
 */
template <typename Signature, typename Class = void>
using FunctionPointer =
    typename impl::SignatureToFunctionPointerImpl<Signature, Class>::type;

/*!
 * \brief
 * Converts signature into pointer type.
 * Simplifies function pointer declaration.
 *
 * \tparam Signature    Should be used in the way you will do it in
 *                      std::function
 * \tparam Class        Optional parameter if given function is method of
 *                      this class.
 * \param fn_ptr        Pointer to function/method
 * \return Returns pointer to function/method that was passed inside
 * functionPointerCast
 * \subsubsection ex Example
 * \code
 * void foo();
 * void foo(int);
 *
 * struct bar {
 *  void call();
 *  void call() const;
 * };
 *
 * void baz() {
 *
 *  auto foo_empty = functionPointerCast<void()>(foo);
 *  foo_empty();
 *
 *  // take const method
 *  auto bar_call = functionPointerCast<void()const,bar>(&bar::call);
 *  bar b;
 *  (b->*bar_call)();
 * }
 * \endcode
 *
 */
template <typename Signature, typename Class = void>
constexpr auto functionPointerCast(FunctionPointer<Signature, Class> fn_ptr) {
  return fn_ptr;
}

/*!
 * \brief  Generates trampoline function that casts given InputT pointer to
 *         the underlying class.
 * \tparam method   Pointer to member function
 * \tparam InputT   Conversion type
 * \return pointer to the function of the form `Return(InputT*, TArgs...)`
 *
 *
 */
template <auto method, typename InputT = void>
constexpr auto generateTrampoline();

/*!
 *
 */
template <typename Signature, typename Class,
          FunctionPointer<Signature, Class> method, typename InputT = void>
constexpr auto generateTrampoline();

/*!
 * \brief  Allows "casting" methods to regular functions
 * \tparam method  Pointer to method
 * \return Function pointer that calls given method
 * \details
 * Essentially it generates function and returns pointer to it.
 * For example given method of this type: `void(foo::*)()const`
 * will give you function with signature `void(const foo*)`
 *
 * Uses generateTrampoline<Signature,ClassOfTheMethod>() internally.
 *
 * \subsubsection ex Example
 * \code
 * struct foo {
 *  void call();
 * };
 *
 * void bar() {
 *  auto fn = castMethodToFunction<&foo::call>();
 *  foo f;
 *  fn(&f);
 * }
 *
 * \endcode
 */
template <auto method> constexpr auto castMethodToFunction();
/*!
 * \brief  Simplifies casting overloaded methods
 * \tparam Signature Forced signature of the method
 * \tparam Class     Class of the method
 * \tparam method    Pointer to method
 * \subsubsection ex Example
 * \code
 * struct foo {
 *  void call();
 *  void call() const;
 * };
 *
 * void bar() {
 *  // get const overload
 *  auto fn = castMethodToFunction<void()const, foo, &foo::call>();
 *  const foo f;
 *  fn(&f);
 * }
 * \endcode
 */
template <typename Signature, typename Class,
          FunctionPointer<Signature, Class> method>
constexpr auto castMethodToFunction();

template <typename T>
static constexpr bool is_callable_v = impl::IsCallableImpl<T>::value;

} // namespace utility

#include <cxx_plugins/function_traits.ipp>
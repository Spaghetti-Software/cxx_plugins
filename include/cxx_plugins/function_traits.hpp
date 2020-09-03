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
 */
#pragma once

#include "cxx_plugins/type_traits.hpp"

#include <utility>

/*!
 * \brief Contains helper functions/classes
 * \details
 * Inside this namespace you will mainly find classes and functions used
 * in implementation but they are generic and can be used by external
 * applications.
 */
namespace plugins::utility {

// implementation details
namespace impl {

template <typename Signature, typename Class = void>
struct SignatureToFunctionPointerImpl;

template <typename T, bool IsClass = std::is_class_v<std::decay_t<T>>>
struct IsCallableImpl;
} // namespace impl

/*!
 * \brief Provides traits for given function
 * \tparam T [Signature|Function Pointer|Pointer to Member Function]
 */
#ifdef DOXYGEN
template <typename T> struct FunctionTraits {
  //! \brief Return type of the function/method
  using ReturnType = Return;
  //! \brief `std::tuple` of function/method arguments
  using ArgsTuple = std::tuple<Args...>;
  //! \brief `true` if pointer to member function, `false` otherwise
  static constexpr bool is_method;
  //! \brief
  //! `true` if method is const, `false` otherwise.
  //! Exists only if `is_method = true`.
  static constexpr bool is_const;
  //! \brief
  //! Class of the given method.
  //! Exists only if `is_method = true`.
  using ClassType = Class;
};
#else
template <typename T> struct FunctionTraits;
#endif

/*!
 * \brief
 * Converts signature into pointer type.
 * Simplifies function pointer declaration.
 * \tparam Signature    Should be used in the way you will do it in
 *                      std::function
 * \tparam Class        Optional parameter if given function is method of
 *                      this class.
 * \details
 * # Example:
 * ```cpp
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
 * ```
 * \attention
 * Don't use FunctionPointer like this:
 * ```cpp
 * auto ptr = FunctionPointer<Signature>(fn);
 * ```
 * \details
 * As it is essentially a c cast. And compiler will not warn you about something
 * like this:
 * ```cpp
 * void foo();
 *
 * void bar() {
 *  auto ptr = FunctionPointer<void(int)>(foo);
 *  ptr(4); // Undefined behavior. Sanitizer might help detect this.
 * }
 * ```
 *
 * Use this instead:
 *
 * ```cpp
 * FunctionPointer<Signature> ptr = fn;
 * ```
 *
 * Or use functionPointerCast<Signature,Class>() for safe cast:
 *
 * ```cpp
 * auto ptr = functionPointerCast<Signature>(fn);
 * ```
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
 *
 * \details
 * # Example:
 * ```cpp
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
 *  auto bar_call = functionPointerCast<void() const, bar>(&bar::call);
 *  bar b;
 *  (b->*bar_call)();
 * }
 * ```
 *
 */
template <typename Signature, typename Class = void>
constexpr auto functionPointerCast(FunctionPointer<Signature, Class> fn_ptr) {
  return fn_ptr;
}

/*!
 * \brief
 * Generates trampoline function that casts given InputT pointer to
 * the underlying class.
 * \tparam method   Pointer to method/function
 * \tparam InputT   Conversion type
 * \return pointer to the function of the form `Return(InputT*, TArgs...)`
 *
 * \details
 * Essentially it generates function and returns pointer to it.
 * For example given method of this type: `void(foo::*)()const`
 * will give you function with signature `void(const InputT*)`.
 * (by default InputT = void)
 *
 * # Example:
 * ```cpp
 * struct foo {
 *  void call();
 * };
 *
 * void bar() {
 *  foo f;
 *  auto fn_ptr = generateTrampoline<&foo::call>(); // void(*)(void*);
 *  fn_ptr(&f);
 * }
 * ```
 *
 */
template <auto function, typename InputT> constexpr auto generateTrampoline();

/*!
 * \brief
 * This is an overloaded function, provided for convenience.
 * It differs from the above function only in what argument(s) it accepts.
 * \tparam Signature Method signature
 * \tparam Class     Class of the provided method
 * \tparam method    Pointer to member function
 * \tparam InputT    Conversion type
 * \return pointer to the function of the form `Return(InputT*, TArgs...)`
 * \details
 * Simplifies generating overloaded methods.
 * # Example:
 * ```cpp
 * struct foo {
 *  void call();
 *  void call() const;
 * };
 *
 * void bar() {
 *  foo f;
 *  auto fn_ptr = generateTrampoline<void() const, foo, &foo::call>(); //
 * void(*)(const void*); fn_ptr(&f);
 * }
 * ```
 */
template <typename Signature, typename Class,
          FunctionPointer<Signature, Class> method, typename InputT>
constexpr auto generateTrampoline();

template <typename Signature, FunctionPointer<Signature> function,
          typename InputT>
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
 * # Example:
 * ```cpp
 * struct foo {
 *  void call();
 * };
 *
 * void bar() {
 *  auto fn = castMethodToFunction<&foo::call>();
 *  foo f;
 *  fn(&f);
 * }
 * ```
 *
 */
template <auto method> constexpr auto castMethodToFunction();
/*!
 * \brief
 * This is an overloaded function, provided for convenience.
 * It differs from the above function only in what argument(s) it accepts.
 * \tparam Signature Forced signature of the method
 * \tparam Class     Class of the method
 * \tparam method    Pointer to method
 * \details
 *
 * Simplifies generating overloaded methods.
 * # Example:
 *
 * ```cpp
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
 * ```
 */
template <typename Signature, typename Class,
          FunctionPointer<Signature, Class> method>
constexpr auto castMethodToFunction();

/*!
 * \brief       Checks if given type is callable
 * \attention   Returns `false` for pointers to member functions
 * As syntax of calling member functions differs from regular functions.
 */
template <typename T>
static constexpr bool is_callable_v = impl::IsCallableImpl<T>::value;

template <typename What, typename With, typename Signature>
struct SignatureReplaceTypeWith;

template <typename What, typename With, typename Return, typename... Args>
struct SignatureReplaceTypeWith<What, With, Return(Args...)> {
  using Type = ReplaceIfSameUnqualifiedT<Return, What, With>(
      ReplaceIfSameUnqualifiedT<Args, What, With>...);
};
template <typename What, typename With, typename Return, typename... Args>
struct SignatureReplaceTypeWith<What, With, Return(Args...) const> {
  using Type = ReplaceIfSameUnqualifiedT<Return, What, With>(
      ReplaceIfSameUnqualifiedT<Args, What, With>...) const;
};

template <typename What, typename With, typename Signature>
using SignatureReplaceTypeWithT =
    typename SignatureReplaceTypeWith<What, With, Signature>::Type;

template <typename T> struct IsFunctionPointer : public std::false_type {};
template <typename Return, typename... Args>
struct IsFunctionPointer<Return (*)(Args...)> : public std::true_type {};
template <typename T>
static constexpr bool is_function_pointer_v = IsFunctionPointer<T>::value;

template <typename T>
struct IsMemberFunctionPointer : public std::is_member_function_pointer<T> {};
template <typename T>
static constexpr bool is_member_function_pointer_v =
    IsMemberFunctionPointer<T>::value;

template <typename T>
struct IsAnyFunctionPointer
    : public std::conditional_t<is_function_pointer_v<T> ||
                                    is_member_function_pointer_v<T>,
                                std::true_type, std::false_type> {};
template <typename T>
static constexpr bool is_any_function_pointer_v =
    IsAnyFunctionPointer<T>::value;

template <typename T> struct SignatureOf;

template <typename T> using SignatureOfT = typename SignatureOf<T>::Type;

template <typename Return, typename... Args>
struct SignatureOf<Return(Args...)> {
  using Type = Return(Args...);
};

template <typename Return, typename... Args>
struct SignatureOf<Return(Args...) const> {
  using Type = Return(Args...) const;
};

template <typename Return, typename... Args>
struct SignatureOf<Return (*)(Args...)> {
  using Type = Return(Args...);
};

template <typename Class, typename Return, typename... Args>
struct SignatureOf<Return (Class::*)(Args...)> {
  using Type = Return(Args...);
};

template <typename Class, typename Return, typename... Args>
struct SignatureOf<Return (Class::*)(Args...) const> {
  using Type = Return(Args...) const;
};

template <typename Class> struct SignatureOf {
  static_assert(std::is_class_v<Class>,
                "Template parameter should be either function pointer type, "
                "pointer to member function or class.");
  using Type = SignatureOfT<decltype(&Class::operator())>;
};

template <typename T> struct SignatureRemoveConst;

template <typename T>
using SignatureRemoveConstT = typename SignatureRemoveConst<T>::Type;

template <typename Return, typename... Args>
struct SignatureRemoveConst<Return(Args...)> {
  using Type = Return(Args...);
};

template <typename Return, typename... Args>
struct SignatureRemoveConst<Return(Args...) const> {
  using Type = Return(Args...);
};

} // namespace CxxPlugins::utility

#include <cxx_plugins/function_traits.ipp>
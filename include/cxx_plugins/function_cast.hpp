/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev
 * This file is part of ECSFramework project.
 * License is available at
 * https://gitlab.com/andrey.ponomarev.1408/ecsframework
 *************************************************************************************************/
/*!
 * \file    function_cast.hpp
 * \author  Andrey Ponomarev
 * \date    03 Sep 2020
 * \brief
 *  This file contains helper functions and types to work with function pointers.
 */
#pragma once

#include "cxx_plugins/function_traits.hpp"
#include "cxx_plugins/type_traits.hpp"
#include <utility>

namespace plugins {
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
 *  FnPtr<void()> foo_empty = foo;
 *  foo_empty();
 *
 *  // take const method
 *  FnPtr<void()const,bar> bar_call = &bar::call;
 *  bar b;
 *  (b->*bar_call)();
 * }
 * ```
 * \attention
 * Don't use FnPtr like this:
 * ```cpp
 * auto ptr = FnPtr<Signature>(fn);
 * ```
 * \details
 * As it is essentially a *C* cast. And compiler will not warn you about something
 * like this:
 * ```cpp
 * void foo();
 *
 * void bar() {
 *  auto ptr = FnPtr<void(int)>(foo);
 *  ptr(4); // Undefined behavior. Sanitizer might help detect this.
 * }
 * ```
 *
 * Use this instead:
 *
 * ```cpp
 * FnPtr<Signature> ptr = fn;
 * ```
 *
 * Or use functionPointerCast<Signature,Class>() for safe cast:
 *
 * ```cpp
 * auto ptr = functionPointerCast<Signature>(fn);
 * ```
 */
#ifdef DOXYGEN
template<typename Signature, typename Class = void>
class FnPtr;
#else
template <typename Signature, typename Class = void>
using FnPtr =
    typename traits::impl::SignatureToFunctionPointerImpl<Signature,
                                                          Class>::type;
#endif

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
constexpr auto functionPointerCast(FnPtr<Signature, Class> fn_ptr) {
  return fn_ptr;
}

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
 * This is an overloaded function, provided for
 * convenience. It differs from the above function only
 * in what argument(s) it accepts.
 * \tparam Signature Forced signature of the method
 * \tparam Class Class of the method
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
 *  auto fn = castMethodToFunction<void()const, foo,
 * &foo::call>(); const foo f; fn(&f);
 * }
 * ```
 */
template <typename Signature, typename Class, FnPtr<Signature, Class> method>
constexpr auto castMethodToFunction();

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
template <typename Signature, typename Class, FnPtr<Signature, Class> method,
          typename InputT>
constexpr auto generateTrampoline();

template <typename Signature, FnPtr<Signature> function, typename InputT>
constexpr auto generateTrampoline();

// IMPLEMENTATION DETAILS

template <auto method> constexpr auto castMethodToFunction() {
  using Traits = plugins::traits::FunctionTraits<decltype(method)>;
  static_assert(Traits::is_method, "Variable passed in should be a method");

  return generateTrampoline<method, typename Traits::ClassType>();
}
template <typename Signature, typename Class, FnPtr<Signature, Class> method>
constexpr auto castMethodToFunction() {
  return generateTrampoline<method, Class>();
}

namespace impl {
template <auto function, typename InputT> struct TrampolineGeneratorImpl;

template <typename Class, typename Return, typename... Args,
          Return (Class::*method)(Args...), typename InputT>
struct TrampolineGeneratorImpl<method, InputT> {

  static constexpr auto fnImpl(InputT *input_p, Args... args) -> Return {
    auto obj_p = static_cast<Class *>(input_p);
    return (obj_p->*method)(std::forward<Args>(args)...);
  }

  static constexpr auto value = &fnImpl;
};

template <typename Class, typename Return, typename... Args,
          Return (Class::*method)(Args...) const, typename InputT>
struct TrampolineGeneratorImpl<method, InputT> {

  static constexpr auto fnImpl(const InputT *input_p, Args... args) -> Return {
    const auto *obj_p = static_cast<const Class *>(input_p);
    return (obj_p->*method)(std::forward<Args>(args)...);
  }

  static constexpr auto value = &fnImpl;
};

template <typename Class, typename Return, typename... Args,
          Return (*fn)(Class *, Args...), typename InputT>
struct TrampolineGeneratorImpl<fn, InputT> {

  static constexpr auto fnImpl(InputT *input_p, Args... args) -> Return {
    auto obj_p = static_cast<Class *>(input_p);
    return fn(obj_p, std::forward<Args>(args)...);
  }

  static constexpr auto value = &fnImpl;
};

template <typename Class, typename Return, typename... Args,
          Return (*fn)(Class const *, Args...), typename InputT>
struct TrampolineGeneratorImpl<fn, InputT> {

  static constexpr auto fnImpl(const InputT *input_p, Args... args) -> Return {
    auto obj_p = static_cast<const Class *>(input_p);
    return fn(obj_p, std::forward<Args>(args)...);
  }

  static constexpr auto value = &fnImpl;
};

template <typename Class, typename Return, typename... Args,
          Return (*fn)(Class *, Args...)>
struct TrampolineGeneratorImpl<fn, Class> {

  static constexpr auto value = fn;
};

template <typename Class, typename Return, typename... Args,
          Return (*fn)(Class const *, Args...)>
struct TrampolineGeneratorImpl<fn, Class> {
  static constexpr auto value = fn;
};
} // namespace impl

template <auto function, typename InputT> constexpr auto generateTrampoline() {
  return impl::TrampolineGeneratorImpl<function, InputT>::value;
}

template <typename Signature, FnPtr<Signature, void> function, typename InputT>
constexpr auto generateTrampoline() {
  return generateTrampoline<function, InputT>();
}

} // namespace plugins
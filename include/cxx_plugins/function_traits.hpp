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
namespace plugins::traits {

// implementation details
namespace impl {

template <typename Signature, typename Class = void>
struct SignatureToFunctionPointerImpl;

template <typename T, bool IsClass = std::is_class_v<std::decay_t<T>>>
struct IsCallableImpl;
} // namespace impl

/*!
 * \brief Provides traits for given function type
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
 * \brief       Checks if given type is callable
 * \attention   Returns `false` for pointers to member functions
 * As syntax of calling member functions differs from regular functions.
 */
template <typename T>
static constexpr bool is_callable_v = impl::IsCallableImpl<T>::value;

/*!
 * \brief Replaces type in the signature with a given type.
 * Use SignatureReplaceTypeWithT to extract actual signature type
 */
template <typename What, typename With, typename Signature>
struct SignatureReplaceTypeWith;

template <typename What, typename With, typename Signature>
using SignatureReplaceTypeWithT =
typename SignatureReplaceTypeWith<What, With, Signature>::Type;

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



/*!
 * \brief Checks if type is a function pointer.
 * Returns false for member function pointers and signatures
 */
template <typename T> struct IsFunctionPointer : public std::false_type {};
template <typename Return, typename... Args>
struct IsFunctionPointer<Return (*)(Args...)> : public std::true_type {};
template <typename T>
static constexpr bool is_function_pointer_v = IsFunctionPointer<T>::value;

/*!
 * \brief Checks if type is a member function pointer
 */
template <typename T>
struct IsMemberFunctionPointer : public std::is_member_function_pointer<T> {};
template <typename T>
static constexpr bool is_member_function_pointer_v =
    IsMemberFunctionPointer<T>::value;

/*!
 * \brief Checks if type is a function pointer or member function pointer
 */
template <typename T>
struct IsAnyFunctionPointer
    : public std::conditional_t<is_function_pointer_v<T> ||
                                    is_member_function_pointer_v<T>,
                                std::true_type, std::false_type> {};
template <typename T>
static constexpr bool is_any_function_pointer_v =
    IsAnyFunctionPointer<T>::value;

/*!
 * \brief Gets the signature of a function pointer, member function pointer
 * or a class(if it has only one overload of call operator).
 */
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

/*!
 * \brief Removes const qualifier from signature.
 * For example `void(int)const` will become `void(int)`
 */
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

} // namespace plugins::traits

#include <cxx_plugins/function_traits.ipp>
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
 * \brief
 * This file contains traits for functions/function pointers
 *
 * FunctionPointer - simplifies declaration of function pointers
 *                   Usage:
 *                   FunctionPointer<Signature> - for regular function pointers
 *                   FunctionPointer<Signature,Class> - for member function
 *                                                      pointers
 * \attention   Don't use FunctionPointer like this:
 *              auto ptr = FunctionPointer<Signature>(fn);
 *              As it casts any function to the given Signature. Use this instead:
 *              FunctionPointer<Signature> ptr = fn;
 *              Or this(for safe cast):
 *              auto ptr = functionPointerCast<Signature>(fn);
 */
#pragma once

namespace utility {

namespace impl {

template <typename Signature, typename Class = void>
struct SignatureToFunctionPointerImpl;

} // namespace impl

/*!
 * \brief Converts signature into pointer type
 *        Simplifies function pointer declaration
 *
 * \tparam Signature  - should be used in the way you will do it in
 *                      std::function
 * \tparam Class      - optional parameter if given function is method of
 *                      this class.
 */
template <typename Signature, typename Class = void>
using FunctionPointer =
    typename impl::SignatureToFunctionPointerImpl<Signature, Class>::type;

template<typename Signature, typename Class = void>
constexpr auto functionPointerCast(FunctionPointer<Signature,Class> fn_ptr) {
  return fn_ptr;
}

template <auto method> constexpr auto castMethodToFunction();

} // namespace utility

#include "function_traits.ipp"
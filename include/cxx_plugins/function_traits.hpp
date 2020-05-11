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
 *
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

template <auto method> constexpr auto castMethodToFunction();

} // namespace utility

#include "function_traits.ipp"
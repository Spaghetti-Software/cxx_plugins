/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    inclusive_scan.hpp
 * \author  Andrey Ponomarev
 * \date    08 Jun 2020
 * \brief
 * $BRIEF$
 */
#pragma once

#include "sequence/exclusive_scan.hpp"
#include "sequence/operators.hpp"

namespace Sequence {

template <typename T, T... vals>
struct InclusiveScan : public PlusT<ExclusiveScanT<T, vals...>,
                                    std::integer_sequence<T, vals...>> {};

template <typename T, T... vals>
struct InclusiveScan<std::integer_sequence<T, vals...>>
    : public PlusT<ExclusiveScanT<std::integer_sequence<T, vals...>>,
                   std::integer_sequence<T, vals...>> {};

template<typename T, T... vals>
using InclusiveScanT = typename InclusiveScan<T,vals...>::type;

} // namespace Sequence
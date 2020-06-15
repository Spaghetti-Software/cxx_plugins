/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    tuple_tests2.cpp
 * \author  Andrey Ponomarev
 * \date    04 Jun 2020
 * \brief
 * $BRIEF$
 */
#include "tuple_tests_decl.hpp"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#endif

INSTANTIATE_TYPED_TEST_SUITE_P(SimpleMultiTypes, PackedTupleTests,
                               SimpleMultiTypes);


#ifdef __clang__
#pragma clang diagnostic pop
#endif
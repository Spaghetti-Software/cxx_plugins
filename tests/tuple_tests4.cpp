/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    tuple_tests4.cpp
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

using Type = CxxPlugins::Tuple<ExplicitData<double>, ExplicitData<double>, ExplicitData<char>, ExplicitData<char>>;

//INSTANTIATE_TYPED_TEST_SUITE_P(PaddedTypes, PackedTupleTests, PaddedTypes);
INSTANTIATE_TYPED_TEST_SUITE_P(PaddedTypes, PackedTupleTests, Type);

// Tests for references should be created separately
// INSTANTIATE_TYPED_TEST_SUITE_P(ReferenceTypes, PackedTupleTests,
//                               ReferenceTypes);

#ifdef __clang__
#pragma clang diagnostic pop
#endif
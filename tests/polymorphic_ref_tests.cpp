/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    polymorphic_ref_tests.cpp
 * \author  Andrey Ponomarev
 * \date    18 Jun 2020
 * \brief
 * $BRIEF$
 */

#include <cxx_plugins/polymorphic_ref.hpp>

#include <gtest/gtest.h>

struct add{};

template<typename T>
constexpr void polymorphicExtend(CxxPlugins::Tag<add> /*unused*/, T& obj, int val) {
  obj.add(val);
}

struct foo {
  void add(int i) {
    i_m += i;
  }

  int i_m = 0;
};




TEST(PolymorphicRef, SimpleTests) {

  using namespace CxxPlugins;
  foo obj0;

  PolymorphicRef<TaggedValue<Tag<add>,void(int)>> simple_poly(obj0);
  simple_poly[tag<add>](4);
  EXPECT_EQ(obj0.i_m, 4);

}

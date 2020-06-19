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

struct add {};
struct multiply {};

template <> struct CxxPlugins::PolymorphicTagSignature<add> {
  using Type = void(int);
};
template <> struct CxxPlugins::PolymorphicTagSignature<multiply> {
  using Type = void(int);
};

template <typename T>
constexpr void polymorphicExtend(add /*unused*/, T &obj, int val) {
  obj.add(val);
}

template <typename T>
constexpr void polymorphicExtend(multiply /*unused*/, T &obj, int val) {
  obj.multiply(val);
}

struct foo {
  void add(int i) { i_m += i; }
  void multiply(int i) { i_m *= i; }

  int i_m = 0;
};

TEST(PolymorphicRef, SimpleTests) {

  using namespace CxxPlugins;
  foo obj0;

  PolymorphicRef<Tag<add>, Tag<multiply>> simple_poly(obj0);
  simple_poly[tag<add>](4);
  EXPECT_EQ(obj0.i_m, 4);
  simple_poly[tag<multiply>](4);
  EXPECT_EQ(obj0.i_m, 16);

}

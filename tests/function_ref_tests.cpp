/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev
 * This file is part of ECSFramework project.
 * License is available at
 * https://gitlab.com/andrey.ponomarev.1408/ecsframework
 *************************************************************************************************/
/*!
 * \file    function_ref_tests.cpp
 * \author  Andrey Ponomarev
 * \date    05 Sep 2020
 * \brief
 *
 */

#include <gtest/gtest.h>
#include <cxx_plugins/functionref.hpp>


TEST(FunctionRefTests, Empty) {
  using namespace plugins;
  FunctionRef<void()> fn;
  FunctionRef<void(int)> fn2;
  FunctionRef<void(int)> fn3;
  FunctionRef<void()> fn4 = nullptr;
  fn2 = fn3;
  fn2 = std::move(fn3);
  EXPECT_EQ(fn2, fn3);
  fn4 = nullptr;
  EXPECT_EQ(fn, fn4);
  fn4 = static_cast<FnPtr<void()>>(nullptr);
  EXPECT_EQ(fn, fn4);
  EXPECT_THROW(fn(), std::logic_error);
}

void foo() {

}

TEST(FunctionRefTests, FnPtrAssignment) {
  using namespace plugins;
  FunctionRef<void()> fn = foo;
  FunctionRef<void()> fn2 = &foo;
  EXPECT_NE(fn, nullptr);
  EXPECT_EQ(fn, fn2);
  EXPECT_EQ(fn, &foo);
  fn();

  fn = foo;
  fn2 = &foo;
  EXPECT_NE(fn, nullptr);
  EXPECT_EQ(fn, fn2);
  EXPECT_EQ(fn, &foo);

}

TEST(FunctionRefTests, MemberPtrAssignment) {
  using namespace plugins;

  struct foo {
    void bar() {

    }
    void baz() const {

    }
  };

  FunctionRef<void(foo &)> fn = &foo::bar;
  FunctionRef<void(foo const&)> fn2 = &foo::baz;

  foo obj;
  fn(obj);
  fn2(obj);

  EXPECT_EQ(fn, &foo::bar);
  EXPECT_EQ(fn2, &foo::baz);
  EXPECT_NE(fn, nullptr);
  EXPECT_NE(fn2, nullptr);
}

TEST(FunctionRefTests, ObjectAssignment) {
  using namespace plugins;

  auto l = [](){};
  FunctionRef<void()> fn = &l;
  fn();
  EXPECT_EQ(fn, &l);
  EXPECT_NE(fn, nullptr);
}
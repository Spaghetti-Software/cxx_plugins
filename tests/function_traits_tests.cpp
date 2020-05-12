//
// Created by andrey on 5/12/20.
//
#include <gtest/gtest.h>
#include <cxx_plugins/function_traits.hpp>

#include <string>

using std::string;

namespace {

  void simpleFunction() {
  }

  void overloadedFunction() {}
  void overloadedFunction([[maybe_unused]] int a) {}

  struct Foo {
    void simpleMethod() {}
    void overloadedMethod() {}
    void overloadedMethod([[maybe_unused]] int a) {}
  };

} // namespace


TEST(FunctionPointerTests, SimpleFn) {

  auto expected_fn = &simpleFunction;
  auto result_fn = utility::functionPointerCast<void()>(simpleFunction);
  EXPECT_EQ(expected_fn, result_fn);

  utility::FunctionPointer<void()> result_fn2 = simpleFunction;
  EXPECT_EQ(expected_fn, result_fn2);
}

TEST(FunctionPointerTests, OverloadedFn) {
    using type0 = void (*)();
    using type1 = void (*)(int);
    auto expected_overloaded0 = type0(overloadedFunction);
    auto expected_overloaded1 = type1(overloadedFunction);

    auto result0_0 = utility::functionPointerCast<void()>(overloadedFunction);
    auto result0_1 = utility::functionPointerCast<void(int)>(overloadedFunction);

    EXPECT_EQ(expected_overloaded0, result0_0);
    EXPECT_EQ(expected_overloaded1, result0_1);

    utility::FunctionPointer<void()> result1_0 = overloadedFunction;
    utility::FunctionPointer<void(int)> result1_1 = overloadedFunction;

    EXPECT_EQ(expected_overloaded0, result1_0);
    EXPECT_EQ(expected_overloaded1, result1_1);
}

TEST(FunctionPointerTests, SimpleMethod) {
  auto expected_fn = &Foo::simpleMethod;
  auto result_fn = utility::functionPointerCast<void(),Foo>(&Foo::simpleMethod);

  EXPECT_EQ(expected_fn, result_fn);

  utility::FunctionPointer<void(),Foo> result_fn2 = &Foo::simpleMethod;
  EXPECT_EQ(expected_fn, result_fn2);
}

TEST(FunctionPointerTests, OverloadedMethod) {
  using type0 = void (Foo::*)();
  using type1 = void (Foo::*)(int);
  auto expected_overloaded0 = type0(&Foo::overloadedMethod);
  auto expected_overloaded1 = type1(&Foo::overloadedMethod);

  auto result0_0 = utility::functionPointerCast<void(),Foo>(&Foo::overloadedMethod);
  auto result0_1 = utility::functionPointerCast<void(int),Foo>(&Foo::overloadedMethod);

  EXPECT_EQ(expected_overloaded0, result0_0);
  EXPECT_EQ(expected_overloaded1, result0_1);

  utility::FunctionPointer<void(),Foo> result1_0 = &Foo::overloadedMethod;
  utility::FunctionPointer<void(int),Foo> result1_1 = &Foo::overloadedMethod;

  EXPECT_EQ(expected_overloaded0, result1_0);
  EXPECT_EQ(expected_overloaded1, result1_1);
}



//
// Created by andrey on 5/12/20.
//
#include <cxx_plugins/function_traits.hpp>
#include <gtest/gtest.h>

#include <cstdio>
#include <string>

using std::string;

namespace {

void simpleFunction() {}

void overloadedFunction() {}
void overloadedFunction([[maybe_unused]] int a) {}

struct Foo {
  int simpleMethod() { return 1; }
  int overloadedMethod() { return 2; }
  int overloadedMethod([[maybe_unused]] int a) { return 3; }
  int overloadedMethod() const { return 4; }
};

} // namespace

TEST(FunctionPointerTests, SimpleFn) {
  using namespace plugins;
  auto expected_fn = &simpleFunction;
  auto result_fn = utility::functionPointerCast<void()>(simpleFunction);
  EXPECT_EQ(expected_fn, result_fn);

  utility::FunctionPointer<void()> result_fn2 = simpleFunction;
  EXPECT_EQ(expected_fn, result_fn2);
}

TEST(FunctionPointerTests, OverloadedFn) {
  using namespace plugins;
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
  using namespace plugins;

  auto expected_fn = &Foo::simpleMethod;
  auto result_fn = utility::functionPointerCast<int(), Foo>(&Foo::simpleMethod);

  EXPECT_EQ(expected_fn, result_fn);

  utility::FunctionPointer<int(), Foo> result_fn2 = &Foo::simpleMethod;
  EXPECT_EQ(expected_fn, result_fn2);
}

TEST(FunctionPointerTests, OverloadedMethod) {
  using namespace plugins;
  using type0 = int (Foo::*)();
  using type1 = int (Foo::*)(int);
  using type2 = int (Foo::*)() const;
  auto expected_overloaded0 = type0(&Foo::overloadedMethod);
  auto expected_overloaded1 = type1(&Foo::overloadedMethod);
  auto expected_overloaded2 = type2(&Foo::overloadedMethod);

  auto result0_0 =
      utility::functionPointerCast<int(), Foo>(&Foo::overloadedMethod);
  auto result0_1 =
      utility::functionPointerCast<int(int), Foo>(&Foo::overloadedMethod);
  auto result0_2 =
      utility::functionPointerCast<int() const, Foo>(&Foo::overloadedMethod);

  EXPECT_EQ(expected_overloaded0, result0_0);
  EXPECT_EQ(expected_overloaded1, result0_1);
  EXPECT_EQ(expected_overloaded2, result0_2);

  utility::FunctionPointer<int(), Foo> result1_0 = &Foo::overloadedMethod;
  utility::FunctionPointer<int(int), Foo> result1_1 = &Foo::overloadedMethod;
  utility::FunctionPointer<int() const, Foo> result1_2 = &Foo::overloadedMethod;

  EXPECT_EQ(expected_overloaded0, result1_0);
  EXPECT_EQ(expected_overloaded1, result1_1);
  EXPECT_EQ(expected_overloaded2, result1_2);
}

TEST(castMethodToFunctionTests, SimpleFunction) {
  using namespace plugins;

  Foo f;
  auto expected = f.simpleMethod();

  auto fn = utility::castMethodToFunction<&Foo::simpleMethod>();
  auto result = fn(&f);

  EXPECT_EQ(expected, result)
      << "Results from method and casted method are different";
}

TEST(castMethodToFunctionTests, OverloadedFunction) {
  using namespace plugins;
  Foo f;

  auto expected0 = f.overloadedMethod();
  auto expected1 = f.overloadedMethod(1);
  auto expected2 = std::as_const(f).overloadedMethod();

  auto fn0 =
      utility::castMethodToFunction<int(), Foo, &Foo::overloadedMethod>();
  auto fn1 =
      utility::castMethodToFunction<int(int), Foo, &Foo::overloadedMethod>();
  auto fn2 =
      utility::castMethodToFunction<int() const, Foo, &Foo::overloadedMethod>();

  auto result0 = fn0(&f);
  auto result1 = fn1(&f, 1);
  auto result2 = fn2(&f);

  EXPECT_EQ(expected0, result0)
      << "Results from method and casted method are different";
  EXPECT_EQ(expected1, result1)
      << "Results from method and casted method are different";
  EXPECT_EQ(expected2, result2)
      << "Results from method and casted method are different";
}

TEST(voidTrampolineTest, SimpleMethod) {
  using namespace plugins;

  Foo f;
  auto expected = f.simpleMethod();

  auto fn = utility::generateTrampoline<&Foo::simpleMethod,void>();
  auto result = fn(&f);

  EXPECT_EQ(expected, result)
      << "Results from method and casted method are different";
}

TEST(voidTrampolineTest, OverloadedMethod) {
//  using namespace CxxPlugins;
//
//  Foo f;
//
//  auto expected0 = f.overloadedMethod();
//  auto expected1 = f.overloadedMethod(1);
//  auto expected2 = std::as_const(f).overloadedMethod();
//
//  auto fn0 =
//      utility::generateTrampoline<int(), Foo, &Foo::overloadedMethod,void>();
//  auto fn1 =
//      utility::generateTrampoline<int(int), Foo, &Foo::overloadedMethod,void>();
//  auto fn2 =
//      utility::generateTrampoline<int() const, Foo, &Foo::overloadedMethod,void>();
//
//  auto result0 = fn0(&f);
//  auto result1 = fn1(&f, 1);
//  auto result2 = fn2(&f);
//
//  EXPECT_EQ(expected0, result0)
//          << "Results from method and casted method are different";
//  EXPECT_EQ(expected1, result1)
//          << "Results from method and casted method are different";
//  EXPECT_EQ(expected2, result2)
//          << "Results from method and casted method are different";
}

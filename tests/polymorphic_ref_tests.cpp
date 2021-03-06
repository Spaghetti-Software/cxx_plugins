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

#include <cxx_plugins/polymorphic_allocator.hpp>
#include <cxx_plugins/polymorphic_ptr.hpp>
#include <gtest/gtest.h>

struct add {};
struct multiply {};
struct stringify {};

template <> struct plugins::PolymorphicTagSignature<add> {
  using Type = void(int);
};
template <> struct plugins::PolymorphicTagSignature<multiply> {
  using Type = void(int);
};
template <> struct plugins::PolymorphicTagSignature<stringify> {
  using Type = std::string() const;
};

template <typename T>
constexpr void polymorphicExtend(add /*unused*/, T &obj, int val) {
  obj.add(val);
}

template <typename T>
constexpr void polymorphicExtend(multiply /*unused*/, T &obj, int val) {
  obj.multiply(val);
}

template <typename T>
inline auto polymorphicExtend(stringify /*unused*/, T const &obj)
    -> std::string {
  return obj.stringify();
}

struct foo {
  void add(int i) { i_m += i; }
  void multiply(int i) { i_m *= i; }
  [[nodiscard]] auto stringify() const -> std::string {
    return std::to_string(i_m);
  }

  int i_m = 0;
};

TEST(PolymorphicRef, SimpleTests) {

  using namespace plugins;
  foo obj0;

  PolymorphicPtr<Tag<add>, Tag<multiply>> simple_poly(&obj0);
  simple_poly[tag<add>](4);
  EXPECT_EQ(obj0.i_m, 4);
  simple_poly[tag<multiply>](4);
  EXPECT_EQ(obj0.i_m, 16);

  PolymorphicPtr<Tag<add>> simpl_poly2(simple_poly);
  simpl_poly2 = simple_poly;

  PolymorphicPtr<Tag<stringify>, Tag<add>, Tag<multiply>> complex_poly0(&obj0);
  EXPECT_EQ(std::to_string(obj0.i_m), complex_poly0[tag<stringify>]());
  PolymorphicPtr<Tag<stringify>> constant_poly(&obj0);
  static_assert(std::is_same_v<void const *, decltype(constant_poly.data())>,
                "Constant objects should give void const*");
  complex_poly0.call<Tag<add>>(1);
  EXPECT_EQ(std::to_string(obj0.i_m), constant_poly.call<Tag<stringify>>());
  constant_poly = complex_poly0;
}

enum class ReferenceType { LValue, ConstLValue, RValue, ConstRValue };
static std::ostream &operator<<(std::ostream &os, ReferenceType ref_type) {
  const char *str = nullptr;
  switch (ref_type) {
  case ReferenceType::LValue:
    str = "LValue";
    break;
  case ReferenceType::ConstLValue:
    str = "ConstLValue";
    break;
  case ReferenceType::RValue:
    str = "RValue";
    break;
  case ReferenceType::ConstRValue:
    str = "ConstRValue";
    break;
  }
  os << str;
  return os;
}
struct Correctness {};
struct CorrectnessConst {
  CorrectnessConst(Correctness) {}
};

struct Dummy {};

constexpr auto polymorphicExtend(Correctness /*unused*/, Dummy & /*unused*/)
    -> ReferenceType {
  return ReferenceType::LValue;
}
constexpr auto polymorphicExtend(Correctness /*unused*/, Dummy const &
                                 /*unused*/) -> ReferenceType {
  return ReferenceType::ConstLValue;
}
//constexpr auto polymorphicExtend(Correctness /*unused*/, Dummy && /*unused*/)
//    -> ReferenceType {
//  return ReferenceType::RValue;
//}
//constexpr auto polymorphicExtend(Correctness /*unused*/, Dummy const &&
//                                 /*unused*/) -> ReferenceType {
//  return ReferenceType::ConstRValue;
//}

TEST(PolymorphicRef, checkReferenceCorrectness) {
  using namespace plugins;
  Dummy obj;

  PolymorphicPtr<TaggedSignature<Correctness, ReferenceType()>> lvalue_ref(&obj);
  auto expected = ReferenceType::LValue;
  auto result = lvalue_ref.call<Correctness>();
  EXPECT_EQ(expected, result)
      << "Expected: " << expected << ", Result: " << result;

  PolymorphicPtr<TaggedSignature<Correctness, ReferenceType() const>>
      lvalue_const_ref(&obj);
  expected = ReferenceType::ConstLValue;
  result = lvalue_const_ref.call<Correctness>();
  EXPECT_EQ(expected, result)
      << "Expected: " << expected << ", Result: " << result;
}


TEST(PolymorphicRef, DefaultConstructor) {
  using namespace plugins;
  [[maybe_unused]] PolymorphicPtr<> default_empty;
  [[maybe_unused]] PolymorphicPtr<add> default_single_arg;
  [[maybe_unused]] PolymorphicPtr<add,multiply,stringify> default_multi_arg;

  EXPECT_TRUE(default_empty.isEmpty());
  EXPECT_TRUE(default_single_arg.isEmpty());
  EXPECT_TRUE(default_multi_arg.isEmpty());
}


TEST(PolymorphicRef, Reorder) {
  using namespace plugins;
  foo obj0;
  PolymorphicPtr<add,multiply> ref0(&obj0);
  PolymorphicPtr<multiply,add> ref1(ref0);
  ref0.call<add>(20);
  EXPECT_EQ(obj0.i_m, 20);
  ref1.call<add>(20);
  EXPECT_EQ(obj0.i_m, 40);
}
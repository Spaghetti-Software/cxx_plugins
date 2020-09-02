/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    polymorphic_tests.cpp
 * \author  Timur Kazhimuratov
 * \author  Andrey Ponomarev
 * \date    18 Jun 2020
 * \brief
 * $BRIEF$
 */

#include <cxx_plugins/polymorphic.hpp>
#include <cxx_plugins/polymorphic_allocator.hpp>
#include <gtest/gtest.h>
#include "cxx_plugins/memory/stack_allocator.hpp"

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

TEST(Polymorphic, SimpleTests) {
  using namespace CxxPlugins;
  foo obj;

  Polymorphic<Tag<add>, Tag<multiply>> simple_poly(obj);

  auto pval = static_cast<int *>(simple_poly.data());

  simple_poly[tag<add>](4);
  EXPECT_EQ(*pval, 4);
  simple_poly[tag<multiply>](4);
  EXPECT_EQ(*pval, 16);
}

struct printSize {};

template <> struct CxxPlugins::PolymorphicTagSignature<printSize> {
  using Type = void();
};

template <typename T>
constexpr void polymorphicExtend(printSize /*unused*/, T const &obj) {
  obj.printSize();
}

struct smallbar {
  smallbar() { counter += i; }
  smallbar(const smallbar &other) { counter += i; }
  ~smallbar() { counter -= i; }

  void printSize() const {
    std::cout << "Size of smallbar: " << sizeof(smallbar) << '\n';
  }

  int i = 1;
  static int counter;
};

int smallbar::counter = 0;

template <std::size_t size> struct bigbar {
  void printSize() const {
    std::cout << "Size of bigbar with size " << size << ": " << sizeof(bigbar) << '\n';
  }

  char arr[size];
};

TEST(Polymorphic, AllocationAndConstructorTests) {
  using namespace CxxPlugins;
  EXPECT_EQ(smallbar::counter, 0);

  smallbar obj0;
  EXPECT_EQ(smallbar::counter, 1);

  Polymorphic<Tag<printSize>> poly(obj0);
  poly[tag<printSize>]();
  EXPECT_EQ(smallbar::counter, 2); // copy constructor of smallbar called
  EXPECT_EQ(poly.data(), &poly); // stack allocated

  bigbar<64> big_obj;
  poly = big_obj;
  poly[tag<printSize>]();
  EXPECT_EQ(smallbar::counter, 1); // destructor of smallbar called
  EXPECT_NE(poly.data(), &poly); // heap allocated

  Polymorphic<Tag<printSize>> copy_poly = poly;
  copy_poly[tag<printSize>]();

  Polymorphic<Tag<printSize>> move_poly = std::move(poly);
  move_poly[tag<printSize>]();
}

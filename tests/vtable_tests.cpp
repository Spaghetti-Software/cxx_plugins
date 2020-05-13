/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    vtable_tests.cpp
 * \author  Andrey Ponomarev
 * \date    13 May 2020
 * \brief
 * Contains tests for vtable class
 *
 */
#include <cxx_plugins/vtable.hpp>

#include <gtest/gtest.h>

struct Foo {};
struct Bar {};

static constexpr Foo foo_tag;
static constexpr Bar bar_tag;

int fooFn(void *) {
  static const int magic = 127;
  return magic;
}

int barFn(int) {
  static const int magic = 52;
  return magic;
}

// showing that both methods of declaring type should work
// (function pointer and function)
using foo_type = decltype(&fooFn);
using bar_type = utility::FunctionPointer<int(int)>;

TEST(VTableTests, CompileSimpleCtors) {
  using namespace CxxPlugins;

  [[maybe_unused]] VTable<Entry<Foo, foo_type>, Entry<Bar, bar_type>>
      table0; // default-ctor
  [[maybe_unused]] VTable<Entry<Foo, foo_type>, Entry<Bar, bar_type>> table1(
      fooFn, barFn); // non-default-ctor
  [[maybe_unused]] VTable<Entry<Foo, foo_type>, Entry<Bar, bar_type>> table2(
      table1); // copy-ctor
  [[maybe_unused]] VTable<Entry<Foo, foo_type>, Entry<Bar, bar_type>> table3(
      std::move(table2)); // move-ctor

  [[maybe_unused]] auto table4 = // more convenient than previous ones
      makeVTable(makeEntry(foo_tag, fooFn), makeEntry(bar_tag, barFn));
}

TEST(VTableTests, CompileSimpleAssignments) {
  using namespace CxxPlugins;
  VTable<Entry<Foo, foo_type>, Entry<Bar, bar_type>> table0; // default-ctor

  VTable<Entry<Foo, foo_type>, Entry<Bar, bar_type>> table1; // default-ctor

  table1 = table0;
  table1 = std::move(table0);

  table1 = makeVTable(makeEntry(foo_tag, fooFn), makeEntry(bar_tag, barFn));
}

TEST(VTableTests, MethodsTest) {
  using namespace CxxPlugins;
  auto table = makeVTable(makeEntry(foo_tag, fooFn), makeEntry(bar_tag, barFn));

  EXPECT_EQ(&fooFn, table[foo_tag]);
  EXPECT_EQ(&barFn, table[bar_tag]);

  table.assign(fooFn, barFn);
  EXPECT_EQ(&fooFn, table[foo_tag]);
  EXPECT_EQ(&barFn, table[bar_tag]);

  EXPECT_EQ(&fooFn, std::as_const(table)[foo_tag]);
  EXPECT_EQ(&barFn, std::as_const(table)[bar_tag]);

  EXPECT_EQ(fooFn(nullptr), table.call<Foo>(nullptr));
  EXPECT_EQ(barFn(4), table.call<Bar>(4));

  table.reset();
  EXPECT_EQ(nullptr, table[foo_tag]);
  EXPECT_EQ(nullptr, table[bar_tag]);
}

struct Baz {};

static constexpr Baz baz_tag;

void bazFn(void*, void*) {

}

TEST(VTableTests, UpCasting) {
  using namespace CxxPlugins;
  auto derived_table =
      makeVTable(makeEntry(foo_tag, fooFn), makeEntry(bar_tag, barFn));

  auto base_table0 = makeVTableSubset(derived_table, foo_tag);
  auto base_table1 = makeVTableSubset(derived_table, bar_tag);

  EXPECT_EQ(&fooFn, base_table0[foo_tag]);
  EXPECT_EQ(&barFn, base_table1[bar_tag]);



  auto recreated_table = makeVTableSuperset(base_table0, base_table1);
  EXPECT_EQ(&fooFn, recreated_table[foo_tag]);
  EXPECT_EQ(&barFn, recreated_table[bar_tag]);

  auto baz_table = makeVTable(makeEntry(baz_tag, bazFn));
  auto bigger_table = makeVTableSuperset(base_table0, base_table1, baz_table);
  EXPECT_EQ(&fooFn, bigger_table[foo_tag]);
  EXPECT_EQ(&barFn, bigger_table[bar_tag]);
  EXPECT_EQ(&bazFn, bigger_table[baz_tag]);


  auto base_table2 = makeVTableSubset(std::move(derived_table), foo_tag);
  EXPECT_EQ(&fooFn, base_table2[foo_tag]);
}

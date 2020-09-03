/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    tuple_map_tests.cpp
 * \author  Andrey Ponomarev
 * \date    21 May 2020
 * \brief
 * $BRIEF$
 */

#include <cxx_plugins/polymorphic_traits.hpp>
#include <tuple/tuple_map.hpp>

#include <gtest/gtest.h>

struct Foo {};
struct Bar {};

TEST(TupleMapTests, ConstructorsAndAssignments) {
  using namespace plugins;

  //  TupleMap a(Pair(1,2.f));

  TupleMap<TaggedValue<int, float>> def;
  TupleMap<TaggedValue<int, float>> copy(def);
  TupleMap<TaggedValue<int, float>> m(std::move(def));
  [[maybe_unused]] TupleMap<TaggedValue<int, float>> non_default(0.5F);
  const float val = 0.5F;
  [[maybe_unused]] TupleMap<TaggedValue<int, float>> non_default_const(val);
  copy = m;
  m = std::move(copy);

  [[maybe_unused]] auto empty = makeTupleMap();
  auto complex_map =
      makeTupleMap(TaggedValue(tag<Foo>, 5), TaggedValue(tag<Bar>, 5.0f));
  auto complex_map1 = makeTupleMap(TaggedValue(tag<Foo>, 5), TaggedValue(tag<Bar>, 5.0f));

  TupleMap<TaggedValue<Tag<Bar>, double>, TaggedValue<Tag<Foo>, int64_t>>
      modified_complex_map(complex_map);

  modified_complex_map = complex_map;
  modified_complex_map = std::move(complex_map);
  modified_complex_map = complex_map1;
}

TEST(TupleMapTests, Subscript) {
  using namespace plugins;

  TupleMap<TaggedValue<int, float>> map(0.5F);
  TaggedValue<int, float> result = get<int>(map);
  EXPECT_EQ(result.value_m, 0.5F);
  [[maybe_unused]] const float &result_const =
      get<int>(std::as_const(map)).value_m;
  EXPECT_EQ(result.value_m, 0.5F);
  [[maybe_unused]] float &&result_rvalue = get<int>(std::move(map)).value_m;
  EXPECT_EQ(result_rvalue, 0.5F);

  map = TupleMap<TaggedValue<int, float>>(0.5F);

  EXPECT_EQ(result, map[int()]);
  EXPECT_EQ(result, std::as_const(map)[int()]);
}

TEST(TupleMapTests, Comparison) {
  using namespace plugins;

  auto t0 =
      makeTupleMap(TaggedValue(tag<Foo>, 0.5f), TaggedValue(tag<Bar>, 1.5f));
  auto t1 =
      makeTupleMap(TaggedValue(tag<Foo>, 0.5), TaggedValue(tag<Bar>, 1.5));

  EXPECT_TRUE(t0 == t1);
  EXPECT_FALSE(t0 != t1);

  auto t3 =
      makeTupleMap(TaggedValue(tag<Bar>, 1.5), TaggedValue(tag<Foo>, 0.5));

  EXPECT_TRUE(t0 == t3) << "Reversing order broke.";
  EXPECT_FALSE(t0 != t3);
}

TEST(TupleMapTests, Cat) {
  using namespace plugins;
  auto map0 = makeTupleMap(TaggedValue(tag<Foo>, 5));
  auto map1 = makeTupleMap(TaggedValue(tag<Bar>, 2.0));
  auto expected =
      makeTupleMap(TaggedValue(tag<Foo>, 5), TaggedValue(tag<Bar>, 2.0));

  auto result = tupleCat(map0, map1);
  EXPECT_TRUE(result == expected);
  EXPECT_EQ(expected, result);
}

TEST(TupleMapTests, SubMap) {
  using namespace plugins;
  auto map = makeTupleMap(TaggedValue(tag<Foo>, 5), TaggedValue(tag<Bar>, 2.0));
  static_assert(
      std::is_same_v<decltype(map), TupleMap<TaggedValue<Tag<Foo>, int>,
                                             TaggedValue<Tag<Bar>, double>>>,
      "Map has incorrect type");
  auto expected = makeTupleMap(TaggedValue(tag<Foo>, 5));
  auto result = tupleMapSubMap(map, tag<Foo>);
  static_assert(
      std::is_same_v<decltype(result), TupleMap<TaggedValue<Tag<Foo>, int>>>,
      "result has incorrect type");
  EXPECT_TRUE(result == expected);
  EXPECT_EQ(expected, result);
  auto expected2 = makeTupleMap();
  auto result2 = tupleMapSubMap(map);
  EXPECT_TRUE(result2 == expected2);
  EXPECT_EQ(expected2, result2);
}

// TEST(TupleMapTests, Insert) {
//  using namespace CxxPlugins;
//  auto map = makeTupleMap(makePair(tag<Foo>, 5));
//  auto expected = makeTupleMap(makePair(tag<Foo>, 5),
//  makePair(tag<Bar>, 2.0)); auto result =
//  tupleMapInsertBack(map,makePair(tag<Bar>, 2.0)); EXPECT_TRUE(result ==
//  expected); EXPECT_EQ(expected, result); auto expected2 =
//  makeTupleMap(makePair(tag<Bar>, 2.0),makePair(tag<Foo>, 5)); auto result2 =
//  tupleMapInsertFront(map,makePair(tag<Bar>, 2.0)); EXPECT_TRUE(result2 ==
//  expected2); EXPECT_EQ(expected2, result2);
//
//  EXPECT_TRUE(result2 == result);
//}
//
// TEST(TupleMapTests, Erase) {
//  using namespace CxxPlugins;
//  auto map = makeTupleMap(makePair(tag<Foo>, 5), makePair(tag<Bar>, 2.0));
//  auto expected = makeTupleMap(makePair(tag<Foo>, 5));
//  auto result = tupleMapErase(map, tag<Bar>);
//  EXPECT_TRUE(result == expected);
//  EXPECT_EQ(expected, result);
//  auto expected2 = makeTupleMap();
//  auto result2 = tupleMapErase(map, tag<Bar>, tag<Foo>);
//
//  EXPECT_TRUE(result2 == expected2);
//  EXPECT_EQ(expected2, result2);
//
//}
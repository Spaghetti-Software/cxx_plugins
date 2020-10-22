/*************************************************************************************************
 * Copyright (C) 2020 by DigiPen
 * This file is part of Material Editor project.
 *************************************************************************************************/
/*!
 * \file    polymorphic_cast_tests.cpp
 * \author  Andrey Ponomarev
 * \date    21 Oct 2020
 * \brief
 *
 */

#include <gtest/gtest.h>

#include <cxx_plugins/polymorphic.hpp>
#include <cxx_plugins/polymorphic_ptr.hpp>
#include <cxx_plugins/polymorphic_cast.hpp>

using namespace plugins;

TEST(PolymorphicCast, Void)
{
  PolymorphicPtr<> b;
  auto* ptr = polymorphicCast<int const>(b);
  EXPECT_EQ(ptr, nullptr);

  Polymorphic<> a;
  EXPECT_EQ(polymorphicCast<int>(a), nullptr);

}

TEST(PolymorphicCast, PolymorphicToPolymorphicPtr)
{
  Polymorphic<> a{4};
  PolymorphicPtr<> b{a};

  auto* a_p = polymorphicCast<int>(a);
  auto* b_p = polymorphicCast<int const>(b);

  EXPECT_NE(a_p, nullptr);
  EXPECT_NE(b_p, nullptr);
  EXPECT_EQ(a_p, b_p);

  a.reset();
  b.reset();

  a_p = polymorphicCast<int>(a);
  b_p = polymorphicCast<int const>(b);

  EXPECT_EQ(a_p, nullptr);
  EXPECT_EQ(b_p, nullptr);

  a = 4.f;
  b = a;


  auto* a_pf = polymorphicCast<float>(a);
  auto* b_pf = polymorphicCast<float const>(b);
  EXPECT_NE(a_pf, nullptr);
  EXPECT_NE(b_pf, nullptr);
  EXPECT_EQ(a_pf, b_pf);

}

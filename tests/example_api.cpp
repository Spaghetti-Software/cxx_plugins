/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of cxx_plugins project.
 * License is available at
 *https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    example_api.cpp
 * \author  Andrey Ponomarev
 * \date    09 May 2020
 * \brief
 * This file contains implementation for example API
 */
#include "example_api.hpp"

#include <cxx_plugins/function_traits.hpp>

#include <type_traits>
#include <utility>

namespace ExampleCPPImpl {

Circle::Circle(float x, float y, float radius) noexcept
    : x_m(x), y_m(y), radius_m(radius) {}

void Circle::translate(float x, float y) noexcept {
  x += x;
  y += y;
}

void Circle::rotate([[maybe_unused]] float angle) noexcept {}

void Circle::draw() const noexcept {}

} // namespace ExampleCPPImpl

class MyClass {
  //...
public:
  void some_function(int some_arg);
};

void foo([[maybe_unused]] int i,[[maybe_unused]] int j) {}
void foo([[maybe_unused]]int i) {}

struct Bar {
  void baz([[maybe_unused]]int i) {}

  void baz2([[maybe_unused]]int i, [[maybe_unused]] int j) const {}
  void baz2([[maybe_unused]] int i,[[maybe_unused]] int j) {}
};

/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of cxx_plugins project.
 * License is available at
 *https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    example_api.hpp
 * \author  Andrey Ponomarev
 * \date    08 May 2020
 * \brief
 * This file declares different example APIs for shapes.
 *
 * \description
 * This file doesn't cover all
 * available functionality, but serves more of a prove of concept and other
 * files with more complex APIs should be created for good testing.
 *
 */
#pragma once

#include <cstddef>

namespace ExampleCppAPI {

struct Shape {
public:
  virtual ~Shape() = default;
  virtual void translate(float x, float y) = 0;
  virtual void rotate(float angle) = 0;
  virtual void draw() const = 0;
};
} // namespace ExampleCppAPI

namespace ExampleCPPImpl {

struct Circle : public ExampleCppAPI::Shape {
public:
  Circle() noexcept = default;
  Circle(float x, float y, float radius) noexcept;

  void translate(float x, float y) noexcept override;
  void rotate([[maybe_unused]] float angle) noexcept override;

  void draw() const noexcept override;

  float x_m = 0.f;
  float y_m = 0.f;
  float radius_m = 0.f;
};
} // namespace ExampleCPPImpl

// C example
extern "C" {

struct ExampleC_Shape;

struct CAllocator {
  void *(*allocate_fn)(size_t size, void *user_data) = nullptr;
  void *user_data = nullptr;
};

using ExampleC_Shape_create = ExampleC_Shape *(*)();
using ExampleC_Shape_translate = void (*)(ExampleC_Shape *shape_p, float x,
                                          float y);
using ExampleC_Shape_rotate = void (*)(ExampleC_Shape *shape_p, float angle);
using ExampleC_Shape_draw = void (*)(const ExampleC_Shape *shape_p);
}

extern "C" {}

namespace ExampleHybrid {

class Shape;

using translate_fn_p_t = void (*)(Shape *, float x, float y);
using rotate_fn_p_t = void (*)(Shape *, float x, float y);
using draw_fn_p_t = void (*)(const Shape *);

// Folliwng sections should be replaced with polymorphic

struct ShapeVtable {
  translate_fn_p_t translate_p;
  rotate_fn_p_t rotate_p;
  draw_fn_p_t draw_p;
};

class ShapePolymorphic {
private:
  // vtable pointer/variable
  // pointer to value
  // destroy function
};

} // namespace ExampleHybrid
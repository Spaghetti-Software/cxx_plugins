/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    polymorphic_allocator_tests.cpp
 * \author  Andrey Ponomarev
 * \date    29 Jun 2020
 * \brief
 * $BRIEF$
 */

#include <cxx_plugins/polymorphic_allocator.hpp>

#include <gtest/gtest.h>

#include <string>
#include <vector>

TEST(PolymorphicAllocator, SimpleTests) {
  using namespace CxxPlugins;
  PolymorphicAllocator<> alloc;
  auto ptr = alloc.allocate(10);
  alloc.deallocate(ptr, 10);
  EXPECT_EQ(alloc, PolymorphicAllocator<>{});
}

TEST(PolymorphicAllocator, VectorIntTests) {
  using namespace CxxPlugins;
  std::vector<int, PolymorphicAllocator<>> vec;
  for (int i = 0; i < 10; ++i) {
    vec.resize(i + 1);
    vec[i] = i;
    vec.shrink_to_fit();
  }
}
namespace {
class Foo {
public:
  constexpr Foo() noexcept = default;
  explicit Foo(int val) noexcept { dummy_member_m = std::to_string(val); }
  auto operator=(int val) noexcept -> Foo & {
    dummy_member_m = std::to_string(val);
    return *this;
  }
  auto operator=(unsigned val) noexcept -> Foo & {
    dummy_member_m = std::to_string(val);
    return *this;
  }

private:
  std::string dummy_member_m = {};
};
} // namespace

TEST(PolymorphicAllocator, VectorObjectTests) {
  using namespace CxxPlugins;
  std::vector<Foo, PolymorphicAllocator<>> vec;
  for (int i = 0; i < 10; ++i) {
    vec.resize(i + 1);
    vec[i] = i;
    vec.shrink_to_fit();
  }
}

constexpr std::size_t upTo(std::size_t size) {
  std::size_t result = 1;
  for (std::size_t i = 2; i <= size; ++i) {
    result += i;
  }
  return result;
}

TEST(PolymorphicAllocator, MonotonicBufferTests) {
  using namespace CxxPlugins;
  static constexpr std::size_t max_size = 10;
  std::array<std::byte, sizeof(Foo) * upTo(max_size / 2 + 1)> buffer = {};
  std::array<std::byte, std::size(buffer)> old_buffer = buffer;

  std::pmr::monotonic_buffer_resource resource(std::data(buffer),
                                               std::size(buffer));
  std::vector<Foo, PolymorphicAllocator<>> vec(&resource);

  for (unsigned i = 1; i < max_size; ++i) {
    vec.resize(i + 1);
    vec[i] = i;
    if (i < max_size / 2) {
      EXPECT_NE(buffer, old_buffer)
          << "Buffers should have different data each iteration "
             "if total number of allocated bytes is less then the size of the "
             "buffer.\n"
             "Test failed at i = "
          << i;
    } else {
      EXPECT_EQ(buffer, old_buffer)
          << "Buffers should have same data each iteration "
             "if total number of allocated bytes is more then the size of the "
             "buffer.\n"
             "Test failed at i = "
          << i;
    }
    vec.shrink_to_fit();
    old_buffer = buffer;
  }
}

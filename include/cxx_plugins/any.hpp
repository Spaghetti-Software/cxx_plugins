/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of cxx_plugins project.
 * License is available at
 *https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    any.hpp
 * \author  Timur Kazhimuratov
 * \date    01 June 2020
 *
 * \brief Provides interface and implementation for class any.
 *
 */
#pragma once

#include <utility>

/*!
 * \brief Contains helper functions/classes
 * \details
 * Inside this namespace you will mainly find classes and functions used
 * in implementation but they are generic and can be used by external
 * applications.
 */
namespace CxxPlugins::utility {

  template<class Allocator>
  class any {
  public:
    constexpr any() noexcept = default;
    any(const any &other) {
      
    }
    any(any &&other) noexcept
    : p_m(other.p_m), allocator_m(std::move(other.allocator_m)) {
      other.p_m = nullptr;
    }

    template <class ValueType,
      std::enable_if_t<!std::is_same_v<std::decay_t<ValueType>, any>>>
    explicit any(ValueType &&value) {
      p_m = allocator_m.allocate(sizeof(ValueType)).ptr;
      *static_cast<ValueType *>(p_m) = std::forward<ValueType>(value);
    }

    ~any() {
      allocator_m.deallocate(p_m);
      p_m = nullptr;
    }

    any& operator=(const any &rhs) {
      
    }

    any& operator=(any &&rhs) noexcept {
      if (p_m)
        allocator_m.deallocate(p_m);
      p_m = rhs.p_m;
      allocator_m = std::move(rhs.allocator_m);
      rhs.p_m = nullptr;
      return *this;
    }
  private:
    void *p_m = nullptr;
    Allocator allocator_m;
  };

} // namespace CxxPlugins::utility
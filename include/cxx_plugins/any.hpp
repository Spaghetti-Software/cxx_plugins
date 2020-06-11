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

    any(const any &other) : function_table_m(other.function_table_m) {
      p_m = allocator_m.allocate(other.function_table_m->type_size).ptr;
      function_table_m->copy_ctor_m(other.p_m, p_m);
    }
    any(any &&other) noexcept
    : p_m(other.p_m), allocator_m(std::move(other.allocator_m)) {
      other.p_m = nullptr;
    }

    template <class ValueType,
      std::enable_if_t<!std::is_same_v<std::decay_t<ValueType>, any>, int> = 0>
    any(ValueType &&value) {
      p_m = allocator_m.allocate(sizeof(ValueType)).ptr;
      new (p_m) std::decay_t<ValueType>(std::forward<ValueType>(value));
      function_table_m = &any_table_generator<ValueType>::table;
    }

    ~any() {
      function_table_m->dtor_m(p_m);
      allocator_m.deallocate(p_m);
      p_m = nullptr;
    }

    any &operator=(const any &rhs) {
      if (this == &rhs)
        return *this;

      if (p_m) {
        function_table_m->dtor_m(p_m);
        allocator_m.deallocate(p_m);
      }
      p_m = allocator_m.allocate(rhs.function_table_m->type_size).ptr;
      function_table_m = rhs.function_table_m;
      function_table_m->copy_ctor_m(rhs.p_m, p_m);

      return *this;
    }

    any& operator=(any &&rhs) noexcept {
      if (p_m) {
        allocator_m.deallocate(p_m);
        function_table_m->dtor_m(p_m);
      }
      p_m = rhs.p_m;
      allocator_m = std::move(rhs.allocator_m);
      function_table_m = rhs.function_table_m;
      rhs.p_m = nullptr;
      rhs.function_table_m = nullptr;

      return *this;
    }

    template <typename ValueType,
      std::enable_if_t<!std::is_same_v<std::decay_t<ValueType>, any>, int> = 0>
    any &operator=(ValueType &&rhs) noexcept {
      if (p_m) {
        allocator_m.deallocate(p_m);
        function_table_m->dtor_m(p_m);
      }
      p_m = allocator_m.allocate(sizeof(ValueType)).ptr;
      new (p_m) std::decay_t<ValueType>(std::forward<ValueType>(rhs));
      function_table_m = &any_table_generator<ValueType>::table;

      return *this;
    }

    template <typename T, class Allocator>
    friend const T *any_cast(const any<Allocator> *operand) noexcept;
  private:
    struct any_table {
      void (*dtor_m)(void *);
      void (*copy_ctor_m)(void *, void *);
      unsigned type_size;
    };

    template <typename T>
    static void any_dtor(void *ptr) {
      static_cast<T*>(ptr)->~T();
    }

    template <typename T>
    static void any_copy_ctor(void *toCopy, void *storage) {
      new (storage) T(*static_cast<T *>(toCopy));
    }

    template<typename T>
    struct any_table_generator {
      static constexpr any_table table = {&any_dtor<T>,
                                          &any_copy_ctor<T>,
                                          sizeof(T)};
    };

    template <typename T, class Allocator>
    static bool check_type(const any<Allocator> *operand) noexcept {
      return operand->function_table_m == &any_table_generator<T>::table;
    }
  private:
    const any_table *function_table_m = nullptr;
    void *p_m = nullptr;
    Allocator allocator_m;
  };

  namespace impl {
    
  }

  template<typename T, class Allocator>
  const T *any_cast(const any<Allocator> *operand) noexcept {
    if (operand == nullptr || !any<Allocator>::check_type<T>(operand))
      return nullptr;
    return static_cast<const T *>(operand->p_m);
  }

  template <typename T, class Allocator>
  T *any_cast(any<Allocator> *operand) noexcept {
    if (operand == nullptr || !any<Allocator>::check_type<T>(operand))
      return nullptr;
    return static_cast<T *>(operand->p_m);
  }

  template <typename T, class Allocator>
  T any_cast(const any<Allocator> &operand) {
    auto p = any_cast<T>(&operand);
    if (p == nullptr)
      throw std::runtime_error("Bad any cast");
    return static_cast<T>(*p);
  }

  template<typename T, class Allocator>
  T any_cast(any<Allocator> &operand) {
    return any_cast<T>(static_cast<const any<Allocator>&>(operand));
  }

  template <typename T, class Allocator>
  T any_cast(any<Allocator> &&operand) {
    auto p = any_cast<T>(&operand);
    if (p == nullptr)
      throw std::runtime_error("Bad any cast");
    return static_cast<T>(std::move(*p));
  }

} // namespace CxxPlugins::utility
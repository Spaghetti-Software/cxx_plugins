/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of cxx_plugins project.
 * License is available at
 *https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    polymorphic.hpp
 * \author  Timur Kazhimuratov
 * \date    01 June 2020
 *
 * \brief Provides interface and implementation for class polymorphic.
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
  class polymorphic {
  public:
    constexpr polymorphic() noexcept = default;

    polymorphic(const polymorphic &other) : function_table_m(other.function_table_m) {
      p_m = allocator_m.allocate(other.function_table_m->type_size).ptr;
      function_table_m->copy_ctor_m(other.p_m, p_m);
    }
    polymorphic(polymorphic &&other) noexcept
    : p_m(other.p_m), allocator_m(std::move(other.allocator_m)) {
      other.p_m = nullptr;
    }

    template <class ValueType,
      std::enable_if_t<!std::is_same_v<std::decay_t<ValueType>, polymorphic>, int> = 0>
    polymorphic(ValueType &&value) {
      p_m = allocator_m.allocate(sizeof(ValueType)).ptr;
      new (p_m) std::decay_t<ValueType>(std::forward<ValueType>(value));
      function_table_m = &polymorphic_table_generator<ValueType>::table;
    }

    ~polymorphic() {
      function_table_m->dtor_m(p_m);
      allocator_m.deallocate(p_m);
      p_m = nullptr;
    }

    polymorphic &operator=(const polymorphic &rhs) {
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

    polymorphic& operator=(polymorphic &&rhs) noexcept {
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
      std::enable_if_t<!std::is_same_v<std::decay_t<ValueType>, polymorphic>, int> = 0>
    polymorphic &operator=(ValueType &&rhs) noexcept {
      if (p_m) {
        allocator_m.deallocate(p_m);
        function_table_m->dtor_m(p_m);
      }
      p_m = allocator_m.allocate(sizeof(ValueType)).ptr;
      new (p_m) std::decay_t<ValueType>(std::forward<ValueType>(rhs));
      function_table_m = &polymorphic_table_generator<ValueType>::table;

      return *this;
    }
  private:
    struct polymorphic_table {
      void (*dtor_m)(void *);
      void (*copy_ctor_m)(void *, void *);
      unsigned type_size;
    };

    template <typename T>
    static void polymorphic_dtor(void *ptr) {
      static_cast<T*>(ptr)->~T();
    }

    template <typename T>
    static void polymorphic_copy_ctor(void *toCopy, void *storage) {
      new (storage) T(*static_cast<T *>(toCopy));
    }

    template<typename T>
    struct polymorphic_table_generator {
      static constexpr polymorphic_table table = {&polymorphic_dtor<T>,
                                          &polymorphic_copy_ctor<T>,
                                          sizeof(T)};
    };
  private:
    const polymorphic_table *function_table_m = nullptr;
    void *p_m = nullptr;
    Allocator allocator_m;
  };

} // namespace CxxPlugins::utility
/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    polymorphic.hpp
 * \author  Andrey Ponomarev
 * \author  Timur Kazhimuratovs
 * \date    21 May 2020
 * \brief
 * $BRIEF$
 */
#pragma once

#include "cxx_plugins/vtable.hpp"
#include "cxx_plugins/function_proxy.hpp"
#include "cxx_plugins/polymorphic_traits.hpp"

namespace CxxPlugins {
  
  template<typename Allocator, typename... Tags, typename... FunctionSignatures>
  class Polymorphic {
    template <typename U>
    static constexpr bool is_same = std::is_same_v<std::decay_t<U>, PolymorphicRef>;

    static constexpr bool is_const = (utility::FunctionTraits<FunctionSignatures>::is_const && ...);

    template <typename T> struct underlying;
  
    template <typename T> struct underlying<T const &> {
      using type = T const &;
    };
    template <typename T> struct underlying<T &> {
      using type = std::conditional_t<is_const, T const &, T &>;
    };
    template <typename T> struct underlying<T &&> {
      using type = std::conditional_t<is_const, T const &, T &&>;
    };
    template <typename T> struct underlying<T const &&> {
      using type = T const &&;
    };
  
    template <typename T> using underlying_t = typename underlying<T>::type;
    
    VtableT<TaggedValue<Tags, FunctionSignatures>...> *function_table_p_m = nullptr;
    Allocator allocator_m;
    std::conditional_t<is_const, void const*, void*> obj_m;
  public:
    template<typename T, typename = std::enable_if_t<!is_same<T>>>
    constexpr Polymorphic(T&& t) noexcept :  function_table_p_m(&vtable_v<underlying_t<decltype(t)>, TaggedValue<Tags, FunctionSignatures>...>) {
      obj_m = allocator_m.allocate(sizeof(T)).ptr;
      new (obj_m) std::decay_t<T>(std::forward<T>(t));    
    }
    
    ~Polymorphic() {
      allocator_m.deallocate(obj_m);
      obj_m = nullptr;
    }
    
    template <typename TagT> constexpr auto operator[](TagT&& t) noexcept {
      return FunctionProxy((*function_table_p_m)[std::forward<TagT>(t)], data_p_m);
    }

    template <typename TagT> constexpr auto operator[](TagT&& t) const noexcept {
      return FunctionProxy((*function_table_p_m)[std::forward<TagT>(t)],
                         const_cast<void const *>(data_p_m));
    }
  };
  
} // namespace CxxPlugins
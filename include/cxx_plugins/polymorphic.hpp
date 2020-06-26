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
  
  template <typename... TaggedSignatures> class PolymorphicRef;
  
  namespace impl {
    template <typename Allocator, typename... TaggedSignatures> class Polymorphic;
  } // namespace impl
  
  template <typename Allocator, typename... Ts>
  using Polymorphic = std::conditional_t<
    (is_tagged_value_v<Ts> && ...), impl::Polymorphic<Allocator, Ts...>,
    impl::Polymorphic<Allocator, TaggedValue<Ts, PolymorphicTagSignatureT<Ts>>...>>;
  
  namespace impl { 
  // Type for storing a function pointer to the polymorphic object's dtor in the vtable
  struct obj_dtor_tag {};
  struct obj_copy_ctor_tag {};  
  
  class PrivateFunctions {
  private:
    template<typename Allocator, typename... Ts>
    friend class Polymorphic;
    template<typename... Ts>
    friend class PolymorphicRef;
    
    struct obj_copy_ctor_tag {};
    template<typename T, typename Allocator>
    friend void* polyExtend(obj_copy_ctor_tag, T const&, Allocator&);
  };
  
  
  template<typename Allocator, typename... Tags, typename... FunctionSignatures>
  class Polymorphic<Allocator, TaggedValue<Tags, FunctionSignatures>...> {
    template <typename U>
    static constexpr bool is_self = std::is_same_v<std::decay_t<U>, Polymorphic>;

    static constexpr bool is_const = (utility::FunctionTraits<FunctionSignatures>::is_const && ...);
  public:
    constexpr Polymorphic() noexcept = default;
    constexpr Polymorphic(Polymorphic const & other) noexcept 
      : function_table_p_m{other.functionTable()} {
      data_p_m = other.call<PrivateFunctions::obj_copy_ctor_tag>(allocator_m);
    }
    constexpr Polymorphic(Polymorphic && other) noexcept 
      : allocator_m{std::move(other.allocator_m)}, function_table_p_m{std::move(other.functionTable())}, data_p_m{other.data()} {
      other.data_p_m = nullptr;
    }
    constexpr auto operator=(Polymorphic const & rhs) noexcept
        -> Polymorphic & {
      if (this == &rhs)
        return *this;
        
      destructAndDeallocate();
        
      function_table_m = rhs.functionTable();
      data_p_m = other.call<PrivateFunctions::obj_copy_ctor_tag>(allocator_m);
    }
    constexpr auto operator=(Polymorphic && rhs) noexcept
        -> Polymorphic & {
      if (this == &rhs) 
        return *this;
      
      destructAndDeallocate();
      
      allocator_m = std::move(rhs.allocator_m);
      function_table_m = std::move(rhs.functionTable());
      data_p_m = rhs.data();
      rhs.data_p_m = nullptr;
    }
    /*!
     * \brief
     * Main constructor for Polymorphic.
     * It gets an object of any type, stores a pointer to it and forms a function table.
     *
     */
    template<typename T,
             typename = std::enable_if_t<!is_polymorphic_ref<std::decay_t<T>> && 
                                         !is_polymorphic<std::decay_t<T>>>>
    constexpr Polymorphic(T&& t) noexcept :  function_table_p_m{std::in_place_t<T>{}} {
      allocateAndConstructFromObject(t);
    }
    
    ~Polymorphic() {
      destructAndDeallocate();
      data_p_m = nullptr;
    }
    
    template <typename T,
              typename = std::enable_if_t<!is_polymorphic_ref<std::decay_t<T>> &&
                                          !is_polymorphic<std::decay_t<T>>>>
    /*!
     * \brief Main assignment operator for Polymorphic.
     * It gets object of any type, copies or moves it into a new object and forms a function table.
     *
     */
    constexpr Polymorphic &operator=(T &&obj) noexcept {
      if (this == &rhs) 
        return *this;
      
      destructAndDeallocate();
      
      function_table_m = std::in_place_type_t<T>{};
      allocateAndConstructFromObject(obj);
      return *this;
    }    
    
    //! \brief Returns proxy object to call function
    template <typename TagT> constexpr auto operator[](TagT &&t) noexcept {
      return FunctionProxy(function_table_m[std::forward<TagT>(t)], data_p_m);
    }
  
    template <typename TagT> constexpr auto operator[](TagT &&t) const noexcept {
      return FunctionProxy(function_table_m[std::forward<TagT>(t)],
                           const_cast<void const *>(data_p_m));
    }
  
    template <typename TagT, typename... Us>
    //! \brief Calls function with given parameters
    constexpr decltype(auto) call(Us &&... parameters) {
      return function_table_m[TagT{}](data_p_m, std::forward<Us>(parameters)...);
    }
  
    template <typename TagT, typename... Us>
    constexpr decltype(auto) call(Us &&... parameters) const {
      return function_table_m[TagT{}](const_cast<void const *>(data_p_m),
                                      std::forward<Us>(parameters)...);
    }
  
    [[nodiscard]] auto data() noexcept -> void* { return data_p_m; }
    [[nodiscard]] constexpr auto data() const noexcept -> void const * {
      return data_p_m;
    }
  
    [[nodiscard]] constexpr auto functionTable() const noexcept
        -> VTable<TaggedValue<obj_dtor_tag, void()>, TaggedValue<Tags, FunctionSignatures>...> const & {
      return function_table_m;
    }
  private:
    template<typename T>
    void allocateAndConstructFromObject(T&& t) {
      data_p_m = allocator_m.allocate(sizeof(T)).ptr;
      new (data_p_m) std::decay_t<T>(std::forward<T>(t));
    }
    
    void destructAndDeallocate() {
      if (data_p_m != nullptr) {
        call<obj_dtor_tag>();
        allocator_m.deallocate(data_p_m);
      }
    }
  private:
    Allocator allocator_m;
    void* data_p_m;
    VTableT<
      TaggedValue<PrivateFunctions::obj_copy_ctor_tag, void*()>,
      TaggedValue<obj_dtor_tag, void()>, 
      TaggedValue<Tags, FunctionSignatures>...
    > function_table_m;
  };
  
  template<typename T, typename Allocator>
  void* polyExtend(impl::PrivateFunctions::obj_copy_ctor_tag /*unused*/, T const& obj, Allocator &allocator) {
    auto ptr = allocator.allocate(sizeof(std::decay_t<T>));
    return new (ptr) T(obj);
  }
  
  template<typename T>
  void polyExtend(impl::obj_dtor_tag /*unused*/, T& obj) {
    obj.~T();
  }
  
  } // namespace impl
  
} // namespace CxxPlugins
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
  
  template<typename Allocator, typename... Tags, typename... FunctionSignatures>
  class Polymorphic<Allocator, TaggedValue<Tags, FunctionSignatures>...> {
    template <typename U>
    static constexpr bool is_self = std::is_same_v<std::decay_t<U>, Polymorphic>;

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
  public:
    constexpr Polymorphic() noexcept = default;
    constexpr Polymorphic(Polymorphic const &) noexcept {
      
    }
    constexpr Polymorphic(Polymorphic &&) noexcept {
      
    }
    constexpr auto operator=(Polymorphic const &) noexcept
        -> Polymorphic & {
            
    }
    constexpr auto operator=(Polymorphic &&) noexcept
        -> Polymorphic & {
          
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
      allocateAndConstruct(t);    
    }
    
    template <typename... OtherTags, typename... OtherFunctions,
            bool constraints =
                // Tags >= 1, because otherwise it is a default constructor
            sizeof...(Tags) >= 1 &&
            // Number of this tags should be less or equal then rhs
            (sizeof...(Tags) <= sizeof...(OtherTags)) && !is_const,
            std::enable_if_t<constraints, int> = 0>
    /*!
     * \brief
     * This constructor allows `upcasting` from bigger Polymorphic
     */
    constexpr Polymorphic(Polymorphic<TaggedValue<OtherTags, OtherFunctions>...> &rhs) noexcept
      : function_table_m{rhs.functionTable()} {
        
    }

    template <typename... OtherTags, typename... OtherFunctions,
            bool constraints =
                // Tags >= 1, because otherwise it is a default constructor
            sizeof...(Tags) >= 1 &&
            // Number of this tags should be strictly less then rhs
            // if equal copy assignment operator should be called instead
            (sizeof...(Tags) <= sizeof...(OtherTags)) && is_const,
            std::enable_if_t<constraints, unsigned> = 0>
    constexpr Polymorphic(Polymorphic<TaggedValue<OtherTags, OtherFunctions>...> const &rhs) noexcept
      : function_table_m{rhs.functionTable()} {
        
    }      
    
    template <typename... OtherTags, typename... OtherFunctions,
            bool constraints =
                // Tags >= 1, because otherwise it is a default constructor
            sizeof...(Tags) >= 1 &&
            // Number of this tags should be strictly less then rhs
            // if equal copy assignment operator should be called instead
            (sizeof...(Tags) < sizeof...(OtherTags)) && !is_const,
            std::enable_if_t<constraints, int> = 0>
    /*!
     * \brief This constructor allows `upcasting` from bigger PolymorphicRef or conversion 
     * from PolymorphicRef to Polymorphic
     */
    constexpr Polymorphic(PolymorphicRef<TaggedValue<OtherTags, OtherFunctions>...> &rhs) noexcept
      : function_table_m{rhs.functionTable()} {
        
    }

    template <typename... OtherTags, typename... OtherFunctions,
            bool constraints =
                // Tags >= 1, because otherwise it is a default constructor
            sizeof...(Tags) >= 1 &&
            // Number of this tags should be strictly less then rhs
            // if equal copy assignment operator should be called instead
            (sizeof...(Tags) < sizeof...(OtherTags)) && is_const,
            std::enable_if_t<constraints, unsigned> = 0>
    constexpr Polymorphic(PolymorphicRef<TaggedValue<OtherTags, OtherFunctions>...> const &rhs) noexcept
      : function_table_m{rhs.functionTable()} {
        
    }
    
    ~Polymorphic() {
      allocator_m.deallocate(obj_m);
      obj_m = nullptr;
    }
    
      template <typename T,
            typename = std::enable_if_t<!is_polymorphic_ref<std::decay_t<T>> &&
                                        !is_polymorphic<std::decay_t<T>>>>
    /*!
     * \brief Main assignment operator for PolymorphicRef.
     * It gets object of any type stores pointer to it and forms a function table.
     *
     */
    constexpr Polymorphic &operator=(T &&obj) noexcept {
      function_table_m = std::in_place_type_t<T>{};
      data_p_m = &obj;
      return *this;
    }
  
    template <typename... OtherTags, typename... OtherFunctions,
              bool constraints =
                  // Tags >= 1, because otherwise it is a default constructor
              sizeof...(Tags) >= 1 &&
              // Number of this tags should be strictly less then rhs
              // if equal copy assignment operator should be called instead
              (sizeof...(Tags) <= sizeof...(OtherTags)) && !is_const,
              std::enable_if_t<constraints, int> = 0>
    /*!
     * \brief This assignment operator allows `upcasting` from bigger
     * Polymorphic.
     */
    constexpr Polymorphic &operator=(
        Polymorphic<TaggedValue<OtherTags, OtherFunctions>...> &rhs) noexcept {
      function_table_m = rhs.functionTable();
      data_p_m = rhs.data();
      return *this;
    }
    template <typename... OtherTags, typename... OtherFunctions,
              bool constraints =
                  // Tags >= 1, because otherwise it is a default constructor
              sizeof...(Tags) >= 1 &&
              // Number of this tags should be strictly less then rhs
              // if equal copy assignment operator should be called instead
              (sizeof...(Tags) <= sizeof...(OtherTags)) && is_const,
              std::enable_if_t<constraints, unsigned> = 0>
    constexpr Polymorphic &
    operator=(Polymorphic<TaggedValue<OtherTags, OtherFunctions>...> const
                  &rhs) noexcept {
      function_table_m = rhs.functionTable();
      data_p_m = rhs.data();
      return *this;
    }
  
    template <typename... OtherTags, typename... OtherFunctions,
              bool constraints =
                  // Tags >= 1, because otherwise it is a default constructor
              sizeof...(Tags) >= 1 &&
              // Number of this tags should be strictly less then rhs
              // if equal copy assignment operator should be called instead
              (sizeof...(Tags) < sizeof...(OtherTags)) && !is_const,
              std::enable_if_t<constraints, int> = 0>
    /*!
     * \brief This assignment operator allows `upcasting` from bigger
     * PolymorphicRef and conversion of PolymorphicRef to Polymorphic
     */
    constexpr Polymorphic &operator=(
        PolymorphicRef<TaggedValue<OtherTags, OtherFunctions>...> &rhs) noexcept {
      function_table_m = rhs.functionTable();
      data_p_m = rhs.data();
      return *this;
    }
    template <typename... OtherTags, typename... OtherFunctions,
              bool constraints =
                  // Tags >= 1, because otherwise it is a default constructor
              sizeof...(Tags) >= 1 &&
              // Number of this tags should be strictly less then rhs
              // if equal copy assignment operator should be called instead
              (sizeof...(Tags) < sizeof...(OtherTags)) && is_const,
              std::enable_if_t<constraints, unsigned> = 0>
    constexpr Polymorphic &
    operator=(PolymorphicRef<TaggedValue<OtherTags, OtherFunctions>...> const
                  &rhs) noexcept {
      function_table_m = rhs.functionTable();
      data_p_m = rhs.data();
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
        -> VTable<TaggedValue<Tags, FunctionSignatures>...> const & {
      return function_table_m;
    }
  private:
    template<typename T>
    void allocateAndConstruct(T&& t) {
      data_p_m = allocator_m.allocate(sizeof(T)).ptr;
      new (data_p_m) std::decay_t<T>(std::forward<T>(t));
    }
  private:
    VTableT<TaggedValue<Tags, FunctionSignatures>...> function_table_m;
    Allocator allocator_m;
    void* data_p_m;
  };
  
  } // namespace impl
  
} // namespace CxxPlugins
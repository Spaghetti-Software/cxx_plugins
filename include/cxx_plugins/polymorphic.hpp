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
#include "cxx_plugins/polymorphic_allocator.hpp"
#include "cxx_plugins/memory/stack_allocator.hpp"

namespace CxxPlugins {
  
  namespace impl {
    template <std::size_t size, typename... TaggedSignatures> class GenericPolymorphic;
    template <typename... TaggedSignatures> class PolymorphicRef;
  } // namespace impl
  
  template <typename... Ts>
  using Polymorphic = std::conditional_t<
      (is_tagged_signature<Ts> && ...), impl::GenericPolymorphic<64, Ts...>,
      impl::GenericPolymorphic<
          64, TaggedSignature<Ts, PolymorphicTagSignatureT<Ts>>...>>;
  
  namespace impl { 
  // Type for storing a function pointer to the polymorphic object's dtor in the vtable
  struct obj_dtor_tag {};
  struct obj_copy_ctor_tag {};  

  template <std::size_t size, typename... TaggedSignatures> class GenericPolymorphic;

  class PrivateFunctions {
  private:
    template <std::size_t size, typename... Ts>
    friend class GenericPolymorphic;
    template<typename... Ts>
    friend class PolymorphicRef;
    
    struct obj_copy_ctor_tag {};
    template<typename T>
    friend void *polymorphicExtend(obj_copy_ctor_tag, T const &, void*);
  };
  
  
  template<std::size_t size, typename... Tags, typename... FunctionSignatures>
  class GenericPolymorphic<size, TaggedSignature<Tags, FunctionSignatures>...> {
    template <typename U>
    static constexpr bool is_self =
        std::is_same_v<std::decay_t<U>, GenericPolymorphic>;

    static constexpr bool is_const = (utility::FunctionTraits<FunctionSignatures>::is_const && ...);
  public:
    constexpr GenericPolymorphic() noexcept : function_table_m() {
      setState(State::empty);
    }
    constexpr GenericPolymorphic(GenericPolymorphic const &other) noexcept 
      : function_table_m{other.functionTable()} {
      auto other_state = other.getState();
      if (other_state == State::fallback_allocated)
        new (data_m) FallbackAllocData();
      other.call<PrivateFunctions::obj_copy_ctor_tag>(allocateBasedOnState(other_state, other.getSize(), other.getAlignment()));
    }
    constexpr GenericPolymorphic(GenericPolymorphic &&other) noexcept 
      : function_table_m{std::move(other.functionTable())} {
      std::memcpy(data_m, other.data_m, size);
      auto other_state = other.getState();
      if (other_state == State::fallback_allocated) {
        // change state to empty for now to prevent from deallocating/destructing in destructor,
        // might have to change the object pointer in other to nullptr instead if this doesn't work
        other.setState(State::empty);
      }
    }
    constexpr auto operator=(GenericPolymorphic const &rhs) noexcept
        -> GenericPolymorphic & {
      if (this == &rhs)
        return *this;
        
      destructAndDeallocate();
        
      function_table_m = rhs.functionTable();
      auto rhs_state = rhs.getState();
      if (rhs_state == State::fallback_allocated)
        new (data_m) FallbackAllocData();
      rhs.call<PrivateFunctions::obj_copy_ctor_tag>(allocateBasedOnState(rhs_state, rhs.getSize(), rhs.getAlignment()));

      return *this;
    }
    constexpr auto operator=(GenericPolymorphic &&rhs) noexcept
        -> GenericPolymorphic & {
      if (this == &rhs) 
        return *this;
      
      destructAndDeallocate();
      
      std::memcpy(data_m, rhs.data_m, size);
      auto rhs_state = rhs.getState();
      if (rhs_state == State::fallback_allocated) {
        // change state to empty for now to prevent from
        // deallocating/destructing in destructor, might have to change the
        // object pointer in other to nullptr instead if this doesn't work
        rhs.setState(State::empty);
      }
      function_table_m = std::move(rhs.functionTable());

      return *this;
    }
    /*!
     * \brief
     * Main constructor for Polymorphic.
     * It gets an object of any type, stores a pointer to it and forms a function table.
     *
     */
    template<typename T,
             typename = std::enable_if_t<!is_polymorphic_ref_v<std::decay_t<T>> &&
                                         !is_polymorphic_v<std::decay_t<T>>>>
    constexpr GenericPolymorphic(T &&t) noexcept
        : function_table_m{std::in_place_type_t<std::remove_reference_t<T>>{}} {
      State state;
      if (sizeof(T) <= size - 3)
        state = State::stack_allocated;
      else {
        state = State::fallback_allocated;
        new (data_m) FallbackAllocData();
      }
      new (allocateBasedOnState(state, sizeof(T), alignof(T))) std::decay_t<T>(std::forward<T>(t));
    }
    
    ~GenericPolymorphic() {
      destructAndDeallocate();
    }
    

    template <typename T,
              typename = std::enable_if_t<!is_polymorphic_ref_v<std::decay_t<T>> &&
                                          !is_polymorphic_v<std::decay_t<T>>>>
    /*!
     * \brief Main assignment operator for Polymorphic.
     * It gets object of any type, copies or moves it into a new object and forms a function table.
     *
     */
    constexpr GenericPolymorphic &operator=(T &&obj) noexcept {
      destructAndDeallocate();
      
      function_table_m = std::in_place_type_t<std::remove_reference_t<T>>{};
      State state;
      if (sizeof(T) <= size - 3)
        state = State::stack_allocated;
      else {
        state = State::fallback_allocated;
        new (data_m) FallbackAllocData();
      }
      new (allocateBasedOnState(state, sizeof(T), alignof(T))) std::decay_t<T>(std::forward<T>(obj));

      return *this;
    }    
    
    //! \brief Returns proxy object to call function
    template <typename TagT> constexpr auto operator[](TagT &&t) noexcept {
      return FunctionProxy(function_table_m[std::forward<TagT>(t)], data());
    }
  
    template <typename TagT> constexpr auto operator[](TagT &&t) const noexcept {
      return FunctionProxy(function_table_m[std::forward<TagT>(t)],
                           const_cast<void const *>(data()));
    }
  
    template <typename TagT, typename... Us>
    //! \brief Calls function with given parameters
    constexpr decltype(auto) call(Us &&... parameters) {
      return function_table_m[TagT{}](data(), std::forward<Us>(parameters)...);
    }
  
    template <typename TagT, typename... Us>
    constexpr decltype(auto) call(Us &&... parameters) const {
      return function_table_m[TagT{}](const_cast<void const *>(data()),
                                      std::forward<Us>(parameters)...);
    }
  
    [[nodiscard]] auto data() noexcept -> void * { return getData(); }
    [[nodiscard]] constexpr auto data() const noexcept -> void const * {
      // copy-pasted from getData. I don't know how to not do this with the const function
      auto state = getState();
      switch (state) {
      case State::empty: {
        return nullptr;
      }
      case State::stack_allocated: {
        return data_m;
      }
      case State::fallback_allocated: {
        auto data = reinterpret_cast<const FallbackAllocData *>(data_m);
        return data->obj_p;
      }
      }
    }
  
    [[nodiscard]] constexpr auto functionTable() const noexcept
        -> VTable<TaggedSignature<PrivateFunctions::obj_copy_ctor_tag, void *(void*)>,
                  TaggedSignature<obj_dtor_tag, void()>,
                  TaggedSignature<Tags, FunctionSignatures>...> const & {
      return function_table_m;
    }
  private:
    enum class State : unsigned char {
      empty,
      stack_allocated,
      fallback_allocated
    };

    struct FallbackAllocData {
      FallbackAllocData()
          : obj_p(nullptr), alloc_size(0), alloc_alignment(0) {}
      PolymorphicAllocator<std::byte> allocator;
      void *obj_p;
      std::size_t alloc_size;
      std::size_t alloc_alignment;
    };

    void setState(State state) { data_m[size - 1] = static_cast<char>(state); }

    State getState() const { return static_cast<State>(data_m[size - 1]); }

    std::size_t getSize() const {
      auto state = getState();
      switch (state) {
      case State::empty: {
        return 0;
      }
      case State::stack_allocated: {
        return static_cast<std::size_t>(data_m[size - 2]);
      }
      case State::fallback_allocated: {
        auto data = reinterpret_cast<FallbackAllocData *>(data_m);
        return data->alloc_size;
      }
      }
    }

    std::size_t getAlignment() const {
      auto state = getState();
      switch (state) {
      case State::empty: {
        return 0;
      }
      case State::stack_allocated: {
        return static_cast<std::size_t>(data_m[size - 3]);
      }
      case State::fallback_allocated: {
        auto data = reinterpret_cast<FallbackAllocData *>(data_m);
        return data->alloc_alignment;
      }
      }
    }

    void *getData() {
      auto state = getState();
      switch (state) {
      case State::empty: {
        return nullptr;
      }
      case State::stack_allocated: {
        return data_m;
      }
      case State::fallback_allocated: {
        auto data = reinterpret_cast<FallbackAllocData *>(data_m);
        return data->obj_p;
      }
      }
    }

    void *allocateBasedOnState(State state, std::size_t bytes, std::size_t alignment) {
      void *ret = nullptr;
      switch (state) {
      case State::empty:
        ret = nullptr;
        break;
      case State::stack_allocated:
        ret = data_m;
        data_m[size - 2] = static_cast<char>(bytes);
        data_m[size - 3] = static_cast<char>(alignment);
        break;
      case State::fallback_allocated: {
        auto alloc_data = reinterpret_cast<FallbackAllocData *>(data_m);
        ret = alloc_data->allocator.allocate_bytes(bytes, alignment);
        alloc_data->obj_p = ret;
        alloc_data->alloc_size = bytes;
        alloc_data->alloc_alignment = alignment;
        break;
      }
      }

      setState(state);
      return ret;
    }
    
    void destructAndDeallocate() {
      auto state = getState();
      if (state == State::empty)
        return;

      call<obj_dtor_tag>();

      
      switch (state) {
      case State::fallback_allocated: {
        auto alloc_data = reinterpret_cast<FallbackAllocData *>(data_m);
        alloc_data->allocator.deallocate_bytes(alloc_data->obj_p,
                                               alloc_data->alloc_size,
                                               alloc_data->alloc_alignment);
        alloc_data->obj_p = nullptr;
      } break;
      default:
        break;
      }
      setState(State::empty);
    }
  private:
    char data_m[size];
    VTable<TaggedSignature<PrivateFunctions::obj_copy_ctor_tag, void *(void*)>,
           TaggedSignature<obj_dtor_tag, void()>, 
           TaggedSignature<Tags, FunctionSignatures>...
    > function_table_m;
  };

  template<typename T>
  void *polymorphicExtend(PrivateFunctions::obj_copy_ctor_tag /*unused*/,
                          T const &obj, void* ptr) {
    return new (ptr) T(obj);
  }
  
  template<typename T>
  void polymorphicExtend(obj_dtor_tag /*unused*/, T &obj) {
    obj.~T();
  }
  
  } // namespace impl

} // namespace CxxPlugins
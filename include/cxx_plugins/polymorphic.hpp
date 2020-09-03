/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    polymorphic.hpp
 * \author  Timur Kazhimuratov
 * \author  Andrey Ponomarev
 * \date    21 May 2020
 * \brief
 * $BRIEF$
 */
#pragma once

#include "cxx_plugins/function_proxy.hpp"
#include "cxx_plugins/memory/stack_allocator.hpp"
#include "cxx_plugins/polymorphic_allocator.hpp"
#include "cxx_plugins/vtable.hpp"

namespace plugins {

namespace impl {
struct obj_dtor_tag {};
struct obj_copy_ctor_tag {};
} // namespace impl

template <std::size_t size, typename... TaggedSignatures>
class UniqueGenericPolymorphic;

template <std::size_t size, typename... TaggedSignatures>
using GenericPolymorphic = UniqueGenericPolymorphic<
    size,
    TaggedSignature<impl::obj_copy_ctor_tag,
                    void *(void *)const>,
    TaggedSignatures...>;

template <typename... Ts>
using Polymorphic = std::conditional_t<
    (is_tagged_signature<Ts> && ...),
    GenericPolymorphic<64, Ts...>,
    GenericPolymorphic<64, TaggedSignature<Ts, PolymorphicTagSignatureT<Ts>>...>>;

template <typename... Ts>
using UniquePolymorphic = std::conditional_t<
    (is_tagged_signature<Ts> && ...),
    UniqueGenericPolymorphic<64, Ts...>,
    UniqueGenericPolymorphic<64, TaggedSignature<Ts, PolymorphicTagSignatureT<Ts>>...>>;

/*!
 *
 */
#ifdef DOXYGEN
template<std::size_t size, typename... TaggedSignatures>
class UniqueGenericPolymorphic
#else
template <std::size_t size, typename... Tags, typename... FunctionSignatures>
class UniqueGenericPolymorphic<size, TaggedSignature<Tags, FunctionSignatures>...>
#endif
{
  template <typename U>
  static constexpr bool is_self =
      std::is_same_v<std::decay_t<U>, UniqueGenericPolymorphic>;

  static constexpr bool is_const =
      (utility::FunctionTraits<FunctionSignatures>::is_const && ...);

public:
  using FunctionTableT =
      VTable<TaggedSignature<impl::obj_dtor_tag, void()>,
             TaggedSignature<Tags, FunctionSignatures>...>;

  constexpr UniqueGenericPolymorphic() noexcept : function_table_m() {
    setState(State::empty);
  }
  constexpr UniqueGenericPolymorphic(UniqueGenericPolymorphic const &other) noexcept
      : function_table_m{other.functionTable()} {

    static_assert(
        utility::is_in_the_pack_v<impl::obj_copy_ctor_tag, Tags...>,
        "This Polymorphic is not copyable"); 

    auto other_state = other.getState();
    if (other_state == State::fallback_allocated)
      new (data_m) FallbackAllocData();
    other.call<impl::obj_copy_ctor_tag>(allocateBasedOnState(
        other_state, other.getSize(), other.getAlignment()));
  }
  constexpr UniqueGenericPolymorphic(UniqueGenericPolymorphic &&other) noexcept
      : function_table_m{std::move(other.functionTable())} {
    std::memcpy(data_m, other.data_m, size);
    auto other_state = other.getState();

    // change state to empty for now to prevent from deallocating/destructing in destructor
    other.setState(State::empty);
  }

  constexpr auto operator=(UniqueGenericPolymorphic const &rhs) noexcept
      -> UniqueGenericPolymorphic & {
    static_assert(
        utility::is_in_the_pack_v<impl::obj_copy_ctor_tag, Tags...>,
        "This Polymorphic is not copyable"); 

    if (this == &rhs)
      return *this;

    destructAndDeallocate();

    function_table_m = rhs.functionTable();
    auto rhs_state = rhs.getState();
    if (rhs_state == State::fallback_allocated)
      new (data_m) FallbackAllocData();
    rhs.call<impl::obj_copy_ctor_tag>(
        allocateBasedOnState(rhs_state, rhs.getSize(), rhs.getAlignment()));

    return *this;
  }
  constexpr auto operator=(UniqueGenericPolymorphic &&rhs) noexcept
      -> UniqueGenericPolymorphic & {
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
   * It gets an object of any type, stores a pointer to it and forms a function
   * table.
   *
   */
  template <typename T, typename = std::enable_if_t<
                            !is_polymorphic_ref_v<std::decay_t<T>> &&
                            !is_polymorphic_v<std::decay_t<T>>>>
  constexpr UniqueGenericPolymorphic(T &&t) noexcept
      : function_table_m{std::in_place_type_t<std::remove_reference_t<T>>{}} {
    static_assert(
        std::is_rvalue_reference_v<T &&> ||
            (std::is_lvalue_reference_v<T &&> &&
             utility::is_in_the_pack_v<impl::obj_copy_ctor_tag, Tags...>),
        "This Polymorphic is not copyable"); 

    State state;
    if (sizeof(T) <= size - 3)
      state = State::stack_allocated;
    else {
      state = State::fallback_allocated;
      new (data_m) FallbackAllocData();
    }
    new (allocateBasedOnState(state, sizeof(T), alignof(T)))
        std::decay_t<T>(std::forward<T>(t));
  }

  ~UniqueGenericPolymorphic() { destructAndDeallocate(); }

  template <typename T, typename = std::enable_if_t<
                            !is_polymorphic_ref_v<std::decay_t<T>> &&
                            !is_polymorphic_v<std::decay_t<T>>>>
  /*!
   * \brief Main assignment operator for Polymorphic.
   * It gets object of any type, copies or moves it into a new object and forms
   * a function table.
   *
   */
  constexpr UniqueGenericPolymorphic &operator=(T &&obj) noexcept {
    destructAndDeallocate();

    static_assert(
        std::is_rvalue_reference_v<T &&> ||
            (std::is_lvalue_reference_v<T &&> &&
             utility::is_in_the_pack_v<impl::obj_copy_ctor_tag, Tags...>),
        "This Polymorphic is not copyable"); 

    function_table_m = std::in_place_type_t<std::remove_reference_t<T>>{};
    State state;
    if (sizeof(T) <= size - 3)
      state = State::stack_allocated;
    else {
      state = State::fallback_allocated;
      new (data_m) FallbackAllocData();
    }
    new (allocateBasedOnState(state, sizeof(T), alignof(T)))
        std::decay_t<T>(std::forward<T>(obj));

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
    // copy-pasted from getData. I don't know how to not do this with the const
    // function
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
      -> FunctionTableT const & {
    return function_table_m;
  }

  template <typename T, typename... Args>
  void emplace(std::in_place_type_t<T> type, Args &&... args) noexcept(
      std::is_nothrow_constructible_v<T, Args &&...>) {
    destructAndDeallocate();

    function_table_m = std::in_place_type_t<std::remove_reference_t<T>>{};
    State state;
    if (sizeof(T) <= size - 3) {
      state = State::stack_allocated;
    } else {
      state = State::fallback_allocated;
      new (data_m) FallbackAllocData();
    }
    new (allocateBasedOnState(state, sizeof(T), alignof(T)))
        std::decay_t<T>(std::forward<Args>(args)...);
  }

private:
  enum class State : unsigned char {
    empty,
    stack_allocated,
    fallback_allocated
  };

  struct FallbackAllocData {
    FallbackAllocData() : obj_p(nullptr), alloc_size(0), alloc_alignment(0) {}
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
      auto data = reinterpret_cast<FallbackAllocData const *>(data_m);
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
      auto data = reinterpret_cast<FallbackAllocData const *>(data_m);
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

  void *allocateBasedOnState(State state, std::size_t bytes,
                             std::size_t alignment) {
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

    call<impl::obj_dtor_tag>();

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
  FunctionTableT function_table_m;
};

namespace impl {
  template <typename T>
  void *polymorphicExtend(impl::obj_copy_ctor_tag /*unused*/, T const &obj, void *ptr) {
    return new (ptr) T(obj);
  }

  template <typename T>
  void polymorphicExtend(impl::obj_dtor_tag /*unused*/, T &obj) {
    obj.~T();
  }
} // namespace impl

} // namespace CxxPlugins
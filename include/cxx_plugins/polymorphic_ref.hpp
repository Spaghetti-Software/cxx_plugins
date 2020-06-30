/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    polymorphic_ref.hpp
 * \author  Andrey Ponomarev
 * \date    18 Jun 2020
 * \brief
 * $BRIEF$
 */
#pragma once

#include "cxx_plugins/function_proxy.hpp"
#include "cxx_plugins/vtable.hpp"

#include <boost/type_index.hpp>

namespace CxxPlugins {

template <typename Allocator, typename... TaggedSignatures> class Polymorphic;



namespace impl {
template <typename... TaggedSignatures> class PolymorphicRef;
template <typename... TaggedSignatures> class PrimitivePolymorphicRef;
} // namespace impl

template <typename... Ts>
using PolymorphicRef = std::conditional_t<
    (is_tagged_signature<Ts> && ...), impl::PolymorphicRef<Ts...>,
    impl::PolymorphicRef<TaggedSignature<Ts, PolymorphicTagSignatureT<Ts>>...>>;

template <typename... Ts>
using PrimitivePolymorphicRef =
    std::conditional_t<(is_tagged_signature<Ts> && ...),
                       impl::PrimitivePolymorphicRef<Ts...>,
                       impl::PrimitivePolymorphicRef<
                           TaggedSignature<Ts, PolymorphicTagSignatureT<Ts>>...>>;

namespace impl {
template <typename... TaggedSignatures> class PolymorphicRef;

/*!
 * \brief
 * PolymorphicRef is a pointer-like wrapper for polymorphic objects.
 */
#ifdef DOXYGEN
template <typename... TaggedSignatures>
class PolymorphicRef
#else
template <typename... Tags, typename... Signatures>
class PolymorphicRef<TaggedSignature<Tags, Signatures>...>
#endif
{
private:
  template <typename U>
  static constexpr bool is_self =
      std::is_same_v<std::decay_t<U>, PolymorphicRef>;

  static constexpr bool is_const =
      (utility::FunctionTraits<Signatures>::is_const && ...);

public:
  using PointerT = std::conditional_t<is_const, void const *, void *>;
  using FunctionTableT = VTable<
      TaggedSignature<Tags, PolymorphicSignatureT<PolymorphicRef, Signatures>>...>;

  constexpr PolymorphicRef() noexcept = default;
  constexpr PolymorphicRef(PolymorphicRef const &) noexcept = default;
  constexpr PolymorphicRef(PolymorphicRef &&) noexcept = default;
  constexpr auto operator=(PolymorphicRef const &) noexcept
      -> PolymorphicRef & = default;
  constexpr auto operator=(PolymorphicRef &&) noexcept
      -> PolymorphicRef & = default;

  template <typename T, typename = std::enable_if_t<
                            !is_polymorphic_ref_v<std::decay_t<T>> &&
                            !is_polymorphic_v<std::decay_t<T>> &&
                            !std::is_pointer_v<std::decay_t<T>>>>
  /*!
   * \brief
   * Main constructor for PolymorphicRef.
   * It gets object of any type stores pointer to it and forms a function table.
   *
   */
  constexpr PolymorphicRef(T &&obj) noexcept
      : type_index_m{boost::typeindex::type_id<T>()}, data_p_m{&obj},
        function_table_m{std::in_place_type_t<decltype(obj)>{}} {}

  template <typename T, typename = std::enable_if_t<
                            !is_polymorphic_ref_v<std::decay_t<T>> &&
                            !is_polymorphic_v<std::decay_t<T>>>>
  /*!
   * \brief
   * Converts pointer to object to PolymorphicRef
   *
   */
  constexpr PolymorphicRef(T *obj) noexcept : PolymorphicRef(*obj) {}

  template <typename... OtherTags, typename... OtherFunctions,
            bool const_contraint = !is_const,
            std::enable_if_t<const_contraint, int> = 0,
            bool size_constraint = sizeof...(Tags) >= 1 &&
                                   sizeof...(Tags) < sizeof...(OtherTags),
            std::enable_if_t<size_constraint, int> = 0>
  /*!
   * \brief This constructor allows `upcasting` from bigger PolymorphicRef.
   */
  constexpr PolymorphicRef(
      PolymorphicRef<TaggedSignature<OtherTags, OtherFunctions>...> &rhs) noexcept
      : type_index_m{rhs.typeIndex()}, data_p_m{rhs.data()},
        function_table_m{rhs.functionTable()} {}

  template <typename... OtherTags, typename... OtherFunctions,
            bool const_contraint = is_const,
            std::enable_if_t<const_contraint, unsigned> = 0,
            bool size_constraint = sizeof...(Tags) >= 1 &&
                                   sizeof...(Tags) < sizeof...(OtherTags),
            std::enable_if_t<size_constraint, int> = 0>
  constexpr PolymorphicRef(
      PolymorphicRef<TaggedSignature<OtherTags, OtherFunctions>...> const
          &rhs) noexcept
      : type_index_m{rhs.typeIndex()}, data_p_m{rhs.data()},
        function_table_m{rhs.functionTable()} {}

  template <typename... OtherTags, typename... OtherFunctions,
            bool const_contraint = !is_const,
            std::enable_if_t<const_contraint, int> = 0,
            bool size_constraint = sizeof...(Tags) >= 1 &&
                                   sizeof...(Tags) == sizeof...(OtherTags),
            std::enable_if_t<size_constraint, unsigned> = 0,
            bool conversion_contraint = (!std::is_same_v<Tags, OtherTags> ||
                                         ...),
            std::enable_if_t<conversion_contraint, int> = 0>
  /*!
   * \brief This constructor allows conversion of PolymorphicRef with different
   * order
   */
  constexpr PolymorphicRef(
      PolymorphicRef<TaggedSignature<OtherTags, OtherFunctions>...> &rhs) noexcept
      : type_index_m{rhs.typeIndex()}, data_p_m{rhs.data()},
        function_table_m{rhs.functionTable()} {}

  template <typename... OtherTags, typename... OtherFunctions,
            bool const_contraint = is_const,
            std::enable_if_t<const_contraint, unsigned> = 0,
            bool size_constraint = sizeof...(Tags) >= 1 &&
                                   sizeof...(Tags) == sizeof...(OtherTags),
            std::enable_if_t<size_constraint, unsigned> = 0,
            bool conversion_contraint = (!std::is_same_v<Tags, OtherTags> ||
                                         ...),
            std::enable_if_t<conversion_contraint, int> = 0>
  constexpr PolymorphicRef(
      PolymorphicRef<TaggedSignature<OtherTags, OtherFunctions>...> const
          &rhs) noexcept
      : type_index_m{rhs.typeIndex()}, data_p_m{rhs.data()},
        function_table_m{rhs.functionTable()} {}

  template <typename... OtherTags, typename... OtherFunctions,
            bool constraints =
                // Tags >= 1, because otherwise it is a default constructor
            sizeof...(Tags) >= 1 &&
            // Number of this tags should be less or equal then rhs
            (sizeof...(Tags) <= sizeof...(OtherTags)) && !is_const,
            std::enable_if_t<constraints, int> = 0>
  /*!
   * \brief
   * This constructor allows `upcasting` from bigger Polymorphic or convertion
   * from Polymorphic to PolymorphicRef
   */
  constexpr PolymorphicRef(
      Polymorphic<TaggedSignature<OtherTags, OtherFunctions>...> &rhs) noexcept
      : type_index_m{rhs.typeIndex()}, data_p_m{rhs.data()},
        function_table_m{rhs.functionTable()} {}

  template <typename... OtherTags, typename... OtherFunctions,
            bool constraints =
                // Tags >= 1, because otherwise it is a default constructor
            sizeof...(Tags) >= 1 &&
            // Number of this tags should be strictly less then rhs
            // if equal copy assignment operator should be called instead
            (sizeof...(Tags) <= sizeof...(OtherTags)) && is_const,
            std::enable_if_t<constraints, unsigned> = 0>
  constexpr PolymorphicRef(
      Polymorphic<TaggedSignature<OtherTags, OtherFunctions>...> const
          &rhs) noexcept
      : type_index_m{rhs.typeIndex()}, data_p_m{rhs.data()},
        function_table_m{rhs.functionTable()} {}

  template <typename T, typename = std::enable_if_t<
                            !is_polymorphic_ref_v<std::decay_t<T>> &&
                            !is_polymorphic_v<std::decay_t<T>> &&
                            !std::is_pointer_v<std::decay_t<T>>>>
  /*!
   * \brief Main assignment operator for PolymorphicRef.
   * It gets object of any type stores pointer to it and forms a function table.
   *
   */
  constexpr PolymorphicRef &operator=(T &&obj) noexcept {
    type_index_m = boost::typeindex::type_id<T>();
    function_table_m = std::in_place_type_t<decltype(obj)>{};
    data_p_m = &obj;
    return *this;
  }

  template <typename T, typename = std::enable_if_t<
                            !is_polymorphic_ref_v<std::decay_t<T>> &&
                            !is_polymorphic_v<std::decay_t<T>>>>
  /*!
   * \brief Main assignment operator for PolymorphicRef.
   * It gets object of any type stores pointer to it and forms a function table.
   *
   */
  constexpr PolymorphicRef &operator=(T *obj) noexcept {
    *this = *obj;
    return *this;
  }

  template <typename... OtherTags, typename... OtherFunctions,
            bool const_contraint = !is_const,
            std::enable_if_t<const_contraint, int> = 0,
            bool size_constraint = sizeof...(Tags) >= 1 &&
                                   sizeof...(Tags) < sizeof...(OtherTags),
            std::enable_if_t<size_constraint, int> = 0>
  /*!
   * \brief This assignment operator allows `upcasting` from bigger
   * PolymorphicRef.
   */
  constexpr PolymorphicRef &operator=(
      PolymorphicRef<TaggedSignature<OtherTags, OtherFunctions>...> &rhs) noexcept {
    type_index_m = rhs.typeIndex();
    function_table_m = rhs.functionTable();
    data_p_m = rhs.data();
    return *this;
  }
  template <typename... OtherTags, typename... OtherFunctions,
            bool const_contraint = is_const,
            std::enable_if_t<const_contraint, unsigned> = 0,
            bool size_constraint = sizeof...(Tags) >= 1 &&
                                   sizeof...(Tags) < sizeof...(OtherTags),
            std::enable_if_t<size_constraint, int> = 0>
  constexpr PolymorphicRef &
  operator=(PolymorphicRef<TaggedSignature<OtherTags, OtherFunctions>...> const
                &rhs) noexcept {
    type_index_m = rhs.typeIndex();
    function_table_m = rhs.functionTable();
    data_p_m = rhs.data();
    return *this;
  }

  template <typename... OtherTags, typename... OtherFunctions,
            bool const_contraint = !is_const,
            std::enable_if_t<const_contraint, int> = 0,
            bool size_constraint = sizeof...(Tags) >= 1 &&
                                   sizeof...(Tags) == sizeof...(OtherTags),
            std::enable_if_t<size_constraint, unsigned> = 0,
            bool conversion_contraint = (!std::is_same_v<Tags, OtherTags> &&
                                         ...),
            std::enable_if_t<conversion_contraint, unsigned> = 0>
  /*!
   * \brief This assignment operator allows `upcasting` from bigger
   * PolymorphicRef.
   */
  constexpr PolymorphicRef &operator=(
      PolymorphicRef<TaggedSignature<OtherTags, OtherFunctions>...> &rhs) noexcept {
    type_index_m = rhs.typeIndex();
    function_table_m = rhs.functionTable();
    data_p_m = rhs.data();
    return *this;
  }
  template <typename... OtherTags, typename... OtherFunctions,
            bool const_contraint = is_const,
            std::enable_if_t<const_contraint, unsigned> = 0,
            bool size_constraint = sizeof...(Tags) >= 1 &&
                                   sizeof...(Tags) == sizeof...(OtherTags),
            std::enable_if_t<size_constraint, unsigned> = 0,
            bool conversion_contraint = (!std::is_same_v<Tags, OtherTags> &&
                                         ...),
            std::enable_if_t<conversion_contraint, unsigned> = 0>
  constexpr PolymorphicRef &
  operator=(PolymorphicRef<TaggedSignature<OtherTags, OtherFunctions>...> const
                &rhs) noexcept {
    type_index_m = rhs.typeIndex();
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
            (sizeof...(Tags) <= sizeof...(OtherTags)) && !is_const,
            std::enable_if_t<constraints, int> = 0>
  /*!
   * \brief This assignment operator allows `upcasting` from bigger
   * Polymorphic and conversion of Polymorphic to PolymorphicRef.
   */
  constexpr PolymorphicRef &operator=(
      Polymorphic<TaggedSignature<OtherTags, OtherFunctions>...> &rhs) noexcept {
    type_index_m = rhs.typeIndex();
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
  constexpr PolymorphicRef &
  operator=(Polymorphic<TaggedSignature<OtherTags, OtherFunctions>...> const
                &rhs) noexcept {
    type_index_m = rhs.typeIndex();
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
    auto fn_ptr = function_table_m[TagT{}];
    return fn_ptr(data_p_m, std::forward<Us>(parameters)...);
  }

  template <typename TagT, typename... Us>
  constexpr decltype(auto) call(Us &&... parameters) const {
    return function_table_m[TagT{}](const_cast<void const *>(data_p_m),
                                    std::forward<Us>(parameters)...);
  }

  [[nodiscard]] auto data() noexcept -> PointerT { return data_p_m; }
  [[nodiscard]] constexpr auto data() const noexcept -> void const * {
    return data_p_m;
  }

  [[nodiscard]] constexpr auto functionTable() const noexcept
      -> FunctionTableT const & {
    return function_table_m;
  }

  [[nodiscard]] auto isEmpty() const noexcept -> bool {
    return data_p_m == nullptr;
  }
  void reset() noexcept {
    type_index_m = boost::typeindex::type_id<void>();
    data_p_m = nullptr;
    function_table_m.reset();
  }

  template <typename T> constexpr auto isA() const noexcept -> bool {
    return boost::typeindex::type_id<T> == type_index_m;
  }

  constexpr auto typeIndex() const noexcept
      -> boost::typeindex::type_index const & {
    return type_index_m;
  }

private:
  /*
   * Order of members is important.
   * Function table is aligned the same way as void*, but it ends with array
   * of uint8_t which means that if we swap these to members there are gonna be
   * padding bytes between members and size of these padding bytes can differ
   * on different compilers.
   */
  boost::typeindex::type_index type_index_m = {};
  PointerT data_p_m = nullptr;
  FunctionTableT function_table_m;
};

template <typename... Signatures> class PrimitivePolymorphicRef;

/*!
 * \brief
 * PolymorphicRSimplePolymorphicReflike wrapper for polymorphic objects.
 */
#ifdef DOXYGEN
template <typename... TaggedSignatures>
class PrimitivePolymorphicRef
#else
template <typename... Tags, typename... Signatures>
class PrimitivePolymorphicRef<TaggedSignature<Tags, Signatures>...>
#endif
{
private:
  template <typename U>
  static constexpr bool is_self =
      std::is_same_v<std::decay_t<U>, PolymorphicRef>;

  static constexpr bool is_const =
      (utility::FunctionTraits<Signatures>::is_const && ...);

public:
  using PointerT = std::conditional_t<is_const, void const *, void *>;
  using FunctionTableT = PrimitiveVTable<TaggedSignature<
      Tags, PolymorphicSignatureT<PrimitivePolymorphicRef, Signatures>>...>;

  constexpr PrimitivePolymorphicRef() noexcept = default;
  constexpr PrimitivePolymorphicRef(PrimitivePolymorphicRef const &) noexcept =
      default;
  constexpr PrimitivePolymorphicRef(PrimitivePolymorphicRef &&) noexcept =
      default;
  constexpr auto operator=(PrimitivePolymorphicRef const &) noexcept
      -> PrimitivePolymorphicRef & = default;
  constexpr auto operator=(PrimitivePolymorphicRef &&) noexcept
      -> PrimitivePolymorphicRef & = default;

  template <typename T, typename = std::enable_if_t<
                            !is_polymorphic_ref_v<std::decay_t<T>> &&
                            !is_polymorphic_v<std::decay_t<T>> &&
                            !std::is_pointer_v<std::decay_t<T>>>>
  /*!
   * \brief
   * Main constructor for SimplePolymorphicRef.
   * It gets object of any type stores pointer to it and forms a function table.
   *
   */
  constexpr PrimitivePolymorphicRef(T &&obj) noexcept
      : type_index_m{boost::typeindex::type_id<T>()}, data_p_m{&obj},
        function_table_m{std::in_place_type_t<decltype(obj)>{}} {
  }

  template <typename T, typename = std::enable_if_t<
                            !is_polymorphic_ref_v<std::decay_t<T>> &&
                            !is_polymorphic_v<std::decay_t<T>>>>
  /*!
   * \brief
   * Conversion constructor for pointers.
   *
   */
  constexpr PrimitivePolymorphicRef(T *obj) noexcept
      : PrimitivePolymorphicRef(*obj) {}

  template <typename T, typename = std::enable_if_t<
                            !is_polymorphic_ref_v<std::decay_t<T>> &&
                            !is_polymorphic_v<std::decay_t<T>> &&
                            !std::is_pointer_v<std::decay_t<T>>>>
  /*!
   * \brief Main assignment operator for SimplePolymorphicRef.
   * It gets object of any type stores pointer to it and forms a function table.
   *
   */
  constexpr PrimitivePolymorphicRef &operator=(T &&obj) noexcept {
    type_index_m = boost::typeindex::type_id<T>();
    function_table_m = std::in_place_type_t<decltype(obj)>{};
    data_p_m = &obj;
    return *this;
  }

  template <typename T, typename = std::enable_if_t<
                            !is_polymorphic_ref_v<std::decay_t<T>> &&
                            !is_polymorphic_v<std::decay_t<T>>>>
  /*!
   * \brief
   * Conversion assignment for pointers
   *
   */
  constexpr PrimitivePolymorphicRef &operator=(T *obj) noexcept {
    *this = *obj;
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
    auto fn_ptr = function_table_m[TagT{}];
    return fn_ptr(data_p_m, std::forward<Us>(parameters)...);
  }

  template <typename TagT, typename... Us>
  constexpr decltype(auto) call(Us &&... parameters) const {
    return function_table_m[TagT{}](const_cast<void const *>(data_p_m),
                                    std::forward<Us>(parameters)...);
  }

  [[nodiscard]] auto data() noexcept -> PointerT { return data_p_m; }
  [[nodiscard]] constexpr auto data() const noexcept -> void const * {
    return data_p_m;
  }

  [[nodiscard]] constexpr auto functionTable() const noexcept
      -> FunctionTableT const & {
    return function_table_m;
  }

  [[nodiscard]] auto isEmpty() const noexcept -> bool {
    return data_p_m == nullptr;
  }
  void reset() noexcept {
    type_index_m = boost::typeindex::type_id<void>();
    data_p_m = nullptr;
    function_table_m.reset();
  }

  template <typename T> constexpr auto isA() const noexcept -> bool {
    return boost::typeindex::type_id<T>() == type_index_m;
  }

  constexpr auto typeIndex() const noexcept
      -> boost::typeindex::type_index const & {
    return type_index_m;
  }

private:
  boost::typeindex::type_index type_index_m = {};
  PointerT data_p_m = nullptr;
  FunctionTableT function_table_m;
};

} // namespace impl

template <typename T, typename PolymorphicT>
auto polymorphicCast(PolymorphicT &&poly) -> T * {
  if (poly.template isA<T>()) {
    return static_cast<T *>(poly.data());
  }
  return nullptr;
}

} // namespace CxxPlugins

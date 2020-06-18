/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    basic_tuple.hpp
 * \author  Andrey Ponomarev
 * \date    10 Jun 2020
 * \brief
 * This file defines basic tuple storage
 */
#pragma once

#include "sequence/map.hpp"        // integer sequence indexing
#include "tuple/tuple_helpers.hpp" // tupleSize and tupleElement

#include <algorithm> // std::max
#include <utility>   // std::pair

namespace CxxPlugins {

///////////////////////////////////////////////////////
/// DECLARATIONS
//////////////////////////////////////////////////////

template <typename T, typename U> using Pair = std::pair<T, U>;

template <typename T, typename U> constexpr auto makePair(T &&t, U &&u);

template <typename... Ts> struct TupleStorage;

template <std::size_t I, typename... Ts>
constexpr auto get(TupleStorage<Ts...> const &t) noexcept ->
    typename TupleElement<I, TupleStorage<Ts...>>::Type const &;

template <std::size_t I, typename... Ts>
constexpr auto get(TupleStorage<Ts...> &t) noexcept ->
    typename TupleElement<I, TupleStorage<Ts...>>::Type &;

template <std::size_t I, typename... Ts>
constexpr auto get(TupleStorage<Ts...> &&t) noexcept ->
    typename TupleElement<I, TupleStorage<Ts...>>::Type &&;

template <std::size_t I, typename... Ts>
constexpr auto get(TupleStorage<Ts...> const &&t) noexcept ->
    typename TupleElement<I, TupleStorage<Ts...>>::Type const &&;

namespace impl {

template <std::size_t I, typename T> struct TupleInnerElement;
template <std::size_t I, typename T>
using TupleInnerElementT = typename TupleInnerElement<I, T>::Type;

template <std::size_t I, typename... Ts>
constexpr auto getInner(TupleStorage<Ts...> const &t) noexcept ->
    typename TupleInnerElement<I, TupleStorage<Ts...>>::Type const &;

template <std::size_t I, typename... Ts>
constexpr auto getInner(TupleStorage<Ts...> &t) noexcept ->
    typename TupleInnerElement<I, TupleStorage<Ts...>>::Type &;

template <std::size_t I, typename... Ts>
constexpr auto getInner(TupleStorage<Ts...> &&t) noexcept ->
    typename TupleInnerElement<I, TupleStorage<Ts...>>::Type &&;

template <std::size_t I, typename... Ts>
constexpr auto getInner(TupleStorage<Ts...> const &&t) noexcept ->
    typename TupleInnerElement<I, TupleStorage<Ts...>>::Type const &&;

template <typename... Ts> struct TupleTypeHelper;

template <typename U> struct TupleRefWrapper;

template <typename U>
using TupleInnerType =
    std::conditional_t<std::is_reference_v<U>, TupleRefWrapper<U>, U>;

} // namespace impl

// Implementation

namespace impl {
template <bool trivial_dtor, bool trivial_copy, typename... Ts>
struct TupleStorageImpl;

template <typename... Ts> struct TupleStorageImpl<true, true, Ts...> {
public:
  template <typename U> using InnerType = impl::TupleInnerType<U>;

  using Helper = impl::TupleTypeHelper<InnerType<Ts>...>;
  // Sizes and offsets

  using OffsetsType = typename Helper::OffsetsType;
  using SizesType = typename Helper::SizesType;
  using AlignmentsType = typename Helper::AlignmentsType;

  static constexpr std::size_t max_alignment =
      Helper::max_alignment == 0 ? 1 : Helper::max_alignment;
  static constexpr std::size_t total_size =
      (Helper::total_size == 0 ? 1 : Helper::total_size);

  constexpr TupleStorageImpl() noexcept = default;
  ~TupleStorageImpl() noexcept = default;
  constexpr TupleStorageImpl(TupleStorageImpl const &) noexcept = default;
  constexpr TupleStorageImpl(TupleStorageImpl &&) noexcept = default;

  constexpr TupleStorageImpl &
  operator=(TupleStorageImpl const &) noexcept = default;
  constexpr TupleStorageImpl &operator=(TupleStorageImpl &&) noexcept = default;

  alignas(max_alignment) unsigned char data_m[total_size] = {};
};

} // namespace impl

template <typename... Ts>
struct TupleStorage
    : impl::TupleStorageImpl<
          (std::is_trivially_destructible_v<impl::TupleInnerType<Ts>> && ...),
          (std::is_trivially_copyable_v<impl::TupleInnerType<Ts>> && ...),
          impl::TupleInnerType<Ts>...> {
public:
  template <typename U> using InnerType = impl::TupleInnerType<U>;

private:
  using Helper = impl::TupleTypeHelper<InnerType<Ts>...>;

  using impl::TupleStorageImpl<
      (std::is_trivially_destructible_v<impl::TupleInnerType<Ts>> && ...),
      (std::is_trivially_copyable_v<impl::TupleInnerType<Ts>> && ...),
      impl::TupleInnerType<Ts>...>::data_m;

public:
  // Noexcept specifiers
  static constexpr bool is_nothrow_default_ctor =
      (std::is_nothrow_default_constructible_v<InnerType<Ts>> && ...);
  static constexpr bool is_nothrow_copy_ctor =
      (std::is_nothrow_copy_constructible_v<InnerType<Ts>> && ...);
  static constexpr bool is_nothrow_move_ctor =
      (std::is_nothrow_move_constructible_v<InnerType<Ts>> && ...);
  template <typename... Args>
  static constexpr bool is_nothrow_ctor =
      (std::is_nothrow_constructible_v<InnerType<Ts>, Args> && ...);
  static constexpr bool is_nothrow_dtor =
      (std::is_nothrow_destructible_v<InnerType<Ts>> && ...);
  static constexpr auto sequence = std::index_sequence_for<Ts...>{};

  // Sizes and offsets

  using OffsetsType = typename Helper::OffsetsType;
  using SizesType = typename Helper::SizesType;
  using AlignmentsType = typename Helper::AlignmentsType;

  static constexpr std::size_t max_alignment =
      Helper::max_alignment == 0 ? 1 : Helper::max_alignment;
  static constexpr std::size_t total_size =
      (Helper::total_size == 0 ? 1 : Helper::total_size);

  // Friends
  template <std::size_t I, typename... Us>
  friend constexpr auto impl::getInner(TupleStorage<Us...> const &t) noexcept ->
      typename impl::TupleInnerElement<I, TupleStorage<Us...>>::Type const &;

  template <std::size_t I, typename... Us>
  friend constexpr auto impl::getInner(TupleStorage<Us...> &t) noexcept ->
      typename impl::TupleInnerElement<I, TupleStorage<Us...>>::Type &;

  template <std::size_t I, typename... Us>
  friend constexpr auto impl::getInner(TupleStorage<Us...> &&t) noexcept ->
      typename impl::TupleInnerElement<I, TupleStorage<Us...>>::Type &&;

  template <std::size_t I, typename... Us>
  friend constexpr auto impl::getInner(TupleStorage<Us...> const &&t) noexcept
      ->
      typename impl::TupleInnerElement<I, TupleStorage<Us...>>::Type const &&;

  constexpr TupleStorage() noexcept(is_nothrow_default_ctor) {
    defaultCtor(sequence);
  }

  template <bool requirement = sizeof...(Ts) >= 1 &&
                               (std::is_copy_constructible_v<InnerType<Ts>> &&
                                ...),
            std::enable_if_t<requirement, bool> = true>
  constexpr explicit TupleStorage(Ts const &... args) noexcept(
      is_nothrow_copy_ctor) {
    forwardCtor(sequence, args...);
  }

  template <typename... Us,
            bool requirement = (sizeof...(Us) == sizeof...(Ts) &&
                                sizeof...(Us) >= 1 &&
                                (std::is_constructible_v<Ts, Us &&> && ...)),
            std::enable_if_t<requirement, bool> = true>
  constexpr explicit TupleStorage(Us &&... args) noexcept(
      is_nothrow_ctor<Us...>) {
    forwardCtor(sequence, std::forward<Us>(args)...);
  }

  template <typename... Us,
            bool requirements =
                (sizeof...(Ts) == sizeof...(Us) &&
                 (sizeof...(Ts) != 1 ||
                  ((!std::is_convertible_v<TupleStorage<Us...> const &,
                                           InnerType<Ts>> &&
                    ...) &&
                   (!std::is_constructible_v<InnerType<Ts>,
                                             TupleStorage<Us...> const &> &&
                    ...) &&
                   (!std::is_same_v<Ts, Us> && ...)))),
            std::enable_if_t<requirements, bool> = true>
  constexpr explicit TupleStorage(TupleStorage<Us...> const &other) noexcept(
      is_nothrow_ctor<Us const &...>) {
    conversionCtor(sequence, other);
  }

  template <
      typename... Us,
      bool requirements =
          (sizeof...(Ts) == sizeof...(Us) &&
           (sizeof...(Ts) != 1 ||
            ((!std::is_convertible_v<TupleStorage<Us...>, InnerType<Ts>> &&
              ...) &&
             (!std::is_constructible_v<InnerType<Ts>, TupleStorage<Us...>> &&
              ...) &&
             (!std::is_same_v<Ts, Us> && ...)))),
      std::enable_if_t<requirements, bool> = true>
  constexpr explicit TupleStorage(TupleStorage<Us...> &&other) noexcept(
      is_nothrow_ctor<Us &&...>) {
    conversionCtor(sequence, std::move(other));
  }

  template <typename U1, typename U2,
            bool size_requirement = sizeof...(Ts) == 2,
            std::enable_if_t<size_requirement, bool> = true>
  constexpr explicit TupleStorage(Pair<U1, U2> const &p) noexcept(
      is_nothrow_ctor<U1 const &, U2 const &>) {
    conversionCtor(p);
  }

  template <typename U1, typename U2,
            bool size_requirement = sizeof...(Ts) == 2,
            std::enable_if_t<size_requirement, bool> = true>
  constexpr explicit TupleStorage(Pair<U1, U2> &&p) noexcept(
      is_nothrow_ctor<U1 &&, U2 &&>) {
    conversionCtor(std::move(p));
  }

  constexpr TupleStorage(TupleStorage const &other) noexcept(
      is_nothrow_copy_ctor) = default;
  constexpr TupleStorage(TupleStorage &&other) noexcept(is_nothrow_move_ctor) =
      default;
  constexpr TupleStorage &operator=(TupleStorage const &) noexcept = default;
  constexpr TupleStorage &operator=(TupleStorage &&) noexcept = default;

private:
  template <std::size_t I> auto unsafeAt() -> void * {
    return &data_m[Sequence::at_index_v<I, OffsetsType>];
  }

  template <std::size_t I>[[nodiscard]] auto unsafeAt() const -> const void * {
    return &data_m[Sequence::at_index_v<I, OffsetsType>];
  }

  template <std::size_t... indices>
  constexpr void destructor(
      std::integer_sequence<std::size_t,
                            indices...> /*unused*/) noexcept(is_nothrow_dtor) {
    auto dtor = [](auto &var) {
      using type = std::decay_t<decltype(var)>;
      var.~type();
    };
    (dtor(impl::getInner<indices>(*this)), ...);
  }

  template <std::size_t... indices>
  constexpr void
  defaultCtor(std::integer_sequence<
              std::size_t,
              indices...> /*unused*/) noexcept(is_nothrow_default_ctor) {
    (new (unsafeAt<indices>()) InnerType<Ts>(), ...);
  }

  template <typename... Us, std::size_t... indices>
  constexpr void copyCtor(
      std::index_sequence<indices...> /*unused*/,
      TupleStorage<Us...> const &rhs) noexcept(is_nothrow_ctor<Us const &...>) {
    (new (unsafeAt<indices>()) InnerType<Ts>(impl::getInner<indices>(rhs)),
     ...);
  }

  template <typename... Us, std::size_t... indices>
  constexpr void
  moveCtor(std::integer_sequence<std::size_t, indices...> /*unused*/,
           TupleStorage<Us...> &&rhs) noexcept(is_nothrow_ctor<Us &&...>) {
    (new (unsafeAt<indices>())
         InnerType<Ts>(std::forward<Us>(impl::getInner<indices>(rhs))),
     ...);
  }

  template <typename... Us, std::size_t... indices>
  constexpr void
  forwardCtor(std::integer_sequence<std::size_t, indices...> /*unused*/,
              Us &&... vals) noexcept(is_nothrow_ctor<Us &&...>) {
    (new (unsafeAt<indices>()) InnerType<Ts>(std::forward<Us>(vals)), ...);
  }

  template <typename... Us, std::size_t... indices>
  constexpr void
  conversionCtor(std::integer_sequence<std::size_t, indices...> /*unused*/,
                 TupleStorage<Us...> const
                     &other) noexcept(is_nothrow_ctor<Us const &...>) {
    (new (unsafeAt<indices>()) InnerType<Ts>(get<indices>(other)), ...);
  }

  template <typename... Us, std::size_t... indices>
  constexpr void conversionCtor(
      std::integer_sequence<std::size_t, indices...> /*unused*/,
      TupleStorage<Us...> &&other) noexcept(is_nothrow_ctor<Us &&...>) {
    (new (unsafeAt<indices>()) Ts(std::forward<Us>(get<indices>(other))), ...);
  }

  template <typename U1, typename U2>
  constexpr void conversionCtor(Pair<U1, U2> const &other) noexcept(
      is_nothrow_ctor<U1 const &, U2 const &>) {

    new (unsafeAt<0>()) impl::TupleInnerElementT<0, TupleStorage>(other.first);
    new (unsafeAt<1>()) impl::TupleInnerElementT<1, TupleStorage>(other.second);
  }

  template <typename U1, typename U2>
  constexpr void
  conversionCtor(Pair<U1, U2> &&other) noexcept(is_nothrow_ctor<U1 &&, U2 &&>) {
    new (unsafeAt<0>()) impl::TupleInnerElementT<0, TupleStorage>(
        std::forward<U1>(other.first));
    new (unsafeAt<1>()) impl::TupleInnerElementT<1, TupleStorage>(
        std::forward<U2>(other.second));
  }
};

template <std::size_t I, typename... Us>
constexpr auto get(const TupleStorage<Us...> &tuple) noexcept ->
    typename TupleElement<I, TupleStorage<Us...>>::Type const & {
  using type = utility::ElementType<I, Us...>;
  if constexpr (std::is_reference_v<type>) {
    return impl::getInner<I>(tuple).reference_m;
  } else {
    return impl::getInner<I>(tuple);
  }
}
template <std::size_t I, typename... Us>
constexpr auto get(TupleStorage<Us...> &tuple) noexcept ->
    typename TupleElement<I, TupleStorage<Us...>>::Type & {
  using type = utility::ElementType<I, Us...>;
  if constexpr (std::is_reference_v<type>) {
    return impl::getInner<I>(tuple).reference_m;
  } else {
    return impl::getInner<I>(tuple);
  }
}

template <std::size_t I, typename... Us>
constexpr auto get(TupleStorage<Us...> &&tuple) noexcept ->
    typename TupleElement<I, TupleStorage<Us...>>::Type && {
  using type = utility::ElementType<I, Us...>;
  if constexpr (std::is_reference_v<type>) {
    return impl::getInner<I>(tuple).reference_m;
  } else {
    return std::move(impl::getInner<I>(tuple));
  }
}

template <std::size_t I, typename... Us>
constexpr auto get(TupleStorage<Us...> const &&tuple) noexcept ->
    typename TupleElement<I, TupleStorage<Us...>>::Type const && {
  using type = utility::ElementType<I, Us...>;
  if constexpr (std::is_reference_v<type>) {
    return impl::getInner<I>(tuple).reference_m;
  } else {
    return std::move(impl::getInner<I>(tuple));
  }
}

namespace impl {

template <std::size_t I, typename... Ts>
struct TupleInnerElement<I, TupleStorage<Ts...>> {
  using Type = TupleInnerType<utility::ElementType<I, Ts...>>;
};

template <std::size_t I, typename T> struct TupleInnerElement;

template <std::size_t I, typename... Ts>
constexpr auto getInner(TupleStorage<Ts...> const &t) noexcept ->
    typename TupleInnerElement<I, TupleStorage<Ts...>>::Type const & {
  using type = typename TupleInnerElement<I, TupleStorage<Ts...>>::Type;
  return *static_cast<type const *>(t.template unsafeAt<I>());
}

template <std::size_t I, typename... Ts>
constexpr auto getInner(TupleStorage<Ts...> &t) noexcept ->
    typename TupleInnerElement<I, TupleStorage<Ts...>>::Type & {
  using type = typename TupleInnerElement<I, TupleStorage<Ts...>>::Type;
  return *static_cast<type *>(t.template unsafeAt<I>());
}

template <std::size_t I, typename... Ts>
constexpr auto getInner(TupleStorage<Ts...> &&t) noexcept ->
    typename TupleInnerElement<I, TupleStorage<Ts...>>::Type && {
  using type = typename TupleInnerElement<I, TupleStorage<Ts...>>::Type;
  return std::move(*static_cast<type *>(t.template unsafeAt<I>()));
}

template <std::size_t I, typename... Ts>
constexpr auto getInner(TupleStorage<Ts...> const &&t) noexcept ->
    typename TupleInnerElement<I, TupleStorage<Ts...>>::Type const && {
  using type = typename TupleInnerElement<I, TupleStorage<Ts...>>::Type;
  return std::move(*static_cast<type const *>(t.template unsafeAt<I>()));
}

template <std::size_t Offset, std::size_t Size, std::size_t Alignment>
struct TupleTypeInfo {
  static constexpr std::size_t offset = Offset;
  static constexpr std::size_t size = Size;
  static constexpr std::size_t alignment = Alignment;
};

template <typename... TypeInfos> struct TupleTypeInfoSequence;

template <> struct TupleTypeInfoSequence<> {
  using OffsetsType = std::index_sequence<>;
  using SizesType = std::index_sequence<>;
  using AlignmentsType = std::index_sequence<>;

  static constexpr std::size_t max_alignment = 0;
  static constexpr std::size_t total_size = 0;
};

template <std::size_t... Offsets, std::size_t... Sizes,
          std::size_t... Alignments>
struct TupleTypeInfoSequence<TupleTypeInfo<Offsets, Sizes, Alignments>...> {
  using OffsetsType = std::index_sequence<Offsets...>;
  using SizesType = std::index_sequence<Sizes...>;
  using AlignmentsType = std::index_sequence<Alignments...>;

  static constexpr std::size_t max_alignment = std::max({Alignments...});
  // \todo Think about manually adding padding bytes to the end
  static constexpr std::size_t total_size =
      Sequence::last_v<OffsetsType> + Sequence::last_v<SizesType>;
};

template <std::size_t RhsOffset, std::size_t RhsSize, std::size_t RhsAlignment>
constexpr auto
operator|(TupleTypeInfoSequence<> /*unused*/,
          TupleTypeInfo<RhsOffset, RhsSize, RhsAlignment> /*unused*/) noexcept {

  return TupleTypeInfoSequence<TupleTypeInfo<0, RhsSize, RhsAlignment>>{};
}

template <typename... Head, std::size_t RhsOffset, std::size_t RhsSize,
          std::size_t RhsAlignment>
constexpr auto
operator|(TupleTypeInfoSequence<Head...> seq,
          TupleTypeInfo<RhsOffset, RhsSize, RhsAlignment> /*unused*/) noexcept {

  constexpr auto LhsOffset =
      Sequence::last_v<typename decltype(seq)::OffsetsType>;
  constexpr auto LhsSize = Sequence::last_v<typename decltype(seq)::SizesType>;

  constexpr std::size_t rhs_start = LhsOffset + LhsSize;
  constexpr std::size_t new_rhs_offset =
      (rhs_start % RhsAlignment == 0
           ? rhs_start
           : (rhs_start / RhsAlignment + 1) * RhsAlignment);

  return TupleTypeInfoSequence<
      Head..., TupleTypeInfo<new_rhs_offset, RhsSize, RhsAlignment>>{};
}

template <typename... Ts>
using TupleTypeInfoSequenceT =
    decltype((TupleTypeInfoSequence<>{} | ... |
              TupleTypeInfo<0, sizeof(Ts), alignof(Ts)>{}));

template <typename... Ts> struct TupleTypeHelper {
private:
  using TypeInfoSequence = impl::TupleTypeInfoSequenceT<Ts...>;

public:
  using OffsetsType = typename TypeInfoSequence::OffsetsType;
  using SizesType = typename TypeInfoSequence::SizesType;
  using AlignmentsType = typename TypeInfoSequence::AlignmentsType;

  static constexpr auto offsets_value = OffsetsType{};
  static constexpr auto sizes_value = SizesType{};
  static constexpr auto alignments_value = AlignmentsType{};

  static constexpr std::size_t max_alignment = TypeInfoSequence::max_alignment;
  static constexpr std::size_t total_size = TypeInfoSequence::total_size;
};

//! \attention For internal usage only!
//! \brief Wrapper for reference types
//! (we can't construct references with placement new)
template <typename U> struct TupleRefWrapper {
  static_assert(std::is_reference_v<U>, "U should be a reference");
  constexpr explicit TupleRefWrapper(U ref) noexcept
      : reference_m(std::forward<U>(ref)) {}
  constexpr TupleRefWrapper(TupleRefWrapper const &) noexcept = default;
  constexpr TupleRefWrapper(TupleRefWrapper &&) noexcept = default;
  U reference_m;
};

} // namespace impl
} // namespace CxxPlugins
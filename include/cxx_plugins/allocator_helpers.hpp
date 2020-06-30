/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    allocator_helpers.hpp
 * \author  Andrey Ponomarev
 * \date    26 Jun 2020
 * \brief
 * $BRIEF$
 */
#pragma once
#include <tuple>
#include <type_traits>

namespace CxxPlugins {

namespace impl {
template <typename T> struct IsPair : public std::false_type{};
template <typename First, typename Second>
struct IsPair<std::pair<First, Second>> : public std::true_type {};
template <typename T> static constexpr bool is_pair_v = IsPair<T>::value;
} // namespace impl

template <class T, class... Args>
constexpr auto constructAt(T *p, Args &&... args) -> T * {
  return ::new (const_cast<void *>(static_cast<const volatile void *>(p)))
      T(std::forward<Args>(args)...);
}

template <class T, class Alloc, class... Args,
          typename = std::enable_if_t<!impl::is_pair_v<T>>>
constexpr auto usesAllocatorConstructionArgs(Alloc const &alloc,
                                             Args &&... args) noexcept {
  if constexpr (!std::uses_allocator_v<T, Alloc> &&
                std::is_constructible_v<T, Args...>) {
    return std::forward_as_tuple(std::forward<Args>(args)...);
  } else if constexpr (std::uses_allocator_v<T, Alloc> &&
                       std::is_constructible_v<T, std::allocator_arg_t,
                                               Alloc const &, Args...>) {
    return std::tuple<std::allocator_arg_t, Alloc const &, Args &&...>(
        std::allocator_arg, alloc, std::forward<Args>(args)...);
  } else if constexpr (std::uses_allocator_v<T, Alloc> &&
                       std::is_constructible_v<T, Args..., Alloc const &>) {
    return std::forward_as_tuple(std::forward<Args>(args)..., alloc);
  } else {
    static_assert(sizeof(T) == 0 && sizeof(Alloc) == 0,
                  "Program is ill-formed");
  }
}

namespace impl {
// Specializations for pair
template <class T1, class T2, class Alloc, typename Tuple1, typename Tuple2>
constexpr auto usesAllocatorConstructionArgs(
    std::in_place_type_t<std::pair<T1, T2>> /*unused*/, const Alloc &alloc,
    std::piecewise_construct_t /*unused*/, Tuple1 &&x, Tuple2 &&y) noexcept {
  return std::make_tuple(
      std::piecewise_construct,
      std::apply(
          [&alloc](auto &&... args1) {
            return CxxPlugins::usesAllocatorConstructionArgs<T1>(
                alloc, std::forward<decltype(args1)>(args1)...);
          },
          std::forward<Tuple1>(x)),
      std::apply(
          [&alloc](auto &&... args2) {
            return CxxPlugins::usesAllocatorConstructionArgs<T2>(
                alloc, std::forward<decltype(args2)>(args2)...);
          },
          std::forward<Tuple2>(y)));
}

} // namespace impl

template <class T, class Alloc, typename Tuple1, typename Tuple2,
          typename = std::enable_if_t<impl::is_pair_v<T>>>
constexpr auto
usesAllocatorConstructionArgs(const Alloc &alloc,
                              std::piecewise_construct_t /*unused*/, Tuple1 &&x,
                              Tuple2 &&y) noexcept {
  return impl::usesAllocatorConstructionArgs(
      std::in_place_type<T>, alloc, std::piecewise_construct,
      std::forward<Tuple1>(x), std::forward<Tuple2>(y));
}

template <class T, class Alloc, typename = std::enable_if_t<impl::is_pair_v<T>>>
constexpr auto usesAllocatorConstructionArgs(const Alloc &alloc) noexcept {
  return CxxPlugins::usesAllocatorConstructionArgs<T>(
      alloc, std::piecewise_construct, std::tuple<>{}, std::tuple<>{});
}

template <class T, class Alloc, class U, class V,
          typename = std::enable_if_t<impl::is_pair_v<T>>>
constexpr auto usesAllocatorConstructionArgs(const Alloc &alloc, U &&u,
                                             V &&v) noexcept {
  return CxxPlugins::usesAllocatorConstructionArgs<T>(
      alloc, std::piecewise_construct,
      std::forward_as_tuple(std::forward<U>(u)),
      std::forward_as_tuple(std::forward<V>(v)));
}

template <class T, class Alloc, class U, class V,
          typename = std::enable_if_t<impl::is_pair_v<T>>>
constexpr auto
usesAllocatorConstructionArgs(const Alloc &alloc,
                              const std::pair<U, V> &pr) noexcept {
  return CxxPlugins::usesAllocatorConstructionArgs<T>(
      alloc, std::piecewise_construct, std::forward_as_tuple(pr.first),
      std::forward_as_tuple(pr.second));
}

template <class T, class Alloc, class U, class V,
          typename = std::enable_if_t<impl::is_pair_v<T>>>
constexpr auto usesAllocatorConstructionArgs(const Alloc &alloc,
                                             std::pair<U, V> &&pr) noexcept {
  return CxxPlugins::usesAllocatorConstructionArgs<T>(
      alloc, std::piecewise_construct,
      std::forward_as_tuple(std::move(pr).first),
      std::forward_as_tuple(std::move(pr).second));
}

template <class T, class Alloc, class... Args>
constexpr auto uninitializedConstructUsingAllocator(T *p, const Alloc &alloc,
                                                    Args &&... args) -> T * {

  return std::apply(
      [&](auto &&... xs) {
        return CxxPlugins::constructAt(p, std::forward<decltype(xs)>(xs)...);
      },
      CxxPlugins::usesAllocatorConstructionArgs<T>(
          alloc, std::forward<Args>(args)...));
}
} // namespace CxxPlugins
/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    functionref.hpp
 * \author  Andrey Ponomarev
 * \date    07 Jul 2020
 * \brief
 * $BRIEF$
 */
#pragma once

#include "cxx_plugins/function_traits.hpp"
#include "cxx_plugins/variant.hpp"

#include <functional>

namespace plugins {
template <typename Signature> struct FunctionRef;

namespace impl {

template <typename Signature, typename Enabler = void>
struct FunctionRefVariant;

template <typename Signature> struct FunctionObjectRef;

template <typename Return, typename... Args>
struct FunctionObjectRef<Return(Args...)> {
private:
  template <typename Callable,
            typename = std::enable_if_t<std::is_class_v<Callable>>>
  auto kickFn(void const *obj_p, Args... args) -> Return {
    return (*static_cast<Callable const *>(obj_p))(
        std::forward<decltype(args)>(args)...);
  }

public:
  void const *obj_p_m = nullptr;
  utility::FunctionPointer<Return(void *, Args...)> kick_starter_m = nullptr;

  constexpr FunctionObjectRef() noexcept = default;

  template <typename Callable>
  FunctionObjectRef(Callable const &obj)
      : obj_p_m{&obj}, kick_starter_m{&kickFn<Callable>} {}

  constexpr Return operator()(Args... args) const {
    return kick_starter_m(obj_p_m, std::forward<decltype(args)>(args)...);
  }
};

template <typename Signature>
constexpr auto operator==(FunctionRef<Signature> const &lhs,
                          FunctionRef<Signature> const &rhs) -> bool {
  return lhs.obj_p_m == rhs.obj_p_m;
}
template <typename Signature>
constexpr auto operator!=(FunctionRef<Signature> const &lhs,
                          FunctionRef<Signature> const &rhs) -> bool {
  return !(lhs == rhs);
}

template <typename Return> struct FunctionRefVariant<Return(), void> {
  using Type = Variant<Monostate, utility::FunctionPointer<Return()>,
                       FunctionObjectRef<Return()>>;
};

template <typename Return, typename ClassArg, typename... RestArgs>
struct FunctionRefVariant<
    Return(ClassArg, RestArgs...),
    std::enable_if_t<std::is_object_v<std::decay_t<ClassArg>> &&
                     std::is_reference_v<ClassArg>>> {
  using Type = Variant<
      Monostate, utility::FunctionPointer<Return(ClassArg, RestArgs...)>,
      FunctionObjectRef<Return(ClassArg, RestArgs...)>,
      std::conditional_t<std::is_const_v<std::remove_reference_t<ClassArg>>,
                         utility::FunctionPointer<Return(RestArgs...) const, std::remove_reference_t<ClassArg>>,
                         utility::FunctionPointer<Return(RestArgs...), std::remove_reference_t<ClassArg>>>>;
};

template <typename Return, typename ClassArg, typename... RestArgs>
struct FunctionRefVariant<
    Return(ClassArg, RestArgs...),
    std::enable_if_t<!std::is_object_v<std::decay_t<ClassArg>> ||
                     !std::is_reference_v<ClassArg>>> {
  using Type = Variant<Monostate,
                       utility::FunctionPointer<Return(ClassArg, RestArgs...)>,
                       FunctionObjectRef<Return(ClassArg, RestArgs...)>>;
};

template <typename Signature>
using FunctionRefVariantT = typename FunctionRefVariant<Signature, void>::Type;

} // namespace impl

template <typename Return, typename... Args>
struct FunctionRef<Return(Args...)> {
public:
  using Signature = Return(Args...);

  constexpr FunctionRef() noexcept = default;
  constexpr FunctionRef(FunctionRef const &) noexcept = default;
  constexpr FunctionRef(FunctionRef &&) noexcept = default;
  constexpr auto operator=(FunctionRef const &) noexcept
      -> FunctionRef & = default;
  constexpr auto operator=(FunctionRef &&) noexcept -> FunctionRef & = default;

  template <typename T, typename = std::enable_if_t<
                            !std::is_same_v<std::decay_t<T>, FunctionRef> &&
                            !std::is_same_v<std::decay_t<T>, std::nullptr_t>>>
  constexpr FunctionRef(T const &fn) noexcept : fn_m(fn) {}

  template <typename T, typename = std::enable_if_t<
                            !std::is_same_v<std::decay_t<T>, FunctionRef> &&
                            !std::is_same_v<std::decay_t<T>, std::nullptr_t>>>
  constexpr FunctionRef &operator=(T const &fn) noexcept {
    fn_m = fn;
    return *this;
  }

  constexpr FunctionRef(std::nullptr_t) noexcept {}

  constexpr FunctionRef &operator=(std::nullptr_t) noexcept {
    fn_m = Monostate{};
    return *this;
  }

  constexpr auto operator()(Args... args) -> Return {
    return std::visit([&](auto &fn) {
      return std::invoke(fn, std::forward<decltype(args)>(args)...);
    });
  }

  void swap(FunctionRef &other) noexcept {
    using std::swap;
    swap(fn_m, other.fn_m);
  }

  constexpr auto operator==(FunctionRef const &rhs) const noexcept -> bool {
    return fn_m == rhs.fn_m;
  }
  constexpr auto operator!=(FunctionRef const &rhs) const noexcept -> bool {
    return !(*this == rhs);
  }

  constexpr explicit operator bool() const noexcept {
    return holdsAlternative<Monostate>(fn_m);
  }

private:
  impl::FunctionRefVariantT<Return(Args...)> fn_m;
};

template <typename Signature>
void swap(FunctionRef<Signature> &lhs, FunctionRef<Signature> &rhs) {
  lhs.swap(rhs);
}

} // namespace CxxPlugins

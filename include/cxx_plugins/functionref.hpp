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
 * Contains implementation for FunctionRef class.
 */
#pragma once

#include "cxx_plugins/function_cast.hpp"
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
  static auto kickFn(void const *obj_p, Args... args) -> Return {
    return (*static_cast<Callable const *>(obj_p))(
        std::forward<decltype(args)>(args)...);
  }

public:
  void const *obj_p_m = nullptr;
  FnPtr<Return(void const *, Args...)> kick_starter_m = nullptr;

  constexpr FunctionObjectRef() noexcept = default;

  template <typename Callable>
  FunctionObjectRef(Callable const *obj)
      : obj_p_m{obj}, kick_starter_m{&kickFn<Callable>} {}

  constexpr Return operator()(Args... args) const {
    return kick_starter_m(obj_p_m, std::forward<decltype(args)>(args)...);
  }
  constexpr bool operator==(FunctionObjectRef const &rhs) const noexcept {
    return obj_p_m == rhs.obj_p_m;
  }
  constexpr bool operator!=(FunctionObjectRef const &rhs) const noexcept {
    return !(*this == rhs);
  }
  constexpr bool operator<(FunctionObjectRef const &rhs) const noexcept {
    return obj_p_m < rhs.obj_p_m;
  }
  constexpr bool operator<=(FunctionObjectRef const &rhs) const noexcept {
    return !(rhs < *this);
  }
  constexpr bool operator>(FunctionObjectRef const& rhs) const noexcept {
    return rhs < *this;
  }
  constexpr bool operator>=(FunctionObjectRef const& rhs) const noexcept {
    return !(*this < rhs);
  }
};

template <typename Return> struct FunctionRefVariant<Return(), void> {
  using Type = Variant<Monostate, FnPtr<Return()>, FunctionObjectRef<Return()>>;
};

template <typename Return, typename ClassArg, typename... RestArgs>
struct FunctionRefVariant<
    Return(ClassArg, RestArgs...),
    std::enable_if_t<std::is_object_v<std::decay_t<ClassArg>> &&
                     std::is_reference_v<ClassArg>>> {
  using Type = Variant<
      Monostate, FnPtr<Return(ClassArg, RestArgs...)>,
      FunctionObjectRef<Return(ClassArg, RestArgs...)>,
      std::conditional_t<
          std::is_const_v<std::remove_reference_t<ClassArg>>,
          FnPtr<Return(RestArgs...) const, std::remove_reference_t<ClassArg>>,
          FnPtr<Return(RestArgs...), std::remove_reference_t<ClassArg>>>>;
};

template <typename Return, typename ClassArg, typename... RestArgs>
struct FunctionRefVariant<
    Return(ClassArg, RestArgs...),
    std::enable_if_t<!std::is_object_v<std::decay_t<ClassArg>> ||
                     !std::is_reference_v<ClassArg>>> {
  using Type = Variant<Monostate, FnPtr<Return(ClassArg, RestArgs...)>,
                       FunctionObjectRef<Return(ClassArg, RestArgs...)>>;
};

template <typename Signature>
using FunctionRefVariantT = typename FunctionRefVariant<Signature, void>::Type;

} // namespace impl

/*!
 * \brief A FunctionRef template class surves the same purpose as std::function.
 * Unlike std::function it doesn't own the object underneath.
 * Which means that if FunctionRef outlives the object it can lead to undefined
 * behavior.
 */
#ifdef DOXYGEN
template <typename Signature> struct FunctionRef<Signature> {
#else
template <typename Return, typename... Args>
struct FunctionRef<Return(Args...)> {
#endif
public:
  using Signature = Return(Args...);

  constexpr FunctionRef() noexcept = default;
  constexpr FunctionRef(FunctionRef const &) noexcept = default;
  constexpr FunctionRef(FunctionRef &&) noexcept = default;
  constexpr auto operator=(FunctionRef const &) noexcept
      -> FunctionRef & = default;
  constexpr auto operator=(FunctionRef &&) noexcept -> FunctionRef & = default;

  /*!
   * \brief Main constructor of FunctionRef.
   * Allows construction from pointer to object or function pointer
   */
#ifdef DOXYGEN
  template<typename T>
#else
  template <typename T, typename = std::enable_if_t<
                            !std::is_same_v<std::decay_t<T>, FunctionRef> &&
                            !std::is_same_v<std::decay_t<T>, std::nullptr_t>>>
#endif
  constexpr FunctionRef(T const *fn) noexcept : fn_m(fn) {
    if (fn == nullptr)
      reset();
  }

  template<typename T, typename = std::enable_if_t<traits::is_any_function_pointer_v<T>>>
  constexpr FunctionRef(T fn) noexcept : fn_m(fn) {
    if (fn == nullptr)
      reset();
  }


  /*!
   * \brief Main assignment operator of FunctionRef.
   * Allows assignment from pointer to object or function pointer
   */
#ifdef DOXYGEN
  template <typename T>
#else
  template <typename T, typename = std::enable_if_t<
                            !std::is_same_v<std::decay_t<T>, FunctionRef> &&
                            !std::is_same_v<std::decay_t<T>, std::nullptr_t>>>
#endif
  constexpr FunctionRef &operator=(T const *fn) noexcept {
    if (fn == nullptr)
      return *this = nullptr;
    fn_m = fn;
    return *this;
  }

  template<typename T, typename = std::enable_if_t<traits::is_any_function_pointer_v<T>>>
  constexpr FunctionRef &operator=(T fn) noexcept {
    if (fn == nullptr)
      return *this = nullptr;
    fn_m = fn;
    return *this;
  }

  /*!
   * \brief Construction from null pointer
   */
  constexpr FunctionRef(std::nullptr_t /*unused*/) noexcept {}

  /*!
   * \brief Assignment of null pointer
   */
  constexpr FunctionRef &operator=(std::nullptr_t) noexcept {
    reset();
    return *this;
  }

  /*!
   * \brief Resets the value of function ref
   */
  constexpr void reset() noexcept{
    fn_m = Monostate {};
  }

  /*!
   * \brief Check whenever FunctionRef has value or not
   * \return
   */
  constexpr bool isEmpty() const noexcept{
    return holdsAlternative<Monostate>(fn_m);
  }

  /*!
   * \brief Call operator overload
   */
  constexpr auto operator()(Args... args) -> Return {
    return std::visit([&](auto &fn) {
      if constexpr (std::is_same_v<std::decay_t<decltype(fn)>, Monostate>) {
        throw std::logic_error("Trying to invoke empty FunctionRef");
      } else {
        return std::invoke(fn, std::forward<decltype(args)>(args)...);
      }
    }, fn_m);
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
  constexpr bool operator<(FunctionRef const &rhs) const noexcept {
    return fn_m < rhs.obj_p_m;
  }
  constexpr bool operator<=(FunctionRef const &rhs) const noexcept {
    return !(rhs < *this);
  }
  constexpr bool operator>(FunctionRef const& rhs) const noexcept {
    return rhs < *this;
  }
  constexpr bool operator>=(FunctionRef const& rhs) const noexcept {
    return !(*this < rhs);
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

} // namespace plugins

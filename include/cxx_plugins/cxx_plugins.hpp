/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    cxx_plugins.hpp
 * \author  Andrey Ponomarev
 * \date    15 May 2020
 * \brief
 *
 */
/*!
 * \dir cxx_plugins
 * \brief Contains public interface for CXX Plugins
 */
#pragma once

#include "cxx_plugins/vtable.hpp"
#include "tuple/tuple_map.hpp"

#include <any>
#include <string>

//! \brief Main namespace for CXX Plugins project
namespace CxxPlugins {

enum class VTableStorage { SharedPointer, Member };

enum class Requirement { Required, Optional };

template <typename APIElementType,
          Requirement requirement_v = Requirement::Required>
struct APIElement {
  static constexpr Requirement requirement = requirement_v;
  APIElementType element_m;
};

template <typename Signature> struct GlobalFunction;

template <typename Return, typename... Args>
struct GlobalFunction<Return(Args...)> {
  constexpr explicit GlobalFunction(Return (*fn_p)(Args...)) noexcept
      : fn_m(fn_p) {}
  Return (*fn_m)(Args...) = nullptr;
};

template <typename Return, typename... Args>
GlobalFunction(Return (*)(Args...)) -> GlobalFunction<Return(Args...)>;

template <typename Class, typename Creators, typename Destroyers>
struct RegularClass {
  static_assert(std::is_class_v<Class>,
                "Class template parameter should be a class");
  template <typename T, typename U,
            typename = std::enable_if_t<!std::is_same_v<
                std::decay_t<T>,
                RegularClass>>> // disabling hiding move/copy constructors
  constexpr explicit RegularClass(T &&creators, U &&destroyers = U())
      : creators_m(std::forward<T>(creators)),
        destroyers_m(std::forward<U>(destroyers)) {}

  Creators creators_m;
  Destroyers destroyers_m;
};

template <typename Class, typename Creators, typename Destroyers>
auto makeRegularClass(Creators &&creators, Destroyers &&destroyers) {
  return RegularClass<Class, std::decay_t<Creators>, std::decay_t<Destroyers>>(
      std::forward<Creators>(creators), std::forward<Destroyers>(destroyers));
}

template <typename Functions, typename Classes, typename Polymorphics>
struct Plugin {
private:
public:
  Functions functions_m;
  Classes classes_m;
  Polymorphics polymorphics_m;
};

template <typename... APIElements> class PluginAPIBuilder {
public:
  template <typename Tag, Requirement requirement_v = Requirement::Required,
            typename FunctionT>
  auto addGlobalFunction(FunctionT &&default_value) {}

  template <typename Tag, Requirement requirement_v = Requirement::Required,
            typename Class>
  auto addClass() {}

  template <typename Tag, Requirement requirement_v = Requirement::Required,
            typename VTableT,
            VTableStorage storage = VTableStorage::SharedPointer,
            size_t stackReserve = sizeof(void *)>
  auto addPolymorphic() {}

private:
  Tuple<APIElements...> elements_;
};
} // namespace CxxPlugins
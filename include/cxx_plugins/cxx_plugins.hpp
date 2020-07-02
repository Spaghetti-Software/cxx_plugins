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

#include "cxx_plugins/function_traits.hpp"
#include "tuple/tuple_map.hpp"

#include <fmt/format.h>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/rapidjson.h>

#include <filesystem>
#include <fstream>

//! \brief Main namespace for CXX Plugins project
namespace CxxPlugins {

template <typename Signature, bool Required> struct GlobalFunction;

template <typename Return, typename... Args, bool Required>
struct GlobalFunction<Return(Args...), Required> {
  static constexpr bool is_required = Required;
  using Signature = Return(Args...);

  constexpr GlobalFunction() noexcept = default;
  constexpr GlobalFunction(Return (*fn_p)(Args...)) noexcept
      : function_m(fn_p) {}

  Return (*function_m)(Args...) = nullptr;
};



template <typename... TaggedValues> class Plugin {

  template <typename TagT, typename Signature, bool Required = false>
  constexpr Plugin<TaggedValues...,
                   TaggedValue<TagT, GlobalFunction<Signature, Required>>>
  addGlobalFunction() noexcept {
    return tupleCat(
        elements_m,
        TupleMap(TaggedValue{TagT(), GlobalFunction<Signature, Required>{}}));
  };

  void load(const char *plugin_config_file);

private:
  TupleMap<TaggedValues...> elements_m;
};

namespace impl {
[[noreturn]] void reportError(const char *message) {
  throw std::runtime_error(message);
}
[[noreturn]] void reportError(std::string const &message) {
  throw std::runtime_error(message);
}

} // namespace impl

struct GeneralPluginInfo {
  Version version_m;
  std::string name_m;
};

template <typename... TaggedValues>
void Plugin<TaggedValues...>::load(char const *plugin_config_file_p) {
  namespace fs = std::filesystem;

  if (!fs::exists(plugin_config_file_p)) {
    impl::reportError(
        fmt::format("Plugin config file '{}' not found.", plugin_config_file_p));
    return;
  }

  std::ifstream file(plugin_config_file_p);
  if (!file.is_open()) {
    impl::reportError(
        fmt::format("Can't open config file '{}'.", plugin_config_file_p));
    return;
  }

  std::string file_content;

  file.seekg(0, std::ios::end);
  file_content.reserve(file.tellg());
  file.seekg(0, std::ios::beg);

  file_content.assign(std::istreambuf_iterator<char>{file},
                      std::istreambuf_iterator<char>{});
  file.close();

  rapidjson::Document doc;
  doc.Parse(file_content.c_str(), file_content.size());

  if (doc.HasParseError()) {
    impl::reportError(
        fmt::format("Error occurred during parsing of '{}': {}",
                    plugin_config_file_p,
                    rapidjson::GetParseError_En(doc.GetParseError())));
    return;
  }




}

} // namespace CxxPlugins
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
#include "cxx_plugins/parser.hpp"
#include "polymorphic_ref.hpp"
#include "tuple/tuple_map.hpp"

#include <fmt/format.h>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/rapidjson.h>

#include <filesystem>
#include <fstream>

namespace plugin_info {
struct api_id {};
struct api_version {};
struct name {};
struct version {};
struct library_path {};
struct elements {};

class ValidationError : public std::runtime_error {
  using std::runtime_error::runtime_error;
};
class FileMissing : public ValidationError {
  using ValidationError::ValidationError;
};
class FileTypeMismatch : public ValidationError {
  using ValidationError::ValidationError;
};

template <typename TagType, typename ValueType, typename PluginType>
void adjust(TagType /*unused*/, [[maybe_unused]] ValueType &value,
            [[maybe_unused]] PluginType const &plugin) {}

template <typename TagType, typename ValueType, typename PluginType>
void validate(TagType /*unused*/, [[maybe_unused]] ValueType const &value,
              [[maybe_unused]] PluginType const &plugin) {}

template <typename PluginType>
void validate(library_path /*unused*/, std::filesystem::path const &lib_path,
              [[maybe_unused]] PluginType const &plugin) {
  namespace fs = std::filesystem;
  if (!fs::exists(lib_path)) {
    throw FileMissing(
        fmt::format("Library path '{}' is not valid.", lib_path.c_str()));
  }

  if (!fs::is_regular_file(lib_path)) {
    throw FileTypeMismatch(
        fmt::format("Library path '{}' doesn't represent a regular file.",
                    lib_path.c_str()));
  }
}

template <typename... Elements, typename... Tags, typename... ElementValues,
          typename PluginType>
void validate(
    elements /*unused*/,
    CxxPlugins::TupleMap<CxxPlugins::TaggedValue<Tags, ElementValues>...> const
        &map_of_elements,
    PluginType const &plugin) {

  auto tags = CxxPlugins::makeTuple(Tags{}...);
  CxxPlugins::tupleForEach(
      [&plugin](auto const &tag, auto const &val) {
        validate(tag, val, plugin);
      },
      tags, map_of_elements);
}

template <typename... Elements, typename... Tags, typename... ElementValues,
          typename PluginType>
void adjust(
    elements /*unused*/,
    CxxPlugins::TupleMap<CxxPlugins::TaggedValue<Tags, ElementValues>...>
        &map_of_elements,
    PluginType const &plugin) {

  auto tags = CxxPlugins::makeTuple(Tags{}...);
  CxxPlugins::tupleForEach(
      [&plugin](auto const &tag, auto &val) { adjust(tag, val, plugin); }, tags,
      map_of_elements);
}

} // namespace plugin_info

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

using VersionInfo = Tuple<int, int, int>;

template <typename PluginInfoMap, typename... TaggedElements>
class GenericPlugin;

using PluginDefaultInfo =
    TupleMap<TaggedValue<plugin_info::api_id, std::string>,
             TaggedValue<plugin_info::api_version, VersionInfo>,
             TaggedValue<plugin_info::name, std::optional<std::string>>,
             TaggedValue<plugin_info::version, std::optional<VersionInfo>>,
             TaggedValue<plugin_info::library_path, std::filesystem::path>>;

template <typename... TaggedElements>
using Plugin = GenericPlugin<PluginDefaultInfo, TaggedElements...>;

template <typename... InfoTags, typename... InfoValues,
          typename... TaggedElements>
class GenericPlugin<TupleMap<TaggedValue<InfoTags, InfoValues>...>,
                    TaggedElements...> {
public:
  using PluginFullInfo =
      TupleMap<TaggedValue<InfoTags, InfoValues>...,
               TaggedValue<plugin_info::elements, TupleMap<TaggedElements...>>>;

  //  template <typename TagT, typename Signature, bool Required = false>
  //  constexpr Plugin<TaggedValues...,
  //                   TaggedValue<TagT, GlobalFunction<Signature, Required>>>
  //  addGlobalFunction() noexcept {
  //    return tupleCat(
  //        elements_m,
  //        TupleMap(TaggedValue{TagT(), GlobalFunction<Signature,
  //        Required>{}}));
  //  };

  inline void loadFromFile(char const *plugin_config_file_p);
  inline void loadFromString(char const *plugin_configuration_p);

  PluginFullInfo const &getInfo() const { return data_m; }

private:
  PluginFullInfo data_m;
};

namespace impl {
[[noreturn]] void reportError(const char *message) {
  throw JSON::ParsingError(message);
}
[[noreturn]] void reportError(std::string const &message) {
  throw JSON::ParsingError(message);
}

} // namespace impl

template <typename... InfoTags, typename... InfoValues,
          typename... TaggedElements>
void GenericPlugin<TupleMap<TaggedValue<InfoTags, InfoValues>...>,
                   TaggedElements...>::loadFromFile(const char
                                                        *plugin_config_file_p) {

  namespace fs = std::filesystem;

  if (plugin_config_file_p == nullptr) {
    cxxPluginsUnreachable(
        "C string provided into GenericPlugin::loadFromFile points to null.");
  }

  if (!fs::exists(plugin_config_file_p)) {
    impl::reportError(fmt::format("Plugin config file '{}' not found.",
                                  plugin_config_file_p));
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

  loadFromString(file_content.c_str());
}

template <typename... InfoTags, typename... InfoValues,
          typename... TaggedElements>
void GenericPlugin<
    TupleMap<TaggedValue<InfoTags, InfoValues>...>,
    TaggedElements...>::loadFromString(const char *plugin_configuration_p) {

  if (plugin_configuration_p == nullptr) {
    cxxPluginsUnreachable(
        "C string provided into GenericPlugin::loadFromString points to null.");
  }

  rapidjson::Document doc;
  doc.Parse(plugin_configuration_p);

  if (doc.HasParseError()) {
    impl::reportError(fmt::format(
        "Error occurred during parsing of '{}': {}", plugin_configuration_p,
        rapidjson::GetParseError_En(doc.GetParseError())));
    return;
  }

  JSON::deserializeValue(doc, data_m);
  auto tags = makeTuple(InfoTags{}..., plugin_info::elements{});
  tupleForEach(
      [this](auto const &tag, auto const &val) { validate(tag, val, *this); },
      tags, data_m);
  tupleForEach([this](auto const &tag, auto &val) { adjust(tag, val, *this); },
               tags, data_m);
}

} // namespace CxxPlugins
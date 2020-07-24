/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    plugin_builder.hpp
 * \author  Andrey Ponomarev
 * \date    06 Jul 2020
 * \brief
 * $BRIEF$
 */
#pragma once

#include "cxx_plugins/function_traits.hpp"
#include "cxx_plugins/parser.hpp"
#include "cxx_plugins/polymorphic_allocator.hpp"

#include <boost/dll.hpp>
#include <boost/dll/shared_library_load_mode.hpp>
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>

namespace CxxPlugins {

struct Library {
  boost::dll::shared_library library_m;
  std::filesystem::path config_path_m = ".";

  operator boost::dll::shared_library &() noexcept { return library_m; }
  operator boost::dll::shared_library const &() const noexcept {
    return library_m;
  }
};

struct SharedLibrary {};
template <> struct JsonName<SharedLibrary> {
  static constexpr char const *value = "shared_library";
};

struct Config {};
template <> struct JsonName<Config> {
  static constexpr char const *value = "configuration";
};
struct Members {};
template <> struct JsonName<Members> {
  static constexpr char const *value = "members";
};

class ConfigLoadingFailure : public ParsingError {
  using ParsingError::ParsingError;
};
class LibraryLoadingFailure : public ParsingError {
  using ParsingError::ParsingError;
};

auto loadLibrary(std::filesystem::path const &lib_path,
                 std::filesystem::path const &config_path)
    -> boost::dll::shared_library;

template <typename... Fields>
constexpr auto makeDescriptor(Fields &&... fields) {
  return makeTupleMap(std::forward<Fields>(fields)...);
}

template <typename Tag, typename T> constexpr auto makeField(T &&value = {}) {
  return makeTaggedValue<Tag>(std::forward<T>(value));
}

template <typename Tag, typename... ChildFields>
constexpr auto makeSection(ChildFields &&... fields) {
  return makeTaggedValue<Tag>(
      makeTupleMap(std::forward<ChildFields>(fields)...));
}

template <typename Tag, typename T,
          typename Allocator = PolymorphicAllocator<T>>
constexpr auto makeList(std::vector<T, Allocator> const &vec) {
  return makeField<Tag, std::vector<T, Allocator>>(vec);
}

template <typename Tag, typename T,
          typename Allocator = PolymorphicAllocator<T>>
constexpr auto makeList(std::vector<T, Allocator> &&vec = {}) {
  return makeField<Tag, std::vector<T, Allocator>>(std::move(vec));
}

template <typename TagT, typename T> constexpr auto makeVariable() {
  if constexpr (std::is_function_v<T>) {
    return makeField<TagT, T **>();
  } else {
    return makeField<TagT, T *>();
  }
}
template <typename TagT, typename T> constexpr auto makeVariableList() {
  if constexpr (std::is_function_v<T>) {
    return makeField<TagT, T **>();
  } else {
    return makeList<TagT, T *>();
  }
}

template <typename TagT, typename Signature> constexpr auto makeFunction() {
  return makeField<TagT, Signature *>();
}
template <typename TagT, typename Signature> constexpr auto makeFunctionList() {
  return makeList<TagT, Signature *>();
}

template <typename ConfigType, typename MembersType>
constexpr auto makePlugin(ConfigType &&default_config,
                          MembersType &&default_members) {
  return makeTupleMap(
      makeField<SharedLibrary>(Library{}),
      makeField<Config>(std::forward<ConfigType>(default_config)),
      makeField<Members>(std::forward<MembersType>(default_members)));
}

template <typename... Fields> constexpr auto makeConfig(Fields &&... fields) {
  return makeTupleMap(std::forward<Fields>(fields)...);
}

template <typename... Fields> constexpr auto makeMembers(Fields &&... fields) {
  return makeTupleMap(std::forward<Fields>(fields)...);
}

template <typename Encoding, typename JSONAllocator, typename Return,
          typename... Args, typename AdditionalInfo>
void parse(rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
           Return (*&fn_ptr)(Args...), AdditionalInfo &&additional_info) {
  if (json_value.IsNull()) {
    fn_ptr = nullptr;
    return;
  }

  if (!json_value.IsString()) {
    throw TypeMismatch(fmt::format(
        "Failed to get function pointer '{}'. Should be a string in JSON.",
        type_id<Return (*)(Args...)>().name()));
  }

  auto const *json_name = json_value.GetString();

  boost::dll::shared_library const &lib =
      get<SharedLibrary>(std::forward<AdditionalInfo>(additional_info));

  fn_ptr = lib.get<Return(Args...)>(json_name);
}

template <typename Encoding, typename JSONAllocator, typename T,
          typename AdditionalInfo>
void parse(rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
           T *&variable_ptr, AdditionalInfo &&additional_info) {
  if (json_value.IsNull()) {
    variable_ptr = nullptr;
    return;
  }

  if (!json_value.IsString()) {
    throw TypeMismatch(fmt::format(
        "Failed to get pointer to variable '{}'. Should be a string in JSON.",
        type_id<T>().name()));
  }

  auto const *json_name = json_value.GetString();

  boost::dll::shared_library const &lib =
      get<SharedLibrary>(std::forward<AdditionalInfo>(additional_info));

  variable_ptr = &lib.get<T>(json_name);
}

template <typename AdditionalInfo = void *>
void parse(rapidjson::Value const &value, Library &lib,
           [[maybe_unused]] AdditionalInfo &&additional_info = nullptr) {
  if (value.IsNull()) {
    lib.library_m.load(boost::dll::program_location());
    return;
  }
  if (!value.IsString()) {
    throw TypeMismatch{
        fmt::format("Failed to get library. Should be a string in JSON.")};
  }

  auto const *lib_path_name = value.GetString();
  if (lib_path_name == std::string_view(".")) {
    lib.library_m.load(boost::dll::program_location());
  }

  std::filesystem::path lib_path = lib.config_path_m;
  lib_path += "/";
  lib_path += lib_path_name;

  lib.library_m.load(lib_path.c_str(),
                     boost::dll::load_mode::append_decorations);
  if (!lib.library_m.is_loaded()) {
    throw LibraryLoadingFailure{
        fmt::format("Failed to load library '{}'.", lib_path.c_str())};
  }
}

template <typename PluginT>
void loadPluginFromString(char const *config_p, PluginT &plugin) {
  rapidjson::Document document;
  document.Parse(config_p);
  parse(document, plugin, plugin);
}

template <typename PluginT>
void loadPluginFromString(std::string const &config, PluginT &plugin) {
  loadPluginFromString(config.c_str(), plugin);
}

template <typename PluginT>
void loadPluginFromFile(std::filesystem::path const &file_path, PluginT &plugin) {
  namespace fs = std::filesystem;
  if (!fs::exists(file_path)) {
    throw ConfigLoadingFailure(fmt::format(
        "Configuration file '{}' doesn't exist.", file_path.c_str()));
  }
  if (!fs::is_regular_file(file_path)) {
    throw ConfigLoadingFailure(fmt::format(
        "Configuration file '{}' is not a regular file", file_path.c_str()));
  }

  std::ifstream file(file_path);
  if (!file.is_open()) {
    throw ConfigLoadingFailure(fmt::format(
        "Failed to open configuration file '{}'", file_path.c_str()));
  }

  std::basic_string<char, std::char_traits<char>, PolymorphicAllocator<char>>
      data;

  file.seekg(0, std::ios::end);
  auto end_pos = file.tellg();
  file.seekg(0, std::ios::beg);

  data.reserve(end_pos);
  data.assign(std::istreambuf_iterator<char>{file},
              std::istreambuf_iterator<char>{});

  Library& lib = get<SharedLibrary>(plugin).value_m;
  lib.config_path_m = file_path.parent_path();

  loadPluginFromString(data.c_str(), plugin);

}

} // namespace CxxPlugins
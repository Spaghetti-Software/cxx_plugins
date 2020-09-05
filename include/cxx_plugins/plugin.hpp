/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    plugin.hpp
 * \author  Andrey Ponomarev
 * \date    06 Jul 2020
 * \brief
 * $BRIEF$
 */
#pragma once

#include "cxx_plugins/function_traits.hpp"
#include "tuple/tuple.hpp"
#include "tuple/tuple_map.hpp"

#include <filesystem>

struct api_id {};
struct api_version {};
struct plugin_name {};
struct plugin_version {};
struct library_path {};

namespace plugins {

namespace fs = std::filesystem;

using VersionInfo = plugins::Tuple<int, int, int>;

using SimplePluginInfo =
    plugins::TupleMap<TaggedValue<api_id, std::string>,
                         TaggedValue<api_version, VersionInfo>,
                         TaggedValue<plugin_name, std::string>,
                         TaggedValue<plugin_version, VersionInfo>,
                         TaggedValue<library_path, fs::path>>;

template <typename PluginInfo, typename PluginMembers> struct GenericPlugin;

template <typename... TaggedMembers>
using Plugin = GenericPlugin<SimplePluginInfo, TupleMap<TaggedMembers...>>;

template <typename... InfoTags, typename... InfoValues,
          typename... MemberTags, typename... MemberValues>
struct GenericPlugin<TupleMap<TaggedValue<InfoTags, InfoValues>...>,
                     TupleMap<TaggedValue<MemberTags, MemberValues>...>> {
public:

  using PluginInfoType = TupleMap<TaggedValue<InfoTags,InfoValues>...>;
  using PluginMembersType = TupleMap<TaggedValue<MemberTags, MemberValues>...>;

  template <typename InfoTag> decltype(auto) getInfo() const {
    return get<InfoTag>(info_m);
  }
  template <typename InfoTag> decltype(auto) getInfo() {
    return get<InfoTag>(info_m);
  }
  template <typename MemberTag> decltype(auto) getMember() const {
    // for non functions we return a reference, but store a pointer
    if constexpr (std::is_function_v<
                      TupleMapElementType<MemberTag, PluginMembersType>>) {
      return get<MemberTag>(members_m);
    } else {
      return *get<MemberTag>(members_m);
    }
  };
  template <typename MemberTag> decltype(auto) getMember() {
    // for non functions we return a reference, but store a pointer
    if constexpr (std::is_function_v<
                      TupleMapElementType<MemberTag, PluginMembersType>>) {
      return get<MemberTag>(members_m);
    } else {
      return *get<MemberTag>(members_m);
    }
  };

private:

  PluginMembersType members_m;
  PluginInfoType info_m;  // info goes last, because it can contain members
                          // like std::string or std::filesystem::path ...
};
} // namespace CxxPlugins
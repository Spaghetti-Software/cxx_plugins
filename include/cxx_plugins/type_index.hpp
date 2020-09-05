/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    type_index.hpp
 * \author  Andrey Ponomarev
 * \date    03 Jul 2020
 * \brief
 * $BRIEF$
 */
#pragma once

#include <boost/type_index/ctti_type_index.hpp>

#include <cstdint>
#include <memory_resource>
#include <string>

namespace plugins {

namespace impl {

static constexpr std::size_t type_index_static_buffer_size = 64;

using inner_type_index = boost::typeindex::ctti_type_index;

struct TypeInfo {
  char const *const type_m;
  std::uint64_t size_m;

  explicit operator std::string_view() const {
    return std::string_view(type_m, size_m);
  }
};

template <typename T>
inline auto typeInfoConstruct() noexcept -> const TypeInfo & {

  constexpr auto pretty_name = [](const char *raw_name, std::size_t len,
                                  std::pmr::memory_resource *resource) {
    std::pmr::string out(resource);
    while (raw_name[len - 1] == ' ') {
      --len; // MSVC sometimes adds whitespaces
    }
    return std::pmr::string(raw_name, len);
  };
  boost::typeindex::ctti_type_index::type_id<void>().pretty_name();

  constexpr auto replace_all = [](std::pmr::string &input,
                                  std::pmr::string const &substr,
                                  std::pmr::string const &with) {
    auto pos = input.find(substr);
    while (pos != std::pmr::string::npos) {
      input.replace(pos, substr.size(), with);
      pos = input.find(substr);
    }
  };

  constexpr auto format = [replace_all](std::pmr::string const &str) {
    std::pmr::string out = str;
    replace_all(out, "struct ", "");
    replace_all(out, "class ", "");
    return out;
  };

  static std::byte buffer[type_index_static_buffer_size];
  static std::pmr::monotonic_buffer_resource resource(std::data(buffer),
                                                      std::size(buffer));

  static const std::pmr::string type =
      format(pretty_name(inner_type_index::type_id<T>().raw_name(),
                         strlen(inner_type_index::type_id<T>().raw_name() +
                         boost::typeindex::detail::ctti_skip_size_at_end),
                         &resource));
  static const TypeInfo info{type.c_str(), type.size()};

  return info;
}

template <typename T>
inline auto typeInfoConstructWithCVR() noexcept -> const TypeInfo & {
  constexpr auto pretty_name = [](const char *raw_name, std::size_t len,
                                  std::pmr::memory_resource *resource) {
    std::pmr::string out(resource);
    while (raw_name[len - 1] == ' ') {
      --len; // MSVC sometimes adds whitespaces
    }
    return std::pmr::string(raw_name, len);
  };
  boost::typeindex::ctti_type_index::type_id<void>().pretty_name();

  constexpr auto replace_all = [](std::pmr::string &input,
                                  std::pmr::string const &substr,
                                  std::pmr::string const &with) {
    auto pos = input.find(substr);
    while (pos != std::pmr::string::npos) {
      input.replace(pos, substr.size(), with);
      pos = input.find(substr);
    }
  };

  constexpr auto format = [replace_all](std::pmr::string const &str) {
    std::pmr::string out = str;
    replace_all(out, "struct ", "");
    replace_all(out, "class ", "");
    return out;
  };

  static std::byte buffer[type_index_static_buffer_size];
  static std::pmr::monotonic_buffer_resource resource(std::data(buffer),
                                                      std::size(buffer));

  static const std::pmr::string type =
      format(pretty_name(inner_type_index::type_id_with_cvr<T>().raw_name(),
                         strlen(inner_type_index::type_id<T>().raw_name() +
                                boost::typeindex::detail::ctti_skip_size_at_end),
                         &resource));
  static const TypeInfo info{type.c_str(), type.size()};

  return info;
}
} // namespace impl

class type_index
    : public boost::typeindex::type_index_facade<type_index, impl::TypeInfo> {
private:
  const impl::TypeInfo *data_m;

public:
  using type_info_t = impl::TypeInfo;

  inline type_index() noexcept : data_m(&impl::typeInfoConstruct<void>()) {}
  inline type_index(type_info_t const &info) : data_m(&info) {}

  [[nodiscard]] inline auto type_info() const noexcept -> const type_info_t & {
    return *data_m;
  }
  [[nodiscard]] inline auto raw_name() const noexcept -> const char * {
    return data_m->type_m;
  }
  [[nodiscard]] inline auto name() const noexcept -> const char* {
    return data_m->type_m;
  }
  [[nodiscard]] inline auto pretty_name() const noexcept -> std::string {
    return std::string(data_m->type_m, data_m->size_m);
  }
  template <typename T> static inline auto type_id() noexcept -> type_index {
    return type_index(impl::typeInfoConstruct<T>());
  }
  template <typename T>
  static inline auto type_id_with_cvr() noexcept -> type_index {
    return impl::typeInfoConstructWithCVR<T>();
  }
};

template <typename T> inline type_index type_id() {
  return type_index::type_id<T>();
}

template <typename T> inline type_index type_id_with_cvr() {
  return type_index::type_id_with_cvr<T>();
}



} // namespace CxxPlugins
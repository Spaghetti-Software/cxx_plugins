/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    parser.hpp
 * \author  Andrey Ponomarev
 * \date    02 Jul 2020
 * \brief
 * $BRIEF$
 */
#pragma once

#include "sequence/conversion.hpp"
#include "tuple/tuple.hpp"
#include "tuple/tuple_map.hpp"
#include "type_index.hpp"

#include <rapidjson/document.h>

#include <fmt/format.h>

#include <boost/type_index.hpp>

#include <array>
#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace plugins {

template <typename T> struct JsonName {
  static inline char const * const value = type_id<T>().name();
};
template<typename T>
inline auto getJsonName()->char const* {
  return JsonName<T>::value;
}

class ParsingError : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

class TypeMismatch : public ParsingError {
  using ParsingError::ParsingError;
};

class ArraySizeMismatch : public ParsingError {
  using ParsingError::ParsingError;
};
class ObjectSizeMismatch : public ParsingError {
  using ParsingError::ParsingError;
};
class ObjectMemberMissing : public ParsingError {
  using ParsingError::ParsingError;
};

template <typename Encoding, typename Allocator>
auto getTypeFlagsAsString(
    rapidjson::GenericValue<Encoding, Allocator> const &json_value) noexcept
    -> std::string {
  std::string result = "{  ";
  result.reserve(32);

  if (json_value.IsNull())
    result += "Null, ";
  if (json_value.IsBool())
    result += "Bool, ";
  if (json_value.IsObject())
    result += "Object, ";
  if (json_value.IsArray())
    result += "Array, ";
  if (json_value.IsNumber())
    result += "Number, ";
  if (json_value.IsInt())
    result += "Int, ";
  if (json_value.IsUint())
    result += "Uint, ";
  if (json_value.IsInt64())
    result += "Int64, ";
  if (json_value.IsUint64())
    result += "Uint64, ";
  if (json_value.IsFloat())
    result += "Float, ";
  if (json_value.IsDouble())
    result += "Double, ";

  result.resize(result.size() - 2);
  result += "}";
  return result;
}

namespace impl {
[[noreturn]] void parsingLippincott(std::string_view type_description);
} // namespace impl

namespace impl {
template <typename T>
static constexpr bool is_optional_v = std::is_convertible_v<std::nullopt_t, T>;
template <typename T>
static constexpr bool is_pointer_v = std::is_convertible_v<std::nullptr_t, T>;
} // namespace impl



template <typename Int, typename Encoding, typename JSONAllocator,
          std::enable_if_t<std::is_integral_v<Int>, int> = 0,
          typename AdditionalInfo = void *>
void parse(rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
           Int &value, AdditionalInfo &&additional_info = nullptr);

template <typename Float, typename Encoding, typename JSONAllocator,
          std::enable_if_t<std::is_floating_point_v<Float>, unsigned> = 0,
          typename AdditionalInfo = void *>
void parse(rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
           Float &value, AdditionalInfo &&additional_info = nullptr);

template <typename Ch, typename ChTraits, typename Allocator, typename Encoding,
          typename JSONAllocator, typename AdditionalInfo = void *>
void parse(rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
           std::basic_string<Ch, ChTraits, Allocator> &string,
           AdditionalInfo &&additional_info = nullptr);

template <typename Encoding, typename JSONAllocator,
          typename AdditionalInfo = void *>
void parse(rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
           std::filesystem::path &path,
           AdditionalInfo &&additional_info = nullptr);

template <typename T, typename Allocator, typename Encoding,
          typename JSONAllocator, typename AdditionalInfo = void *>
void parse(rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
           std::vector<T, Allocator> &vec,
           AdditionalInfo &&additional_info = nullptr);

template <typename T, std::size_t Size, typename Encoding,
          typename JSONAllocator, typename AdditionalInfo = void *>
void parse(rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
           std::array<T, Size> &array,
           AdditionalInfo &&additional_info = nullptr);

template <typename T, std::size_t Size, typename Encoding,
          typename JSONAllocator, typename AdditionalInfo = void *>
void parse(rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
           T (&array)[Size], AdditionalInfo &&additional_info = nullptr);

template <typename Key, typename T, typename Hash, typename KeyEqual,
          typename Allocator, typename Encoding, typename JSONAllocator,
          typename AdditionalInfo = void *>
void parse(rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
           std::unordered_map<Key, T, Hash, KeyEqual, Allocator> &map,
           AdditionalInfo &&additional_info = nullptr);

template <typename Key, typename T, typename Hash, typename KeyEqual,
          typename Allocator, typename Encoding, typename JSONAllocator,
          typename AdditionalInfo = void *>
void parse(rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
           std::unordered_multimap<Key, T, Hash, KeyEqual, Allocator> &map,
           AdditionalInfo &&additional_info = nullptr);

template <typename Key, typename T, typename Compare, typename Allocator,
          typename Encoding, typename JSONAllocator,
          typename AdditionalInfo = void *>
void parse(rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
           std::map<Key, T, Compare, Allocator> &map,
           AdditionalInfo &&additional_info = nullptr);

template <typename Key, typename T, typename Compare, typename Allocator,
          typename Encoding, typename JSONAllocator,
          typename AdditionalInfo = void *>
void parse(rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
           std::multimap<Key, T, Compare, Allocator> &map,
           AdditionalInfo &&additional_info = nullptr);

template <typename... Ts, typename Encoding, typename JSONAllocator,
          typename AdditionalInfo = void *>
void parse(rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
           Tuple<Ts...> &tuple, AdditionalInfo &&additional_info = nullptr);

template <typename... Ts, typename Encoding, typename JSONAllocator,
          typename AdditionalInfo = void *>
void parse(rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
           std::tuple<Ts...> &tuple);

template <typename T, typename Encoding, typename JSONAllocator,
          typename AdditionalInfo = void *>
void parse(rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
           std::optional<T> &optional_value,
           AdditionalInfo &&additional_info = nullptr);

template <typename... Keys, typename... Values, typename Encoding,
          typename JSONAllocator, typename AdditionalInfo = void *>
void parse(rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
           plugins::TupleMap<TaggedValue<Keys, Values>...> &map,
           AdditionalInfo &&additional_info = nullptr);



/// IMPLEMENTATION

template <typename Int, typename Encoding, typename JSONAllocator,
    std::enable_if_t<std::is_integral_v<Int>, int>,
    typename AdditionalInfo>
void parse(rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
           Int &value, [[maybe_unused]] AdditionalInfo &&additional_info) {
  static_assert(std::is_same_v<int, Int> || std::is_same_v<unsigned, Int> ||
                    std::is_same_v<std::int64_t, Int> ||
                    std::is_same_v<std::uint64_t, Int>,
                "RapidJSON doesn't allow this integral type. "
                "It allows only following: int, unsigned, int64_t, uint64_t.");

  if (json_value.template Is<Int>()) {
    value = json_value.template Get<Int>();
  } else {
    throw TypeMismatch(fmt::format(
        "Failed to get type '{}'. JSON value has following type flags: {}.",
        type_id<Int>().name(), getTypeFlagsAsString(json_value)));
  }
}

template <typename Float, typename Encoding, typename JSONAllocator,
          std::enable_if_t<std::is_floating_point_v<Float>, unsigned>,
          typename AdditionalInfo>
void parse(rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
           Float &value, [[maybe_unused]] AdditionalInfo &&additional_info) {

  if (json_value.IsNumber()) {
    value = json_value.template Get<Float>();
  } else {
    throw TypeMismatch(fmt::format(
        "Failed to get type '{}'. JSON value has following type flags: {}.",
        type_id<Float>().name(), getTypeFlagsAsString(json_value)));
  }
}

template <typename Ch, typename ChTraits, typename Allocator, typename Encoding,
          typename JSONAllocator, typename AdditionalInfo>
void parse(rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
           std::basic_string<Ch, ChTraits, Allocator> &string,
           [[maybe_unused]] AdditionalInfo &&additional_info) {

  using string_type = std::basic_string<Ch, ChTraits, Allocator>;

  if (json_value.IsString()) {
    string.reserve(json_value.GetStringLength());
    string.assign(json_value.GetString(), json_value.GetStringLength());
  } else {
    throw TypeMismatch(fmt::format(
        "Failed to get type '{}'. JSON value has following type flags: {}.",
        type_id<string_type>().name(), getTypeFlagsAsString(json_value)));
  }
}

template <typename Encoding, typename JSONAllocator, typename AdditionalInfo>
void parse(rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
           std::filesystem::path &path, [[maybe_unused]] AdditionalInfo &&additional_info) {
  std::string path_name;
  path_name.reserve(256);
  parse(json_value, path_name);
  path.assign(std::move(path_name));
}

template <typename T, typename Allocator, typename Encoding,
          typename JSONAllocator, typename AdditionalInfo>
void parse(rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
           std::vector<T, Allocator> &vec, AdditionalInfo &&additional_info) {

  using json_value_t = rapidjson::GenericValue<Encoding, JSONAllocator>;
  using vector_t = std::vector<T, Allocator>;
  using array_t = typename json_value_t::ConstArray;

  if (json_value.IsArray()) {
    array_t const &array = json_value.GetArray();
    vec.clear();
    vec.resize(array.Size());
    for (unsigned i = 0; i < vec.size(); ++i) {
      try {
        parse(array[i], vec[i], std::forward<AdditionalInfo>(additional_info));
      } catch (...) {
        impl::parsingLippincott(
            fmt::format("{} at index {}", type_id<vector_t>().name(), i));
      }
    }
  } else {
    throw TypeMismatch(fmt::format(
        "Failed to get type '{}'. JSON value has following type flags: {}.",
        type_id<vector_t>().name(), getTypeFlagsAsString(json_value)));
  }
}

template <typename T, std::size_t Size, typename Encoding,
    typename JSONAllocator, typename AdditionalInfo>
void parse(rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
           std::array<T, Size> &array,
           AdditionalInfo &&additional_info) {

  using json_value_t = rapidjson::GenericValue<Encoding, JSONAllocator>;
  using array_t = std::array<T, Size>;
  using json_array_t = typename json_value_t::ConstArray;

  if (json_value.IsArray()) {

    json_array_t const &json_array = json_value.GetArray();

    if (json_array.Size() != Size) {
      throw ArraySizeMismatch(fmt::format(
          "Size of json array({}) doesn't match size of std::array({}).",
          json_array.Size(), Size));
    }

    for (unsigned i = 0; i < array.size(); ++i) {
      try {
        parse(json_array[i], array[i],
              std::forward<AdditionalInfo>(additional_info));
      } catch (...) {
        impl::parsingLippincott(
            fmt::format("{} at index {}", type_id<array_t>().name(), i));
      }
    }
  } else {
    throw TypeMismatch(fmt::format(
        "Failed to get type '{}'. JSON value has following type flags: {}.",
        type_id<array_t>().name(), getTypeFlagsAsString(json_value)));
  }
}

template <typename T, std::size_t Size, typename Encoding,
          typename JSONAllocator, typename AdditionalInfo>
void parse(rapidjson::GenericValue<Encoding, JSONAllocator> &json_value,
           T (&array)[Size], AdditionalInfo &&additional_info) {

  using json_value_t = rapidjson::GenericValue<Encoding, JSONAllocator>;
  using array_t = T[Size];
  using json_array_t = typename json_value_t::ConstArray;

  if (json_value.IsArray()) {

    json_array_t const &json_array = json_value.GetArray();

    if (json_array.Size() != Size) {
      throw ArraySizeMismatch(
          fmt::format("Size of json array({}) doesn't match size of {}.",
                      json_array.Size(), type_id<array_t>().name()));
    }

    for (unsigned i = 0; i < array.size(); ++i) {
      try {
        parse(json_array[i], array[i],
              std::forward<AdditionalInfo>(additional_info));
      } catch (...) {
        impl::parsingLippincott(
            fmt::format("{} at index {}", type_id<array_t>().name(), i));
      }
    }
  } else {
    throw TypeMismatch(fmt::format(
        "Failed to get type '{}'. JSON value has following type flags: {}.",
        type_id<array_t>().name(), getTypeFlagsAsString(json_value)));
  }
}

namespace impl {

template <typename MapT, typename Encoding, typename JSONAllocator,
          typename AdditionalInfo>
void deserializeMapImpl(
    rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
    MapT &map, AdditionalInfo &&additional_info) {

  using map_t = MapT;
  using value_t = typename map_t::mapped_type;
  using key_t = typename map_t::key_type;

  using json_value_t = rapidjson::GenericValue<Encoding, JSONAllocator>;
  using json_obj_t = typename json_value_t::ConstObject;
  using json_member_t = typename json_value_t::Member;

  if (json_value.IsObject()) {
    json_obj_t const &json_obj = json_value.GetObject();

    map.clear();

    for (json_member_t const &json_member : json_obj) {
      key_t key{};
      value_t value{};

      try {
        parse(json_member.name, key,
              std::forward<AdditionalInfo>(additional_info));
        parse(json_member.value, value,
              std::forward<AdditionalInfo>(additional_info));
      } catch (...) {
        impl::parsingLippincott(fmt::format(
            "{} at key {}", type_id<map_t>().name(), type_id<key_t>().name()));
      }
      map.emplace(std::move(key), std::move(value));
    }

  } else {
    throw TypeMismatch(fmt::format(
        "Failed to get type '{}'. JSON value has following type flags: {}.",
        type_id<map_t>().name(), getTypeFlagsAsString(json_value)));
  }
}
} // namespace impl

template <typename Key, typename T, typename Hash, typename KeyEqual,
          typename Allocator, typename Encoding, typename JSONAllocator,
          typename AdditionalInfo>
void parse(rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
           std::unordered_map<Key, T, Hash, KeyEqual, Allocator> &map,
           AdditionalInfo &&additional_info) {

  impl::deserializeMapImpl(json_value, map,
                           std::forward<AdditionalInfo>(additional_info));
}

template <typename Key, typename T, typename Hash, typename KeyEqual,
          typename Allocator, typename Encoding, typename JSONAllocator,
          typename AdditionalInfo>
void parse(rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
           std::unordered_multimap<Key, T, Hash, KeyEqual, Allocator> &map,
           AdditionalInfo &&additional_info) {
  impl::deserializeMapImpl(json_value, map,
                           std::forward<AdditionalInfo>(additional_info));
}

template <typename Key, typename T, typename Compare, typename Allocator,
          typename Encoding, typename JSONAllocator, typename AdditionalInfo>
void parse(rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
           std::map<Key, T, Compare, Allocator> &map,
           AdditionalInfo &&additional_info) {
  impl::deserializeMapImpl(json_value, map,
                           std::forward<AdditionalInfo>(additional_info));
}

template <typename Key, typename T, typename Compare, typename Allocator,
          typename Encoding, typename JSONAllocator, typename AdditionalInfo>
void parse(rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
           std::multimap<Key, T, Compare, Allocator> &map,
           AdditionalInfo &&additional_info) {
  impl::deserializeMapImpl(json_value, map,
                           std::forward<AdditionalInfo>(additional_info));
}

namespace impl {

template <typename Tuple, typename Encoding, typename JSONAllocator,
          typename AdditionalInfo>
void deserializeTupleImpl(
    rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
    Tuple &tuple, AdditionalInfo &&additional_info) {
  using tuple_t = Tuple;
  static constexpr std::size_t tuple_size = plugins::tuple_size_v<tuple_t>;
  static constexpr std::array<std::size_t, tuple_size> index_array =
      Sequence::AsStdArray<std::make_index_sequence<tuple_size>>::value;

  using json_value_t = rapidjson::GenericValue<Encoding, JSONAllocator>;
  using json_array_t = typename json_value_t::ConstArray;

  if (json_value.IsArray()) {

    json_array_t const &json_array = json_value.GetArray();

    if (json_array.Size() != tuple_size) {
      throw ArraySizeMismatch(
          fmt::format("Size of json array({}) doesn't match size of {}.",
                      json_array.Size(), type_id<tuple_t>().name()));
    }
    tupleForEach(
        [&json_array, &additional_info](auto &tuple_val,
                                        std::size_t const index) {
          try {
            parse(json_array[index], tuple_val,
                  std::forward<AdditionalInfo>(additional_info));
          } catch (...) {
            impl::parsingLippincott(fmt::format(
                "{} at index {}", type_id<tuple_t>().name(), index));
          }
        },
        tuple, index_array);

  } else {
    throw TypeMismatch(fmt::format(
        "Failed to get type '{}'. JSON value has following type flags: "
        "{}.(Note: tuples should be represented as lists in json)",
        type_id<tuple_t>().name(), getTypeFlagsAsString(json_value)));
  }
}

} // namespace impl

template <typename... Ts, typename Encoding, typename JSONAllocator,
          typename AdditionalInfo>
void parse(rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
           Tuple<Ts...> &tuple, AdditionalInfo &&additional_info) {
  impl::deserializeTupleImpl(json_value, tuple,
                             std::forward<AdditionalInfo>(additional_info));
}

template <typename... Ts, typename Encoding, typename JSONAllocator,
          typename AdditionalInfo>
void parse(rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
           std::tuple<Ts...> &tuple, AdditionalInfo &&additional_info) {
  impl::deserializeTupleImpl(json_value, tuple,
                             std::forward<AdditionalInfo>(additional_info));
}

template <typename T, typename Encoding, typename JSONAllocator,
          typename AdditionalInfo>
void parse(rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
           std::optional<T> &optional_value, AdditionalInfo &&additional_info) {
  if (json_value.IsNull()) {
    optional_value = std::nullopt;
  } else {
    if (!optional_value.has_value()) {
      optional_value.emplace();
    }
    parse(json_value, optional_value.value(),
          std::forward<AdditionalInfo>(additional_info));
  }
}

template <typename... Keys, typename... Values, typename Encoding,
          typename JSONAllocator, typename AdditionalInfo>
void parse(rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
           plugins::TupleMap<TaggedValue<Keys, Values>...> &map,
           AdditionalInfo &&additional_info) {

  using map_t = plugins::TupleMap<TaggedValue<Keys, Values>...>;

  using json_value_t = rapidjson::GenericValue<Encoding, JSONAllocator>;
  using json_object_t = typename json_value_t::ConstObject;

  if (!json_value.IsObject())
    throw TypeMismatch(fmt::format(
        "Failed to get type '{}'. JSON value has following type flags: "
        "{}.(Note: TupleMap should be represented as Object in json)",
        type_id<map_t>().name(), getTypeFlagsAsString(json_value)));

  json_object_t const &json_object = json_value.GetObject();

  tupleForEach(
      [&json_object, &additional_info](auto &tagged_member) {
        using TaggedMemberType = std::decay_t<decltype(tagged_member)>;
        using TagType = typename TaggedMemberType::TagType;
        using ValueType = typename TaggedMemberType::ValueType;
        auto name = getJsonName<TagType>();
        if (name == nullptr) {
          if (!type_id<TagType>().name()) {
            throw std::runtime_error(typeid(TagType).name());
          } else {
            throw std::runtime_error(type_id<TagType>().name());
          }

        }
        auto iter = json_object.FindMember(name);

        if (iter == json_object.end()) {
          if constexpr (!impl::is_optional_v<ValueType> &&
                        !impl::is_pointer_v<ValueType>) {
            throw ObjectMemberMissing(
                fmt::format("Couldn't find member {} for {}",
                            getJsonName<TagType>(), type_id<map_t>().name()));
          }
        } else {
          try {
            parse(iter->value, tagged_member.value_m,
                  std::forward<AdditionalInfo>(additional_info));
          } catch (...) {
            impl::parsingLippincott(fmt::format(
                "{} at key {}", type_id<map_t>().name(), getJsonName<TagType>()));
          }
        }
      },
      map);
}

} // namespace CxxPlugins
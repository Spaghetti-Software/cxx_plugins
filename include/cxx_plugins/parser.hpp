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

#include <rapidjson/document.h>

#include <fmt/format.h>

#include <boost/type_index.hpp>

#include <array>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace CxxPlugins {

template<typename T>
struct IsOptional : std::false_type {};

template<typename T>
struct IsOptional<std::optional<T>> : std::true_type {};

template<typename T>
static constexpr bool is_optional_v = IsOptional<T>::value;

struct Version {
  constexpr Version() noexcept = default;
  constexpr Version(Version const &) noexcept = default;
  constexpr Version(Version &&) noexcept = default;
  constexpr Version &operator=(Version const &) noexcept = default;
  constexpr Version &operator=(Version &&) noexcept = default;

  std::size_t major = 0;
  std::size_t minor = 0;
  std::size_t patch = 0;
};

constexpr auto operator==(Version const &lhs, Version const &rhs) -> bool {
  return lhs.major == rhs.major && lhs.minor == rhs.minor &&
         lhs.patch == rhs.patch;
}
constexpr auto operator!=(Version const &lhs, Version const &rhs) -> bool {
  return !(lhs == rhs);
}
constexpr auto operator<(Version const &lhs, Version const &rhs) -> bool {
  std::array<std::size_t, 3> lhs_arr = {lhs.major, lhs.minor, lhs.patch};
  std::array<std::size_t, 3> rhs_arr = {rhs.major, rhs.minor, rhs.patch};

  for (unsigned i = 0; i < 3; ++i) {
    if (lhs_arr[i] < rhs_arr[i]) {
      return true;
    }
    if (rhs_arr[i] < lhs_arr[i]) {
      return false;
    }
  }
  return false;
}
constexpr auto operator<=(Version const &lhs, Version const &rhs) -> bool {
  return !(rhs < lhs);
}
constexpr auto operator>(Version const &lhs, Version const &rhs) -> bool {
  return rhs < lhs;
}
constexpr auto operator>=(Version const &lhs, Version const &rhs) -> bool {
  return !(lhs < rhs);
}

struct GeneralInfo {
  std::string name_m;
};

namespace JSON {

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
[[noreturn]] void parsingLippincott(std::string_view type_description) {
  try {
    throw;
  } catch (TypeMismatch const &type_mismatch) {
    throw TypeMismatch(
        fmt::format("Exception thrown during parsing {}, message\n: {}",
                    type_description, type_mismatch.what()));
  } catch (ArraySizeMismatch const &array_size_mismatch) {
    throw ArraySizeMismatch(
        fmt::format("Exception thrown during parsing {}, message\n: {}",
                    type_description, array_size_mismatch.what()));
  } catch (ObjectSizeMismatch const &object_size_mismatch) {
    throw ObjectSizeMismatch(
        fmt::format("Exception thrown during parsing {}, message\n: {}",
                    type_description, object_size_mismatch.what()));
  } catch (ObjectMemberMissing const &object_member_missing) {
    throw ObjectMemberMissing(
        fmt::format("Exception thrown during parsing {}, message\n: {}",
                    type_description, object_member_missing.what()));
  } catch (ParsingError const &parsing_error) {
    throw ParsingError(
        fmt::format("Exception thrown during parsing {}, message\n: {}",
                    type_description, parsing_error.what()));
  } catch (std::runtime_error const &re) {
    throw std::runtime_error(
        fmt::format("Exception thrown during parsing {}, message\n: {}",
                    type_description, re.what()));
  } catch (...) {
    throw;
  }
}
} // namespace impl

template <typename Int, typename Encoding, typename JSONAllocator,
          std::enable_if_t<std::is_integral_v<Int>, int> = 0>
void deserializeValue(
    rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
    Int &value);

template <typename Float, typename Encoding, typename JSONAllocator,
          std::enable_if_t<std::is_floating_point_v<Float>, unsigned> = 0>
void deserializeValue(
    rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
    Float &value);

template <typename Ch, typename ChTraits, typename Allocator, typename Encoding,
          typename JSONAllocator>
void deserializeValue(
    rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
    std::basic_string<Ch, ChTraits, Allocator> &string);

template <typename T, typename Allocator, typename Encoding,
          typename JSONAllocator>
void deserializeValue(
    rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
    std::vector<T, Allocator> &vec);

template <typename T, std::size_t Size, typename Encoding,
          typename JSONAllocator>
void deserializeValue(
    rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
    std::array<T, Size> &array);

template <typename T, std::size_t Size, typename Encoding,
          typename JSONAllocator>
void deserializeValue(
    rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
    T (&array)[Size]);

template <typename Key, typename T, typename Hash, typename KeyEqual,
          typename Allocator, typename Encoding, typename JSONAllocator>
void deserializeValue(
    rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
    std::unordered_map<Key, T, Hash, KeyEqual, Allocator> &map);

template <typename Key, typename T, typename Hash, typename KeyEqual,
          typename Allocator, typename Encoding, typename JSONAllocator>
void deserializeValue(
    rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
    std::unordered_multimap<Key, T, Hash, KeyEqual, Allocator> &map);

template <typename Key, typename T, typename Compare, typename Allocator,
          typename Encoding, typename JSONAllocator>
void deserializeValue(
    rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
    std::map<Key, T, Compare, Allocator> &map);

template <typename Key, typename T, typename Compare, typename Allocator,
          typename Encoding, typename JSONAllocator>
void deserializeValue(
    rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
    std::multimap<Key, T, Compare, Allocator> &map);

template <typename... Ts, typename Encoding, typename JSONAllocator>
void deserializeValue(
    rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
    Tuple<Ts...> &tuple);

template <typename... Ts, typename Encoding, typename JSONAllocator>
void deserializeValue(
    rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
    std::tuple<Ts...> &tuple);

template <typename T, typename Encoding, typename JSONAllocator>
void deserializeValue(
    rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
    std::optional<T> &optional_value);

template <typename... Keys, typename... Values, typename Encoding,
          typename JSONAllocator>
void deserializeValue(
    rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
    CxxPlugins::TupleMap<TaggedValue<Keys, Values>...> &map);

template <typename Int, typename Encoding, typename JSONAllocator,
          std::enable_if_t<std::is_integral_v<Int>, int>>
void deserializeValue(
    rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
    Int &value) {
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
        boost::typeindex::type_id<Int>().pretty_name(),
        getTypeFlagsAsString(json_value)));
  }
}

template <typename Float, typename Encoding, typename JSONAllocator,
          std::enable_if_t<std::is_floating_point_v<Float>, unsigned>>
void deserializeValue(
    rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
    Float &value) {

  if (json_value.IsNumber()) {
    value = json_value.template Get<Float>();
  } else {
    throw TypeMismatch(fmt::format(
        "Failed to get type '{}'. JSON value has following type flags: {}.",
        boost::typeindex::type_id<Float>().pretty_name(),
        getTypeFlagsAsString(json_value)));
  }
}

template <typename Ch, typename ChTraits, typename Allocator, typename Encoding,
          typename JSONAllocator>
void deserializeValue(
    rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
    std::basic_string<Ch, ChTraits, Allocator> &string) {

  using string_type = std::basic_string<Ch, ChTraits, Allocator>;

  if (json_value.IsString()) {
    string.reserve(json_value.GetStringLength());
    string.assign(json_value.GetString(), json_value.GetStringLength());
  } else {
    throw TypeMismatch(fmt::format(
        "Failed to get type '{}'. JSON value has following type flags: {}.",
        boost::typeindex::type_id<string_type>().pretty_name(),
        getTypeFlagsAsString(json_value)));
  }
}

template <typename T, typename Allocator, typename Encoding,
          typename JSONAllocator>
void deserializeValue(
    rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
    std::vector<T, Allocator> &vec) {

  using json_value_t = rapidjson::GenericValue<Encoding, JSONAllocator>;
  using vector_t = std::vector<T, Allocator>;
  using array_t = typename json_value_t::ConstArray;

  if (json_value.IsArray()) {
    array_t const &array = json_value.GetArray();
    vec.clear();
    vec.resize(array.Size());
    for (unsigned i = 0; i < vec.size(); ++i) {
      try {
        deserializeValue(array[i], vec[i]);
      } catch (...) {
        impl::parsingLippincott(fmt::format(
            "{} at index {}",
            boost::typeindex::type_id<vector_t>().pretty_name(), i));
      }
    }
  } else {
    throw TypeMismatch(fmt::format(
        "Failed to get type '{}'. JSON value has following type flags: {}.",
        boost::typeindex::type_id<vector_t>().pretty_name(),
        getTypeFlagsAsString(json_value)));
  }
}

template <typename T, std::size_t Size, typename Encoding,
          typename JSONAllocator>
void deserializeValue(
    rapidjson::GenericValue<Encoding, JSONAllocator> &json_value,
    std::array<T, Size> &array) {

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
        deserializeValue(json_array[i], array[i]);
      } catch (...) {
        impl::parsingLippincott(
            fmt::format("{} at index {}",
                        boost::typeindex::type_id<array_t>().pretty_name(), i));
      }
    }
  } else {
    throw TypeMismatch(fmt::format(
        "Failed to get type '{}'. JSON value has following type flags: {}.",
        boost::typeindex::type_id<array_t>().pretty_name(),
        getTypeFlagsAsString(json_value)));
  }
}

template <typename T, std::size_t Size, typename Encoding,
          typename JSONAllocator>
void deserializeValue(
    rapidjson::GenericValue<Encoding, JSONAllocator> &json_value,
    T (&array)[Size]) {

  using json_value_t = rapidjson::GenericValue<Encoding, JSONAllocator>;
  using array_t = T[Size];
  using json_array_t = typename json_value_t::ConstArray;

  if (json_value.IsArray()) {

    json_array_t const &json_array = json_value.GetArray();

    if (json_array.Size() != Size) {
      throw ArraySizeMismatch(fmt::format(
          "Size of json array({}) doesn't match size of {}.", json_array.Size(),
          boost::typeindex::type_id<array_t>().pretty_name()));
    }

    for (unsigned i = 0; i < array.size(); ++i) {
      try {
        deserializeValue(json_array[i], array[i]);
      } catch (...) {
        impl::parsingLippincott(
            fmt::format("{} at index {}",
                        boost::typeindex::type_id<array_t>().pretty_name(), i));
      }
    }
  } else {
    throw TypeMismatch(fmt::format(
        "Failed to get type '{}'. JSON value has following type flags: {}.",
        boost::typeindex::type_id<array_t>().pretty_name(),
        getTypeFlagsAsString(json_value)));
  }
}

namespace impl {

template <typename MapT, typename Encoding, typename JSONAllocator>
void deserializeMapImpl(
    rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
    MapT &map) {

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
        deserializeValue(json_member.name, key);
        deserializeValue(json_member.value, value);
      } catch (...) {
        impl::parsingLippincott(fmt::format(
            "{}", boost::typeindex::type_id<map_t>().pretty_name()));
      }
      map.emplace(std::move(key), std::move(value));
    }

  } else {
    throw TypeMismatch(fmt::format(
        "Failed to get type '{}'. JSON value has following type flags: {}.",
        boost::typeindex::type_id<map_t>().pretty_name(),
        getTypeFlagsAsString(json_value)));
  }
}
} // namespace impl

template <typename Key, typename T, typename Hash, typename KeyEqual,
          typename Allocator, typename Encoding, typename JSONAllocator>
void deserializeValue(
    rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
    std::unordered_map<Key, T, Hash, KeyEqual, Allocator> &map) {

  impl::deserializeMapImpl(json_value, map);
}

template <typename Key, typename T, typename Hash, typename KeyEqual,
          typename Allocator, typename Encoding, typename JSONAllocator>
void deserializeValue(
    rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
    std::unordered_multimap<Key, T, Hash, KeyEqual, Allocator> &map) {
  impl::deserializeMapImpl(json_value, map);
}

template <typename Key, typename T, typename Compare, typename Allocator,
          typename Encoding, typename JSONAllocator>
void deserializeValue(
    rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
    std::map<Key, T, Compare, Allocator> &map) {
  impl::deserializeMapImpl(json_value, map);
}

template <typename Key, typename T, typename Compare, typename Allocator,
          typename Encoding, typename JSONAllocator>
void deserializeValue(
    rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
    std::multimap<Key, T, Compare, Allocator> &map) {
  impl::deserializeMapImpl(json_value, map);
}

namespace impl {

template <typename Tuple, typename Encoding, typename JSONAllocator>
void deserializeTupleImpl(
    rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
    Tuple &tuple) {
  using tuple_t = Tuple;
  static constexpr std::size_t tuple_size = CxxPlugins::tuple_size_v<tuple_t>;
  static constexpr std::array<std::size_t, tuple_size> index_array =
      Sequence::AsStdArray<std::make_index_sequence<tuple_size>>::value;

  using json_value_t = rapidjson::GenericValue<Encoding, JSONAllocator>;
  using json_array_t = typename json_value_t::ConstArray;

  if (json_value.IsArray()) {

    json_array_t const &json_array = json_value.GetArray();

    if (json_array.Size() != tuple_size) {
      throw ArraySizeMismatch(fmt::format(
          "Size of json array({}) doesn't match size of {}.", json_array.Size(),
          boost::typeindex::type_id<tuple_t>().pretty_name()));
    }
    tupleForEach(
        [&json_array](auto &tuple_val, std::size_t const index) {
          try {
            deserializeValue(json_array[index], tuple_val);
          } catch (...) {
            impl::parsingLippincott(fmt::format(
                "{} at index {}",
                boost::typeindex::type_id<tuple_t>().pretty_name(), index));
          }
        },
        tuple, index_array);

  } else {
    throw TypeMismatch(fmt::format(
        "Failed to get type '{}'. JSON value has following type flags: "
        "{}.(Note: tuples should be represented as lists in json)",
        boost::typeindex::type_id<tuple_t>().pretty_name(),
        getTypeFlagsAsString(json_value)));
  }
}

} // namespace impl

template <typename... Ts, typename Encoding, typename JSONAllocator>
void deserializeValue(
    rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
    Tuple<Ts...> &tuple) {
  impl::deserializeTupleImpl(json_value, tuple);
}

template <typename... Ts, typename Encoding, typename JSONAllocator>
void deserializeValue(
    rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
    std::tuple<Ts...> &tuple) {
  impl::deserializeTupleImpl(json_value, tuple);
}

template <typename T, typename Encoding, typename JSONAllocator>
void deserializeValue(
    rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
    std::optional<T> &optional_value) {
  if (json_value.IsNull()) {
    optional_value.reset();
  } else {
    if (!optional_value.has_value()) {
      optional_value.emplace();
    }
    deserializeValue(json_value, optional_value.value());
  }
}

template <typename... Keys, typename... Values, typename Encoding,
          typename JSONAllocator>
void deserializeValue(
    rapidjson::GenericValue<Encoding, JSONAllocator> const &json_value,
    CxxPlugins::TupleMap<TaggedValue<Keys, Values>...> &map) {

  using map_t = CxxPlugins::TupleMap<TaggedValue<Keys, Values>...>;
  static constexpr std::size_t map_size = CxxPlugins::tuple_size_v<map_t>;

  using json_value_t = rapidjson::GenericValue<Encoding, JSONAllocator>;
  using json_object_t = typename json_value_t::ConstObject;

  if (json_value.IsObject()) {

    json_object_t const &json_object = json_value.GetObject();
    static const std::array key_names = {
        boost::typeindex::type_id<Keys>().pretty_name()...};

    static constexpr bool has_optionals = (is_optional_v<Values> ||...);
    if constexpr (!has_optionals) {
      if (json_object.MemberCount() != map_size) {
        throw ObjectSizeMismatch(
            fmt::format("Size of json object({}) doesn't match size of {}.",
                        json_object.MemberCount(),
                        boost::typeindex::type_id<map_t>().pretty_name()));
      }
    }

    tupleForEach(
        [&json_object](auto &tuple_val, std::string const &key_name) {
          auto iter = json_object.FindMember(key_name.c_str());

          if (iter == json_object.end()) {
            if constexpr (is_optional_v<std::decay_t<decltype(tuple_val)>>) {
              tuple_val = std::nullopt;
            } else {
              throw ObjectMemberMissing(fmt::format(
                  "Couldn't find member {} for {}", key_name,
                  boost::typeindex::type_id<map_t>().pretty_name()));
            }
          } else {
            try {
              deserializeValue(iter->value, tuple_val);
            } catch (...) {
              impl::parsingLippincott(fmt::format(
                  "{} at key {}",
                  boost::typeindex::type_id<map_t>().pretty_name(), key_name));
            }
          }


        },
        map, key_names);

  } else {
    throw TypeMismatch(fmt::format(
        "Failed to get type '{}'. JSON value has following type flags: "
        "{}.(Note: TupleMap should be represented as Object in json)",
        boost::typeindex::type_id<map_t>().pretty_name(),
        getTypeFlagsAsString(json_value)));
  }
}

} // namespace JSON

} // namespace CxxPlugins
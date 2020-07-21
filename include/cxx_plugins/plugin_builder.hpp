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

#include "cxx_plugins/overload_set.hpp"
#include "cxx_plugins/plugin.hpp"
#include "cxx_plugins/type_index.hpp"

#include <fmt/format.h>
#include <rapidjson/document.h>

#include <fstream>

struct declaration_name {};

namespace CxxPlugins {

class ValidationError : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

class MissingValueError : public ValidationError {
public:
  using ValidationError::ValidationError;
};

class NullValueError : public ValidationError {
  using ValidationError::ValidationError;
};

struct JsonMissing {};
struct JsonNull {};

template <typename T> constexpr void ignoreValue(T & /*unused*/) {}

template <typename T>
constexpr void ignoreMissing(T & /*unused*/, JsonMissing /*unused*/) {}

template <typename T>
constexpr void ignoreNull(T & /*unused*/, JsonNull /*unused*/) {}

template <typename Callable, typename T>
constexpr bool is_value_validator_v =
    std::conditional_t<std::is_invocable_r_v<void, Callable, T &>,
                       std::true_type, std::false_type>::value;

template <typename Callable, typename T>
constexpr bool is_missing_validator_v =
    std::conditional_t<std::is_invocable_r_v<void, Callable, T &, JsonMissing>,
                       std::true_type, std::false_type>::value;

template <typename Callable, typename T>
constexpr bool is_null_validator_v =
    std::conditional_t<std::is_invocable_r_v<void, Callable, T &, JsonNull>,
                       std::true_type, std::false_type>::value;

template <typename T> constexpr bool is_field_v = is_tagged_value_v<T>;

template <typename Callable, typename T>
constexpr bool is_validator_v = is_value_validator_v<Callable, T>
    &&is_missing_validator_v<Callable, T> &&is_null_validator_v<Callable, T>;

template <typename ValueValidator, typename MissingValidator,
          typename NullValidator>
constexpr auto makeValidator(ValueValidator &&value_validator,
                             MissingValidator &&missing_validator,
                             NullValidator &&null_validator) {
  return makeOverloadSet(std::forward<ValueValidator>(value_validator),
                         std::forward<MissingValidator>(missing_validator),
                         std::forward<NullValidator>(null_validator));
}

inline char const *const missing_message_format =
    R"fmt(Missing required value '{}'.)fmt";
inline char const *const null_message_format =
    R"fmt(Null for required value '{}'.)fmt";

template <typename T, typename MessageType>
auto createThrowingOnMissing(MessageType message) {
  return [message](T & /*unused*/, JsonMissing /*unused*/) {
    throw MissingValueError(message);
  };
}

template <typename T, typename MessageType>
auto createThrowingOnNull(MessageType message) {
  return [message](T & /*unused*/, JsonNull /*unused*/) {
    throw NullValueError(message);
  };
}

template <typename... Members>
constexpr auto makePluginDescriptor(Members &&... members) {
  return makeTupleMap(std::forward<Members>(members)...);
}

////////////////////////////////////////////////////////////////////
//// CREATION OF FIELDS
////////////////////////////////////////////////////////////////////

template <typename FieldName, typename FieldType, typename Validator>
constexpr auto makeField(Validator &&validator) {
  static_assert(is_validator_v<std::decay_t<Validator>, FieldType>,
                "Validator is not valid.");
  return makeTaggedValue<FieldName>(
      makeTaggedValue<FieldType>(std::forward<Validator>(validator)));
}

template <typename FieldName, typename FieldType, typename ValueValidator,
          typename MissingValidator, typename NullValidator>
constexpr auto makeField(ValueValidator &&value_validator,
                         MissingValidator &&missing_validator,
                         NullValidator &&null_validator) {
  static_assert(is_value_validator_v<std::decay_t<ValueValidator>, FieldType>,
                "Value validator is not valid.");
  static_assert(
      is_missing_validator_v<std::decay_t<MissingValidator>, FieldType>,
      "Missing validator is not valid");
  static_assert(is_null_validator_v<std::decay_t<NullValidator>, FieldType>,
                "Null validator is not valid");

  return makeField<FieldName, FieldType>(
      makeValidator(std::forward<ValueValidator>(value_validator),
                    std::forward<MissingValidator>(missing_validator),
                    std::forward<NullValidator>(null_validator)));
}

template <typename FieldName, typename FieldType,
          typename ValueValidator = decltype(ignoreValue<FieldType>)>
constexpr auto
makeRequiredField(ValueValidator &&value_validator = ignoreValue<FieldType>) {
  return makeField<FieldName, FieldType>(
      std::forward<ValueValidator>(value_validator),
      createThrowingOnMissing<FieldType>(fmt::format(
          missing_message_format, type_id<FieldName>().pretty_name())),
      createThrowingOnNull<FieldType>(fmt::format(
          null_message_format, type_id<FieldName>().pretty_name())));
}

template <typename FieldName, typename FieldType,
          typename ValueValidator = decltype(ignoreValue<FieldType>),
          typename NullValidator = decltype(ignoreNull<FieldType>)>
constexpr auto makeRequiredNullableField(
    ValueValidator &&value_validator = ignoreValue<FieldType>,
    NullValidator &&null_validator = ignoreNull<FieldType>) {
  return makeField<FieldName, FieldType>(
      std::forward<ValueValidator>(value_validator),
      createThrowingOnMissing<FieldType>(fmt::format(
          missing_message_format, type_id<FieldName>().pretty_name())),
      std::forward<NullValidator>(null_validator));
}

template <typename FieldName, typename FieldType,
          typename ValueValidator = decltype(ignoreValue<FieldType>),
          typename MissingValidator = decltype(ignoreMissing<FieldType>),
          typename NullValidator = decltype(ignoreNull<FieldType>)>
constexpr auto makeOptionalField(
    ValueValidator &&value_validator = ignoreValue<FieldType>,
    MissingValidator &&missing_validator = ignoreMissing<FieldType>,
    NullValidator &&null_validator = ignoreNull<FieldType>) {
  return makeField<FieldName, FieldType>(
      std::forward<ValueValidator>(value_validator),
      std::forward<MissingValidator>(missing_validator),
      std::forward<NullValidator>(null_validator));
}

template <typename FieldName, typename FieldType,
          typename ValueValidator = decltype(ignoreValue<FieldType>),
          typename MissingValidator = decltype(ignoreMissing<FieldType>)>
constexpr auto makeOptionalNonNullableField(
    ValueValidator &&value_validator = ignoreValue<FieldType>,
    MissingValidator &&missing_validator = ignoreMissing<FieldType>) {
  return makeField<FieldName, FieldType>(
      std::forward<ValueValidator>(value_validator),
      std::forward<MissingValidator>(missing_validator),
      createThrowingOnNull<FieldType>(fmt::format(
          null_message_format, type_id<FieldName>().pretty_name())));
}

////////////////////////////////////////////////////////////////////
//// CREATION OF SECTIONS
////////////////////////////////////////////////////////////////////
template <typename... Members> struct SectionResult {
  //! \todo Change this type to real type of section
  using Type =
      decltype(makeTupleMap(std::forward<Members>(std::declval<Members>())...));
};

template <typename... Members>
using SectionResultT = typename SectionResult<Members...>::Type;

template <typename SectionName, typename MissingValidator,
          typename NullValidator, typename... Members>
[[deprecated("(Function is incomplete. Look at todo section)")]] constexpr auto
makeSection(MissingValidator &&missing_validator,
            NullValidator &&null_validator, Members &&... members) {

  using Type = decltype(makeTupleMap(std::forward<Members>(members)...));

  return makeField<SectionName, Type>(
      makeValidator(ignoreValue<SectionResultT<Members...>>,
                    std::forward<MissingValidator>(missing_validator),
                    std::forward<NullValidator>(null_validator)));
}

template <typename SectionName, typename... Members>
constexpr auto makeRequiredSection(Members &&... members) {
  return makeSection<SectionName>(
      createThrowingOnMissing<SectionResultT<Members...>>(fmt::format(
          missing_message_format, type_id<SectionName>().pretty_name())),
      createThrowingOnNull<SectionResultT<Members...>>(fmt::format(
          null_message_format, type_id<SectionName>().pretty_name())),
      std::forward<Members>(members)...);
}

template <
    typename SectionName, typename NullValidator, typename... Members,
    typename = std::enable_if_t<(is_field_v<std::decay_t<Members>> && ...)>,
    std::enable_if_t<is_null_validator_v<std::decay_t<NullValidator>,
                                         SectionResultT<Members...>>,
                     int> = 0>
constexpr auto makeRequiredNullableSection(NullValidator &&null_validator,
                                           Members &&... members) {
  return makeSection<SectionName>(
      createThrowingOnMissing<SectionResultT<Members...>>(fmt::format(
          missing_message_format, type_id<SectionName>().pretty_name())),
      std::forward<NullValidator>(null_validator),
      std::forward<Members>(members)...);
}

template <typename SectionName> constexpr auto makeRequiredNullableSection() {
  return makeRequiredNullableSection<SectionName>(ignoreNull<SectionResultT<>>);
}

template <
    typename SectionName, typename FirstMember, typename... RestMembers,
    typename = std::enable_if_t<is_field_v<std::decay_t<FirstMember>> &&
                                (is_field_v<std::decay_t<RestMembers>> && ...)>,
    std::enable_if_t<
        is_null_validator_v<std::decay_t<FirstMember>,
                            SectionResultT<FirstMember, RestMembers...>>,
        unsigned> = 0>
constexpr auto makeRequiredNullableSection(FirstMember &&first,
                                           RestMembers &&... rest) {
  return makeRequiredNullableSection<SectionName>(
      ignoreNull<SectionResultT<FirstMember, RestMembers...>>,
      std::forward<FirstMember>(first), std::forward<RestMembers>(rest)...);
}

template <
    typename SectionName, typename MissingValidator, typename NullValidator,
    typename... Members,
    typename = std::enable_if_t<(is_field_v<std::decay_t<Members>> && ...)>,
    std::enable_if_t<is_missing_validator_v<std::decay_t<MissingValidator>,
                                            SectionResultT<Members...>>,
                     int> = 0,
    std::enable_if_t<is_null_validator_v<std::decay_t<NullValidator>,
                                         SectionResultT<Members...>>,
                     int> = 0>
constexpr auto makeOptionalSection(MissingValidator &&missing_validator,
                                   NullValidator &&null_validator,
                                   Members &&... members) {
  return makeSection<SectionName>(
      std::forward<MissingValidator>(missing_validator),
      std::forward<NullValidator>(null_validator),
      std::forward<Members>(members)...);
}

template <
    typename SectionName, typename MissingValidator,
    std::enable_if_t<is_missing_validator_v<std::decay_t<MissingValidator>,
                                            SectionResultT<>>,
                     int> = 0>
constexpr auto makeOptionalSection(MissingValidator &&missing_validator) {
  return makeOptionalSection<SectionName>(
      std::forward<MissingValidator>(missing_validator),
      ignoreNull<SectionResultT<>>);
}

template <typename SectionName, typename NullValidator,
          std::enable_if_t<is_null_validator_v<std::decay_t<NullValidator>,
                                               SectionResultT<>>,
                           int> = 0>
constexpr auto makeOptionalSection(NullValidator &&null_validator) {
  return makeOptionalSection<SectionName>(
      ignoreMissing<SectionResultT<>>,
      std::forward<NullValidator>(null_validator));
}

template <typename SectionName> constexpr auto makeOptionalSection() {
  return makeOptionalSection<SectionName>(ignoreMissing<SectionResultT<>>,
                                          ignoreNull<SectionResultT<>>);
}

template <
    typename SectionName, typename FirstMember, typename... RestMembers,
    typename = std::enable_if_t<is_field_v<std::decay_t<FirstMember>> &&
                                (is_field_v<std::decay_t<RestMembers>> && ...)>,
    std::enable_if_t<!is_missing_validator_v<std::decay_t<FirstMember>,
                                             SectionResultT<RestMembers...>>,
                     unsigned> = 0,
    std::enable_if_t<!is_null_validator_v<std::decay_t<FirstMember>,
                                          SectionResultT<RestMembers...>>,
                     unsigned> = 0>
constexpr auto makeOptionalSection(FirstMember &&first,
                                   RestMembers &&... rest) {
  return makeOptionalSection<SectionName>(
      ignoreMissing<SectionResultT<FirstMember, RestMembers...>>,
      ignoreNull<SectionResultT<FirstMember, RestMembers...>>,
      std::forward<FirstMember>(first), std::forward<RestMembers>(rest)...);
}

template <
    typename SectionName, typename MissingValidator, typename FirstMember,
    typename... RestMembers,
    typename = std::enable_if_t<is_field_v<std::decay_t<FirstMember>> &&
                                (is_field_v<std::decay_t<RestMembers>> && ...)>,
    std::enable_if_t<
        is_missing_validator_v<std::decay_t<MissingValidator>,
                               SectionResultT<FirstMember, RestMembers...>>,
        int> = 0,
    std::enable_if_t<!is_null_validator_v<std::decay_t<FirstMember>,
                                          SectionResultT<RestMembers...>>,
                     unsigned> = 0>
constexpr auto makeOptionalSection(MissingValidator &&missing_validator,
                                   FirstMember &&first,
                                   RestMembers &&... rest) {
  return makeOptionalSection<SectionName>(
      std::forward<MissingValidator>(missing_validator),
      ignoreNull<SectionResultT<FirstMember, RestMembers...>>,
      std::forward<FirstMember>(first), std::forward<RestMembers>(rest)...);
}

template <
    typename SectionName, typename NullValidator, typename FirstMember,
    typename... RestMembers,
    typename = std::enable_if_t<is_field_v<std::decay_t<FirstMember>> &&
                                (is_field_v<std::decay_t<RestMembers>> && ...)>,
    std::enable_if_t<
        is_null_validator_v<std::decay_t<NullValidator>,
                            SectionResultT<FirstMember, RestMembers...>>,
        int> = 0>
constexpr auto makeOptionalSection(NullValidator &&null_validator,
                                   FirstMember &&first,
                                   RestMembers &&... rest) {
  return makeOptionalSection<SectionName>(
      ignoreMissing<SectionResultT<FirstMember, RestMembers...>>,
      std::forward<NullValidator>(null_validator),
      std::forward<FirstMember>(first), std::forward<RestMembers>(rest)...);
}

template <
    typename SectionName, typename MissingValidator, typename... Members,
    typename = std::enable_if_t<(is_field_v<std::decay_t<Members>> && ...)>,
    std::enable_if_t<is_missing_validator_v<std::decay_t<MissingValidator>,
                                            SectionResultT<Members...>>,
                     int> = 0>
constexpr auto
makeOptionalNonNullableSection(MissingValidator &&missing_validator,
                               Members &&... members) {
  return makeSection<SectionName>(
      std::forward<MissingValidator>(missing_validator),
      createThrowingOnNull<SectionResultT<Members...>>(fmt::format(
          null_message_format, type_id<SectionName>().pretty_name())),
      std::forward<Members>(members)...);
}

template <typename SectionName>
constexpr auto makeOptionalNonNullableSection() {
  return makeOptionalNonNullableSection<SectionName>(
      ignoreMissing<SectionResultT<>>);
}

template <
    typename SectionName, typename FirstMember, typename... RestMembers,
    typename = std::enable_if_t<is_field_v<std::decay_t<FirstMember>> &&
                                (is_field_v<std::decay_t<RestMembers>> && ...)>,
    std::enable_if_t<!is_missing_validator_v<std::decay_t<FirstMember>,
                                             SectionResultT<RestMembers...>>,
                     unsigned> = 0>
constexpr auto makeOptionalNonNullableSection(FirstMember &&first,
                                              RestMembers &&... rest) {
  return makeOptionalNonNullableSection<SectionName>(
      ignoreMissing<SectionResultT<FirstMember, RestMembers...>>,
      std::forward<FirstMember>(first), std::forward<RestMembers>(rest)...);
}

////////////////////////////////////////////////////////////////////
//// CREATION OF FUNCTIONS
////////////////////////////////////////////////////////////////////

template <typename FunctionName, typename FunctionSignature,
          typename... AdditionalFields>
constexpr auto makeRequiredFunction(AdditionalFields &&... additional_fields) {
  static_assert(std::is_function_v<FunctionSignature>,
                "Should be a function signature.");
  return makeRequiredSection<FunctionName>(
      makeRequiredField<declaration_name,
                        utility::FunctionPointer<FunctionSignature>>(),
      std::forward<AdditionalFields>(additional_fields)...);
}

template <typename FunctionName, typename FunctionSignature,
          typename... AdditionalFields>
constexpr auto
makeNullableRequiredFunction(AdditionalFields &&... additional_fields) {
  static_assert(std::is_function_v<FunctionSignature>,
                "Should be a function signature.");
  return makeRequiredNullableSection<FunctionName>(
      makeRequiredField<declaration_name,
                        utility::FunctionPointer<FunctionSignature>>(),
      std::forward<AdditionalFields>(additional_fields)...);
}

template <typename FunctionName, typename FunctionSignature,
          typename... AdditionalFields>
constexpr auto makeOptionalFunction(AdditionalFields &&... additional_fields) {
  static_assert(std::is_function_v<FunctionSignature>,
                "Should be a function signature.");
  return makeOptionalSection<FunctionName>(
      makeRequiredField<declaration_name,
                        utility::FunctionPointer<FunctionSignature>>(),
      std::forward<AdditionalFields>(additional_fields)...);
}

template <typename FunctionName, typename FunctionSignature,
          typename... AdditionalFields>
constexpr auto
makeOptionalNonNullableFunction(AdditionalFields &&... additional_fields) {
  static_assert(std::is_function_v<FunctionSignature>,
                "Should be a function signature.");
  return makeOptionalNonNullableSection<FunctionName>(
      makeRequiredField<declaration_name,
                        utility::FunctionPointer<FunctionSignature>>(),
      std::forward<AdditionalFields>(additional_fields)...);
}

////////////////////////////////////////////////////////////////////
//// CREATION OF VARIABLES
////////////////////////////////////////////////////////////////////

template <
    typename VariableName, typename VariableType,
    typename ValuePointerValidator = decltype(ignoreValue<VariableType *>),
    typename... AdditionalFields>
constexpr auto makeRequiredVariable(ValuePointerValidator &&value_ptr_validator,
                                    AdditionalFields &&... additional_fields) {
  return makeRequiredSection<VariableName>(
      makeRequiredField<declaration_name, VariableType *>(
          std::forward<ValuePointerValidator>(value_ptr_validator)),
      std::forward<AdditionalFields>(additional_fields)...);
}

template <
    typename VariableName, typename VariableType,
    typename ValuePointerValidator = decltype(ignoreValue<VariableType *>),
    typename NullValidator = decltype(ignoreNull<VariableType *>),
    typename... AdditionalFields>
constexpr auto makeRequiredNullableVariable(
    ValuePointerValidator &&value_ptr_validator = ignoreValue<VariableType *>,
    NullValidator &&null_validator = ignoreNull<VariableType *>,
    AdditionalFields &&... additional_fields) {
  return makeRequiredNullableSection<VariableName>(
      makeRequiredField<declaration_name, VariableType *>(
          std::forward<ValuePointerValidator>(value_ptr_validator)),
      std::forward<NullValidator>(null_validator),
      std::forward<AdditionalFields>(additional_fields)...);
}

template <
    typename VariableName, typename VariableType,
    typename ValuePointerValidator = decltype(ignoreValue<VariableType *>),
    typename MissingValidator = decltype(ignoreMissing<VariableType *>),
    typename NullValidator = decltype(ignoreNull<VariableType *>),
    typename... AdditionalFields>
constexpr auto makeOptionalVariable(
    ValuePointerValidator &&value_ptr_validator = ignoreValue<VariableType *>,
    MissingValidator &&missing_validator = ignoreMissing<VariableType *>,
    NullValidator &&null_validator = ignoreNull<VariableType *>,
    AdditionalFields &&... additional_fields) {
  return makeOptionalSection<VariableName>(
      makeRequiredField<declaration_name, VariableType *>(value_ptr_validator),
      std::forward<MissingValidator>(missing_validator),
      std::forward<NullValidator>(null_validator),
      std::forward<AdditionalFields>(additional_fields)...);
}

template <
    typename VariableName, typename VariableType,
    typename ValuePointerValidator = decltype(ignoreValue<VariableType *>),
    typename MissingValidator = decltype(ignoreMissing<VariableType *>),
    typename... AdditionalFields>
constexpr auto makeOptionalNonNullableVariable(
    ValuePointerValidator &&value_ptr_validator = ignoreValue<VariableType *>,
    MissingValidator &&missing_validator = ignoreMissing<VariableType *>,
    AdditionalFields &&... additional_fields) {
  return makeOptionalNonNullableSection<VariableName>(
      makeRequiredField<declaration_name, VariableType *>(value_ptr_validator),
      std::forward<MissingValidator>(missing_validator),
      std::forward<AdditionalFields>(additional_fields)...);
}

namespace impl {

template <typename T> struct FieldUnroller;

template <typename T> using FieldUnrollerT = typename FieldUnroller<T>::Type;

template <typename FieldName, typename FieldType, typename Validator>
struct FieldUnroller<
    TaggedValue<FieldName, TaggedValue<FieldType, Validator>>> {
  using Type = TaggedValue<FieldName, FieldType>;
};

template <typename FieldName, typename... FieldTypeTaggedVals,
          typename Validator>
struct FieldUnroller<TaggedValue<
    FieldName, TaggedValue<TupleMap<FieldTypeTaggedVals...>, Validator>>> {
  using Type =
      TaggedValue<FieldName, TupleMap<FieldUnrollerT<FieldTypeTaggedVals>...>>;
};

} // namespace impl

template <typename T> struct PluginFromLoader;

template <typename... TaggedVals>
struct PluginFromLoader<TupleMap<TaggedVals...>> {
  using Type = TupleMap<impl::FieldUnrollerT<TaggedVals>...>;
};

template <typename T>
using PluginFromLoaderT = typename PluginFromLoader<T>::Type;

template <typename Key, typename ValueT, typename Validator>
void deserializeRecursive(rapidjson::Document::Object const &json_parent,
                          ValueT &value, Validator &validator) {
  std::cerr << fmt::format(R"(Can't deserialize type '{}')",
                           type_id<ValueT>().name()) << '\n';
}


template <typename Key, typename Validator>
void deserializeRecursive(rapidjson::Document::Object const &json_parent,
                          std::string &str, Validator &validator) {
  rapidjson::Document::MemberIterator iter = json_parent.FindMember(type_id<Key>().name());
  if (iter == json_parent.end()) {
    validator(str, JsonMissing{});
  } else if (iter->value.IsNull()) {
    validator(str, JsonNull{});
  } else {
    validator(str);
  }
}

template <typename... FieldKeys, typename... FieldValues,
          typename... FieldValidators>
auto deserialize(
    rapidjson::Document::Object const &json_obj,
    TupleMap<TaggedValue<FieldKeys,
                         TaggedValue<FieldValues, FieldValidators>>...> const
        &loader) {

  PluginFromLoaderT<TupleMap<
      TaggedValue<FieldKeys, TaggedValue<FieldValues, FieldValidators>>...>>
      plugin{};

  Tuple<FieldKeys...> keys{};

  tupleForEach(
      [&json_obj](auto &plugin_member, auto const &loader_member,
                  auto const &key) {
        deserializeRecursive<std::decay_t<decltype(key)>>(
            json_obj, plugin_member, loader_member.value_m);
      },
      plugin, loader, keys);
  return plugin;
}

template <typename Loader>
auto loadPluginFromString(char const *configuration, Loader const &loader) {
  rapidjson::Document document;
  document.Parse(configuration);

  return deserialize(document.GetObject(), loader);
}

} // namespace CxxPlugins
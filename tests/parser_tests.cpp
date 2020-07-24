///*************************************************************************************************
// * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
// * This file is part of CXX Plugins project.
// * License is available at
// * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
// *************************************************************************************************/
///*!
// * \file    parser_tests.cpp
// * \author  Andrey Ponomarev
// * \date    02 Jul 2020
// * \brief
// * $BRIEF$
// */
//#include <cxx_plugins/parser.hpp>
//#include <gtest/gtest.h>
//
//TEST(ParserTests, NumberTests) {
//  using namespace CxxPlugins;
//  const char *json_u = "4";
//  const char *json_i = "-4";
//  const char *json_f = "4.5";
//
//  rapidjson::Document doc_u_non_const;
//  doc_u_non_const.Parse(json_u);
//  rapidjson::Document const &doc_u = doc_u_non_const;
//
//  rapidjson::Document doc_i_non_const;
//  doc_i_non_const.Parse(json_i);
//  rapidjson::Document const &doc_i = doc_i_non_const;
//
//  rapidjson::Document doc_f_non_const;
//  doc_f_non_const.Parse(json_f);
//  rapidjson::Document const &doc_f = doc_f_non_const;
//
//  unsigned uvar = 0;
//  parse(doc_u, uvar);
//  EXPECT_EQ(uvar, 4);
//  EXPECT_THROW({ parse(doc_i, uvar); }, TypeMismatch);
//  EXPECT_THROW({ parse(doc_f, uvar); }, TypeMismatch);
//
//  int ivar = 0;
//  parse(doc_u, ivar);
//  EXPECT_EQ(ivar, 4);
//  parse(doc_i, ivar);
//  EXPECT_EQ(ivar, -4);
//  EXPECT_THROW({ parse(doc_f, ivar); }, TypeMismatch);
//
//  std::uint64_t u64var = 0;
//  parse(doc_u, u64var);
//  EXPECT_EQ(u64var, 4);
//  EXPECT_THROW({ parse(doc_i, u64var); }, TypeMismatch);
//  EXPECT_THROW({ parse(doc_f, u64var); }, TypeMismatch);
//
//  std::int64_t i64var = 0;
//  parse(doc_u, i64var);
//  EXPECT_EQ(i64var, 4);
//  parse(doc_i, i64var);
//  EXPECT_EQ(i64var, -4);
//  EXPECT_THROW({ parse(doc_f, u64var); }, TypeMismatch);
//
//  float f = 0;
//  parse(doc_u, f);
//  EXPECT_EQ(f, 4);
//  parse(doc_i, f);
//  EXPECT_EQ(f, -4);
//  parse(doc_f, f);
//  EXPECT_EQ(f, 4.5);
//
//  double d = 0;
//  parse(doc_u, d);
//  EXPECT_EQ(d, 4);
//  parse(doc_i, d);
//  EXPECT_EQ(d, -4);
//  parse(doc_f, d);
//  EXPECT_EQ(d, 4.5);
//}
//
//TEST(ParserTests, OptionalNumbers) {
//  using namespace CxxPlugins;
//  const char *json_u = "4";
//  const char *json_i = "-4";
//  const char *json_f = "4.5";
//  const char *json_null = "null";
//
//  rapidjson::Document doc_u_non_const;
//  doc_u_non_const.Parse(json_u);
//  rapidjson::Document const &doc_u = doc_u_non_const;
//
//  rapidjson::Document doc_i_non_const;
//  doc_i_non_const.Parse(json_i);
//  rapidjson::Document const &doc_i = doc_i_non_const;
//
//  rapidjson::Document doc_f_non_const;
//  doc_f_non_const.Parse(json_f);
//  rapidjson::Document const &doc_f = doc_f_non_const;
//
//  rapidjson::Document doc_null_non_const;
//  doc_null_non_const.Parse(json_null);
//  rapidjson::Document const &doc_null = doc_null_non_const;
//
//  std::optional<unsigned> uvar = 0;
//  parse(doc_null, uvar);
//  EXPECT_EQ(uvar, std::nullopt);
//  parse(doc_u, uvar);
//  EXPECT_EQ(uvar, 4);
//  EXPECT_THROW({ parse(doc_i, uvar); }, TypeMismatch);
//  EXPECT_THROW({ parse(doc_f, uvar); }, TypeMismatch);
//
//  std::optional<int> ivar = 0;
//  parse(doc_null, ivar);
//  EXPECT_EQ(ivar, std::nullopt);
//  parse(doc_u, ivar);
//  EXPECT_EQ(ivar, 4);
//  parse(doc_i, ivar);
//  EXPECT_EQ(ivar, -4);
//  EXPECT_THROW({ parse(doc_f, ivar); }, TypeMismatch);
//
//  std::optional<std::uint64_t> u64var = 0;
//  parse(doc_null, u64var);
//  EXPECT_EQ(u64var, std::nullopt);
//  parse(doc_u, u64var);
//  EXPECT_EQ(u64var, 4);
//  EXPECT_THROW({ parse(doc_i, u64var); }, TypeMismatch);
//  EXPECT_THROW({ parse(doc_f, u64var); }, TypeMismatch);
//
//  std::optional<std::int64_t> i64var = 0;
//  parse(doc_null, i64var);
//  EXPECT_EQ(i64var, std::nullopt);
//  parse(doc_u, i64var);
//  EXPECT_EQ(i64var, 4);
//  parse(doc_i, i64var);
//  EXPECT_EQ(i64var, -4);
//  EXPECT_THROW({ parse(doc_f, u64var); }, TypeMismatch);
//
//  std::optional<float> f = 0;
//  parse(doc_null, f);
//  EXPECT_EQ(f, std::nullopt);
//  parse(doc_u, f);
//  EXPECT_EQ(f, 4);
//  parse(doc_i, f);
//  EXPECT_EQ(f, -4);
//  parse(doc_f, f);
//  EXPECT_EQ(f, 4.5);
//
//  std::optional<double> d = 0;
//  parse(doc_null, d);
//  EXPECT_EQ(d, std::nullopt);
//  parse(doc_u, d);
//  EXPECT_EQ(d, 4);
//  parse(doc_i, d);
//  EXPECT_EQ(d, -4);
//  parse(doc_f, d);
//  EXPECT_EQ(d, 4.5);
//}
//
//TEST(ParserTests, String) {
//  using namespace CxxPlugins;
//  const char *json_str = "\"string\"";
//  const char *json_null = "null";
//
//  rapidjson::Document doc_non_const;
//  doc_non_const.Parse(json_str);
//  rapidjson::Document const &doc = doc_non_const;
//
//  rapidjson::Document doc_null_non_const;
//  doc_null_non_const.Parse(json_null);
//  rapidjson::Document const &doc_null = doc_null_non_const;
//
//  std::string str;
//  parse(doc, str);
//  EXPECT_EQ(str, "string");
//
//  EXPECT_THROW({ parse(doc_null, str); }, TypeMismatch);
//}
//
//TEST(ParserTests, OptionalString) {
//  using namespace CxxPlugins;
//  const char *json_str = "\"string\"";
//  const char *json_null = "null";
//
//  rapidjson::Document doc_non_const;
//  doc_non_const.Parse(json_str);
//  rapidjson::Document const &doc = doc_non_const;
//
//  rapidjson::Document doc_null_non_const;
//  doc_null_non_const.Parse(json_null);
//  rapidjson::Document const &doc_null = doc_null_non_const;
//
//  std::optional<std::string> str;
//  parse(doc, str);
//  EXPECT_EQ(str, "string");
//
//  parse(doc_null, str);
//  EXPECT_EQ(str, std::nullopt);
//}
//
//TEST(ParserTests, Vector) {
//  using namespace CxxPlugins;
//  const char *json_null = "null";
//  std::vector<unsigned> expected_uint = {1, 2, 3, 4, 5};
//  std::vector<int> expected_int = {-1, 2, -3, 4, -5};
//  std::vector<double> expected_double = {-1, 2, 0.5, 3.4, -5.6};
//  const char *json_array_uint = "[ 1,2,3,4,5]";
//  const char *json_array_int = "[-1,2,-3,4,-5]";
//  const char *json_array_double = "[-1, 2, 0.5, 3.4, -5.6]";
//
//  rapidjson::Document doc_null_non_const;
//  doc_null_non_const.Parse(json_null);
//  rapidjson::Document const &doc_null = doc_null_non_const;
//
//  rapidjson::Document doc_uint_non_const;
//  doc_uint_non_const.Parse(json_array_uint);
//  rapidjson::Document const &doc_uint = doc_uint_non_const;
//
//  rapidjson::Document doc_int_non_const;
//  doc_int_non_const.Parse(json_array_int);
//  rapidjson::Document const &doc_int = doc_int_non_const;
//
//  rapidjson::Document doc_double_non_const;
//  doc_double_non_const.Parse(json_array_double);
//  rapidjson::Document const &doc_double = doc_double_non_const;
//
//  std::vector<unsigned> uvec;
//  EXPECT_THROW({ parse(doc_null, uvec); }, TypeMismatch);
//  std::optional<std::vector<unsigned>> uvec_opt;
//  parse(doc_null, uvec_opt);
//  EXPECT_EQ(uvec_opt, std::nullopt);
//  parse(doc_uint, uvec_opt);
//  EXPECT_EQ(uvec_opt, expected_uint);
//  EXPECT_THROW({ parse(doc_int, uvec_opt); },
//               TypeMismatch);
//  EXPECT_THROW({ parse(doc_double, uvec_opt); },
//               TypeMismatch);
//
//  std::vector<int> ivec;
//  EXPECT_THROW({ parse(doc_null, ivec); }, TypeMismatch);
//  std::optional<std::vector<int>> ivec_opt;
//  parse(doc_null, ivec_opt);
//  EXPECT_EQ(ivec_opt, std::nullopt);
//
//  parse(doc_int, ivec_opt);
//  EXPECT_EQ(ivec_opt.value(), expected_int);
//  EXPECT_THROW({ parse(doc_double, ivec_opt); },
//               TypeMismatch);
//
//  std::vector<double> dvec;
//  EXPECT_THROW({ parse(doc_null, dvec); }, TypeMismatch);
//  std::optional<std::vector<double>> dvec_opt;
//  parse(doc_null, dvec_opt);
//  EXPECT_EQ(dvec_opt, std::nullopt);
//
//  parse(doc_double, dvec_opt);
//  EXPECT_EQ(expected_double, dvec_opt);
//}
//
//TEST(ParserTests, Map) {
//  using namespace CxxPlugins;
//
//  std::map<std::string, int> expected = {{"foo", -4}, {"bar", 3}, {"baz", 2}};
//  std::string json = "{";
//  for (auto &[name, val] : expected) {
//    json += '\"' + name + '\"' + " : " + std::to_string(val) + ",\n";
//  }
//  // remove last comma and \n
//  json.resize(json.size() - 2);
//  json += "\n}";
//
//  rapidjson::Document doc_non_const;
//  doc_non_const.Parse(json.c_str(), json.size());
//  rapidjson::Document const &doc = doc_non_const;
//
//  std::map<std::string, int> result;
//  parse(doc, result);
//
//  EXPECT_EQ(result, expected);
//}
//
//TEST(ParserTests, UnorderedMap) {
//  using namespace CxxPlugins;
//
//  std::unordered_map<std::string, int> expected = {
//      {"foo", -4}, {"bar", 3}, {"baz", 2}};
//  std::string json = "{";
//  for (auto &[name, val] : expected) {
//    json += '\"' + name + '\"' + " : " + std::to_string(val) + ",\n";
//  }
//  // remove last comma and \n
//  json.resize(json.size() - 2);
//  json += "\n}";
//
//  rapidjson::Document doc;
//  doc.Parse(json.c_str(), json.size());
//
//  std::unordered_map<std::string, int> result;
//  parse(doc, result);
//
//  EXPECT_EQ(result, expected);
//}
//
//TEST(ParserTests, Tuple) {
//  using namespace CxxPlugins;
//
//  Tuple<int, double, std::string, std::optional<std::string>> expected = {
//      4, 0.5, "string", std::nullopt};
//
//  const char *json = "[4,0.5,\"string\",null]";
//  rapidjson::Document doc_non_const;
//  doc_non_const.Parse(json);
//  rapidjson::Document const &doc = doc_non_const;
//
//  Tuple<int, double, std::string, std::optional<std::string>> result;
//  parse(doc, result);
//  EXPECT_EQ(expected, result);
//}
//
//TEST(ParserTests, StdTuple) {
//  using namespace CxxPlugins;
//
//  std::tuple<int, double, std::string, std::optional<std::string>> expected = {
//      4, 0.5, "string", std::nullopt};
//
//  const char *json = "[4,0.5,\"string\",null]";
//  rapidjson::Document doc_non_const;
//  doc_non_const.Parse(json);
//  rapidjson::Document const &doc = doc_non_const;
//
//  std::tuple<int, double, std::string, std::optional<std::string>> result;
//  parse(doc, result);
//  EXPECT_EQ(expected, result);
//}
//
//struct my_tag0 {};
//static constexpr my_tag0 my_tag0v = {};
//struct my_tag1 {};
//static constexpr my_tag1 my_tag1v = {};
//struct my_tag2 {};
//static constexpr my_tag2 my_tag2v = {};
//
//TEST(ParserTests, SimpleTupleMap) {
//  using namespace CxxPlugins;
//
//  TupleMap expected{TaggedValue{my_tag0v, 4}, TaggedValue{my_tag1v, 5.0},
//                    TaggedValue{my_tag2v, std::string{"string"}}};
//
//  const char *json = "{"
//                     "\"my_tag0\" : 4,"
//                     "\"my_tag1\" : 5.0,"
//                     "\"my_tag2\" : \"string\""
//                     "}";
//  rapidjson::Document doc_non_const;
//  doc_non_const.Parse(json);
//  rapidjson::Document const &doc = doc_non_const;
//
//  TupleMap<TaggedValue<my_tag0, int>, TaggedValue<my_tag1, double>,
//           TaggedValue<my_tag2, std::string>>
//      result;
//
//  parse(doc, result);
//  EXPECT_EQ(result, expected);
//}
//
//TEST(ParserTests, TupleMapSizeMismatch) {
//  using namespace CxxPlugins;
//
//  const char *json = "{"
//                     "\"my_tag0\" : 4,"
//                     "\"my_tag2\" : \"string\""
//                     "}";
//  rapidjson::Document doc_non_const;
//  doc_non_const.Parse(json);
//  rapidjson::Document const &doc = doc_non_const;
//
//  TupleMap<TaggedValue<my_tag0, int>, TaggedValue<my_tag1, double>,
//           TaggedValue<my_tag2, std::string>>
//      result;
//
//  EXPECT_THROW({ parse(doc, result); },
//               ObjectSizeMismatch);
//}
//
//TEST(ParserTests, TupleMapMissingMember) {
//  using namespace CxxPlugins;
//
//  const char *json = "{"
//                     "\"my_tag0\" : 4,"
//                     "\"my_tag1\" : 5.0,"
//                     "\"unknown_tag\" : null"
//                     "}";
//  rapidjson::Document doc_non_const;
//  doc_non_const.Parse(json);
//  rapidjson::Document const &doc = doc_non_const;
//
//  TupleMap<TaggedValue<my_tag0, int>, TaggedValue<my_tag1, double>,
//           TaggedValue<my_tag2, std::string>>
//      result;
//
//  EXPECT_THROW({ parse(doc, result); },
//               ObjectMemberMissing);
//}
//
//TEST(ParserTests, OptionalTupleMap) {
//  using namespace CxxPlugins;
//
//  TupleMap<TaggedValue<my_tag0, std::optional<int>>,
//           TaggedValue<my_tag1, std::optional<double>>,
//           TaggedValue<my_tag2, std::optional<std::string>>>
//      expected{std::nullopt, std::nullopt, "string"};
//
//  const char *json = "{"
//                     //                     "\"my_tag0\" : 4,"
//                     //                     "\"my_tag1\" : 5.0,"
//                     "\"my_tag2\" : \"string\""
//                     "}";
//  rapidjson::Document doc_non_const;
//  doc_non_const.Parse(json);
//  rapidjson::Document const &doc = doc_non_const;
//
//  TupleMap<TaggedValue<my_tag0, std::optional<int>>,
//           TaggedValue<my_tag1, std::optional<double>>,
//           TaggedValue<my_tag2, std::optional<std::string>>>
//      result;
//
//  parse(doc, result);
//  EXPECT_EQ(result, expected);
//}
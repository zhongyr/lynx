// Copyright 2025 The Lynx Authors. All rights reserved.
// Licensed under the Apache License Version 2.0 that can be found in the
// LICENSE file in the root directory of this source tree.

#include <algorithm>
#include <map>
#include <memory>
#include <random>
#include <string>
#include <unordered_map>

#include "base/include/vector.h"
#include "core/runtime/vm/lepus/lepus_value.h"
#include "core/runtime/vm/lepus/table.h"
#include "third_party/benchmark/include/benchmark/benchmark.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"

using namespace std;          // NOLINT
using namespace lynx::base;   // NOLINT
using namespace lynx::lepus;  // NOLINT

namespace lynx {
namespace base {

// Test template codec ranges.
struct Range {
  uint32_t start;
  uint32_t end;
};

namespace {
template <class T>
T Cast(size_t v);

template <>
int Cast(size_t v) {
  return (int)v;
}

template <>
Range Cast(size_t v) {
  return {static_cast<uint32_t>(v), static_cast<uint32_t>(v)};
}

template <>
string Cast(size_t v) {
  if (v % 2 == 0) {
    return "string_" + to_string(v);
  } else {
    return to_string(v) + "string_";
  }
}

template <>
shared_ptr<string> Cast(size_t v) {
  return make_shared<string>(to_string(v));
}

template <>
String Cast(size_t v) {
  if (v % 2 == 0) {
    return String(std::string("string_") + to_string(v));
  } else {
    return String(to_string(v) + std::string("string_"));
  }
}

template <>
Value Cast(size_t v) {
  if (v % 3 == 1) {
    return Value(std::to_string(v));
  } else if (v % 3 == 2) {
    return Value(lepus::Dictionary::Create());
  } else {
    return Value((int32_t)v);
  }
}
}  // namespace

#define STRINGIFY(s) #s
#define FOREACH_STRINGIFY_1(a) #a
#define FOREACH_STRINGIFY_2(a, ...) #a ", " FOREACH_STRINGIFY_1(__VA_ARGS__)
#define FOREACH_STRINGIFY_3(a, ...) #a ", " FOREACH_STRINGIFY_2(__VA_ARGS__)
#define FOREACH_STRINGIFY_4(a, ...) #a ", " FOREACH_STRINGIFY_3(__VA_ARGS__)
#define FOREACH_STRINGIFY_N(_4, _3, _2, _1, N, ...) FOREACH_STRINGIFY##N
#define FOREACH_STRINGIFY(...)                     \
  FOREACH_STRINGIFY_N(__VA_ARGS__, _4, _3, _2, _1) \
  (__VA_ARGS__)

#define TEST_FUNC_MAP_INSERT(TYPE, DATA_COUNT, MAP, ...)                   \
  static void BM_##MAP##_insert_##DATA_COUNT##_##TYPE(                     \
      benchmark::State& state) {                                           \
    state.SetLabel(STRINGIFY(MAP) "<" FOREACH_STRINGIFY(__VA_ARGS__) ">"); \
    using MapType = MAP<__VA_ARGS__>;                                      \
    constexpr size_t kDataCount = DATA_COUNT;                              \
    vector<pair<MapType::key_type, MapType::mapped_type>> data;            \
                                                                           \
    /* Generate data. Keys should not be ordered. */                       \
    int data_index = 0;                                                    \
    data.resize(kDataCount);                                               \
    for (size_t i = 0; i < kDataCount / 2; i++) {                          \
      data[data_index].first = Cast<MapType::key_type>(i);                 \
      data[data_index++].second = Cast<MapType::mapped_type>(i);           \
    }                                                                      \
    for (size_t i = kDataCount - 1; i >= kDataCount / 2; i--) {            \
      data[data_index].first = Cast<MapType::key_type>(i);                 \
      data[data_index++].second = Cast<MapType::mapped_type>(i);           \
    }                                                                      \
                                                                           \
    size_t total = 0;                                                      \
    for (auto _ : state) {                                                 \
      for (int i = 0; i < 10; i++) {                                       \
        MapType map;                                                       \
        for (auto it = data.begin(); it != data.end(); it++) {             \
          map[it->first] = it->second;                                     \
        }                                                                  \
        total += map.size();                                               \
      }                                                                    \
    }                                                                      \
  }

#define TEST_FUNC_MAP_FIND(TYPE, DATA_COUNT, NOT_FOUND_COUNT, MAP, ...)        \
  static void BM_##MAP##_find_##DATA_COUNT##_##TYPE(benchmark::State& state) { \
    constexpr size_t kDataCount = DATA_COUNT;                                  \
    constexpr size_t kNotFoundCount = NOT_FOUND_COUNT;                         \
    state.SetLabel(STRINGIFY(MAP) "<" FOREACH_STRINGIFY(__VA_ARGS__) ">");     \
    using MapType = MAP<__VA_ARGS__>;                                          \
    vector<pair<MapType::key_type, MapType::mapped_type>> data;                \
    vector<pair<MapType::key_type, MapType::mapped_type>> find_data;           \
                                                                               \
    /* Generate data.  */                                                      \
    data.resize(kDataCount);                                                   \
    find_data.resize(kDataCount + kNotFoundCount);                             \
    for (size_t i = 0; i < kDataCount; i++) {                                  \
      data[i].first = Cast<MapType::key_type>(i);                              \
      data[i].second = Cast<MapType::mapped_type>(i);                          \
      find_data[i].first = Cast<MapType::key_type>(i);                         \
      find_data[i].second = Cast<MapType::mapped_type>(i);                     \
    }                                                                          \
                                                                               \
    for (size_t i = 0; i < kNotFoundCount; i++) {                              \
      find_data[kDataCount + i].first =                                        \
          Cast<MapType::key_type>(kDataCount + i);                             \
    }                                                                          \
                                                                               \
    MapType map;                                                               \
    auto rng = std::default_random_engine{};                                   \
    std::shuffle(std::begin(data), std::end(data), rng);                       \
    for (auto it = data.begin(); it != data.end(); it++) {                     \
      map[it->first] = it->second;                                             \
    }                                                                          \
                                                                               \
    size_t total = 0;                                                          \
    for (auto _ : state) {                                                     \
      for (auto it = find_data.begin(); it != find_data.end(); it++) {         \
        if (map.find(it->first) != map.end()) {                                \
          total++;                                                             \
        }                                                                      \
      }                                                                        \
    }                                                                          \
  }

#define TEST_MAP_INSERT_II(DATA_COUNT, MAP, ...)         \
  TEST_FUNC_MAP_INSERT(II, DATA_COUNT, MAP, __VA_ARGS__) \
  BENCHMARK(BM_##MAP##_insert_##DATA_COUNT##_II)

#define TEST_MAP_INSERT_IR(DATA_COUNT, MAP, ...)         \
  TEST_FUNC_MAP_INSERT(IR, DATA_COUNT, MAP, __VA_ARGS__) \
  BENCHMARK(BM_##MAP##_insert_##DATA_COUNT##_IR)

#define TEST_MAP_INSERT_ss(DATA_COUNT, MAP, ...)         \
  TEST_FUNC_MAP_INSERT(ss, DATA_COUNT, MAP, __VA_ARGS__) \
  BENCHMARK(BM_##MAP##_insert_##DATA_COUNT##_ss)

#define TEST_MAP_INSERT_ISP(DATA_COUNT, MAP, ...)         \
  TEST_FUNC_MAP_INSERT(ISP, DATA_COUNT, MAP, __VA_ARGS__) \
  BENCHMARK(BM_##MAP##_insert_##DATA_COUNT##_ISP)

#define TEST_MAP_INSERT_sSP(DATA_COUNT, MAP, ...)         \
  TEST_FUNC_MAP_INSERT(sSP, DATA_COUNT, MAP, __VA_ARGS__) \
  BENCHMARK(BM_##MAP##_insert_##DATA_COUNT##_sSP)

#define TEST_MAP_INSERT_SV(DATA_COUNT, MAP, ...)         \
  TEST_FUNC_MAP_INSERT(SV, DATA_COUNT, MAP, __VA_ARGS__) \
  BENCHMARK(BM_##MAP##_insert_##DATA_COUNT##_SV)

#define TEST_MAP_INSERT_IV(DATA_COUNT, MAP, ...)         \
  TEST_FUNC_MAP_INSERT(IV, DATA_COUNT, MAP, __VA_ARGS__) \
  BENCHMARK(BM_##MAP##_insert_##DATA_COUNT##_IV)

#define TEST_MAP_INSERT_SS(DATA_COUNT, MAP, ...)         \
  TEST_FUNC_MAP_INSERT(SS, DATA_COUNT, MAP, __VA_ARGS__) \
  BENCHMARK(BM_##MAP##_insert_##DATA_COUNT##_SS)

/**
 * <int, int>
 * data count
 *   0~48: LinearFlatMap best
 *   48~2048: OrderedFlatMap best
 *   > 2048: std::unordered_map best
 */
TEST_MAP_INSERT_II(8, map, int, int);
TEST_MAP_INSERT_II(8, unordered_map, int, int);
TEST_MAP_INSERT_II(8, OrderedFlatMap, int, int);
TEST_MAP_INSERT_II(8, InlineOrderedFlatMap, int, int, 8);
TEST_MAP_INSERT_II(8, LinearFlatMap, int, int);
TEST_MAP_INSERT_II(8, InlineLinearFlatMap, int, int, 8);

TEST_MAP_INSERT_II(2048, map, int, int);
TEST_MAP_INSERT_II(2048, unordered_map, int, int);
TEST_MAP_INSERT_II(2048, OrderedFlatMap, int, int);
TEST_MAP_INSERT_II(2048, InlineOrderedFlatMap, int, int, 2048);

TEST_MAP_INSERT_II(48, OrderedFlatMap, int, int);
TEST_MAP_INSERT_II(48, InlineOrderedFlatMap, int, int, 48);
TEST_MAP_INSERT_II(48, LinearFlatMap, int, int);
TEST_MAP_INSERT_II(48, InlineLinearFlatMap, int, int, 48);

/**
 * <int, Range>
 * data count
 *   0~72: LinearFlatMap best
 *   72~1300: OrderedFlatMap best
 *   > 1300: std::unordered_map best
 */
TEST_MAP_INSERT_IR(8, map, int, Range);
TEST_MAP_INSERT_IR(8, unordered_map, int, Range);
TEST_MAP_INSERT_IR(8, OrderedFlatMap, int, Range);
TEST_MAP_INSERT_IR(8, InlineOrderedFlatMap, int, Range, 8);
TEST_MAP_INSERT_IR(8, LinearFlatMap, int, Range);
TEST_MAP_INSERT_IR(8, InlineLinearFlatMap, int, Range, 8);

TEST_MAP_INSERT_IR(1300, map, int, Range);
TEST_MAP_INSERT_IR(1300, unordered_map, int, Range);
TEST_MAP_INSERT_IR(1300, OrderedFlatMap, int, Range);
TEST_MAP_INSERT_IR(1300, InlineOrderedFlatMap, int, Range, 1300);

TEST_MAP_INSERT_IR(72, OrderedFlatMap, int, Range);
TEST_MAP_INSERT_IR(72, InlineOrderedFlatMap, int, Range, 72);
TEST_MAP_INSERT_IR(72, LinearFlatMap, int, Range);
TEST_MAP_INSERT_IR(72, InlineLinearFlatMap, int, Range, 72);

/**
 * <std::string, std::string>
 * data count
 *   0~42: LinearFlatMap best
 *   =24: std::unordered_map == OrderedFlatMap
 *   > 42: std::unordered_map best
 */
TEST_MAP_INSERT_ss(8, map, string, string);
TEST_MAP_INSERT_ss(8, unordered_map, string, string);
TEST_MAP_INSERT_ss(8, OrderedFlatMap, string, string);
TEST_MAP_INSERT_ss(8, InlineOrderedFlatMap, string, string, 8);
TEST_MAP_INSERT_ss(8, LinearFlatMap, string, string);
TEST_MAP_INSERT_ss(8, InlineLinearFlatMap, string, string, 8);

TEST_MAP_INSERT_ss(24, map, string, string);
TEST_MAP_INSERT_ss(24, unordered_map, string, string);
TEST_MAP_INSERT_ss(24, OrderedFlatMap, string, string);
TEST_MAP_INSERT_ss(24, InlineOrderedFlatMap, string, string, 24);
TEST_MAP_INSERT_ss(24, LinearFlatMap, string, string);
TEST_MAP_INSERT_ss(24, InlineLinearFlatMap, string, string, 24);

TEST_MAP_INSERT_ss(32, map, string, string);
TEST_MAP_INSERT_ss(32, unordered_map, string, string);
TEST_MAP_INSERT_ss(32, OrderedFlatMap, string, string);
TEST_MAP_INSERT_ss(32, InlineOrderedFlatMap, string, string, 32);
TEST_MAP_INSERT_ss(32, LinearFlatMap, string, string);
TEST_MAP_INSERT_ss(32, InlineLinearFlatMap, string, string, 32);

TEST_MAP_INSERT_ss(42, map, string, string);
TEST_MAP_INSERT_ss(42, unordered_map, string, string);
TEST_MAP_INSERT_ss(42, OrderedFlatMap, string, string);
TEST_MAP_INSERT_ss(42, InlineOrderedFlatMap, string, string, 42);
TEST_MAP_INSERT_ss(42, LinearFlatMap, string, string);
TEST_MAP_INSERT_ss(42, InlineLinearFlatMap, string, string, 42);

/**
 * <int, std::shared_ptr<string>>
 * data count
 *   0~270: LinearFlatMap best
 *   =68: std::unordered_map == OrderedFlatMap
 *   > 270: std::unordered_map best
 */
TEST_MAP_INSERT_ISP(8, map, int, shared_ptr<string>);
TEST_MAP_INSERT_ISP(8, unordered_map, int, shared_ptr<string>);
TEST_MAP_INSERT_ISP(8, OrderedFlatMap, int, shared_ptr<string>);
TEST_MAP_INSERT_ISP(8, InlineOrderedFlatMap, int, shared_ptr<string>, 8);
TEST_MAP_INSERT_ISP(8, LinearFlatMap, int, shared_ptr<string>);
TEST_MAP_INSERT_ISP(8, InlineLinearFlatMap, int, shared_ptr<string>, 8);

TEST_MAP_INSERT_ISP(68, map, int, shared_ptr<string>);
TEST_MAP_INSERT_ISP(68, unordered_map, int, shared_ptr<string>);
TEST_MAP_INSERT_ISP(68, OrderedFlatMap, int, shared_ptr<string>);
TEST_MAP_INSERT_ISP(68, InlineOrderedFlatMap, int, shared_ptr<string>, 68);
TEST_MAP_INSERT_ISP(68, LinearFlatMap, int, shared_ptr<string>);
TEST_MAP_INSERT_ISP(68, InlineLinearFlatMap, int, shared_ptr<string>, 68);

TEST_MAP_INSERT_ISP(128, map, int, shared_ptr<string>);
TEST_MAP_INSERT_ISP(128, unordered_map, int, shared_ptr<string>);
TEST_MAP_INSERT_ISP(128, OrderedFlatMap, int, shared_ptr<string>);
TEST_MAP_INSERT_ISP(128, InlineOrderedFlatMap, int, shared_ptr<string>, 128);
TEST_MAP_INSERT_ISP(128, LinearFlatMap, int, shared_ptr<string>);
TEST_MAP_INSERT_ISP(128, InlineLinearFlatMap, int, shared_ptr<string>, 128);

TEST_MAP_INSERT_ISP(270, map, int, shared_ptr<string>);
TEST_MAP_INSERT_ISP(270, unordered_map, int, shared_ptr<string>);
TEST_MAP_INSERT_ISP(270, OrderedFlatMap, int, shared_ptr<string>);
TEST_MAP_INSERT_ISP(270, InlineOrderedFlatMap, int, shared_ptr<string>, 270);
TEST_MAP_INSERT_ISP(270, LinearFlatMap, int, shared_ptr<string>);
TEST_MAP_INSERT_ISP(270, InlineLinearFlatMap, int, shared_ptr<string>, 270);

/**
 * <std::string, std::shared_ptr<string>>
 * data count
 *   0~50: LinearFlatMap best
 *   > 50: std::unordered_map best
 */
TEST_MAP_INSERT_sSP(8, map, string, shared_ptr<string>);
TEST_MAP_INSERT_sSP(8, unordered_map, string, shared_ptr<string>);
TEST_MAP_INSERT_sSP(8, OrderedFlatMap, string, shared_ptr<string>);
TEST_MAP_INSERT_sSP(8, InlineOrderedFlatMap, string, shared_ptr<string>, 8);
TEST_MAP_INSERT_sSP(8, LinearFlatMap, string, shared_ptr<string>);
TEST_MAP_INSERT_sSP(8, InlineLinearFlatMap, string, shared_ptr<string>, 8);

TEST_MAP_INSERT_sSP(50, map, string, shared_ptr<string>);
TEST_MAP_INSERT_sSP(50, unordered_map, string, shared_ptr<string>);
TEST_MAP_INSERT_sSP(50, OrderedFlatMap, string, shared_ptr<string>);
TEST_MAP_INSERT_sSP(50, InlineOrderedFlatMap, string, shared_ptr<string>, 50);
TEST_MAP_INSERT_sSP(50, LinearFlatMap, string, shared_ptr<string>);
TEST_MAP_INSERT_sSP(50, InlineLinearFlatMap, string, shared_ptr<string>, 50);

/**
 * <lepus::String, lepus::Value>
 * data count
 *   0~66: LinearFlatMap best
 *   =12: std::unordered_map == OrderedFlatMap
 *   > 66: std::unordered_map best
 */
TEST_MAP_INSERT_SV(8, map, String, Value);
TEST_MAP_INSERT_SV(8, unordered_map, String, Value);
TEST_MAP_INSERT_SV(8, OrderedFlatMap, String, Value);
TEST_MAP_INSERT_SV(8, InlineOrderedFlatMap, String, Value, 8);
TEST_MAP_INSERT_SV(8, LinearFlatMap, String, Value);
TEST_MAP_INSERT_SV(8, InlineLinearFlatMap, String, Value, 8);

TEST_MAP_INSERT_SV(12, map, String, Value);
TEST_MAP_INSERT_SV(12, unordered_map, String, Value);
TEST_MAP_INSERT_SV(12, OrderedFlatMap, String, Value);
TEST_MAP_INSERT_SV(12, InlineOrderedFlatMap, String, Value, 12);
TEST_MAP_INSERT_SV(12, LinearFlatMap, String, Value);
TEST_MAP_INSERT_SV(12, InlineLinearFlatMap, String, Value, 12);

TEST_MAP_INSERT_SV(24, map, String, Value);
TEST_MAP_INSERT_SV(24, unordered_map, String, Value);
TEST_MAP_INSERT_SV(24, OrderedFlatMap, String, Value);
TEST_MAP_INSERT_SV(24, InlineOrderedFlatMap, String, Value, 24);
TEST_MAP_INSERT_SV(24, LinearFlatMap, String, Value);
TEST_MAP_INSERT_SV(24, InlineLinearFlatMap, String, Value, 24);

TEST_MAP_INSERT_SV(66, map, String, Value);
TEST_MAP_INSERT_SV(66, unordered_map, String, Value);
TEST_MAP_INSERT_SV(66, OrderedFlatMap, String, Value);
TEST_MAP_INSERT_SV(66, InlineOrderedFlatMap, String, Value, 66);
TEST_MAP_INSERT_SV(66, LinearFlatMap, String, Value);
TEST_MAP_INSERT_SV(66, InlineLinearFlatMap, String, Value, 66);

/**
 * <int, lepus::Value>
 * data count
 *   0~256: LinearFlatMap best
 *   =36: std::unordered_map == OrderedFlatMap
 *   > 256: std::unordered_map best
 */
TEST_MAP_INSERT_IV(8, map, int, Value);
TEST_MAP_INSERT_IV(8, unordered_map, int, Value);
TEST_MAP_INSERT_IV(8, OrderedFlatMap, int, Value);
TEST_MAP_INSERT_IV(8, InlineOrderedFlatMap, int, Value, 8);
TEST_MAP_INSERT_IV(8, LinearFlatMap, int, Value);
TEST_MAP_INSERT_IV(8, InlineLinearFlatMap, int, Value, 8);

TEST_MAP_INSERT_IV(18, map, int, Value);
TEST_MAP_INSERT_IV(18, unordered_map, int, Value);
TEST_MAP_INSERT_IV(18, OrderedFlatMap, int, Value);
TEST_MAP_INSERT_IV(18, InlineOrderedFlatMap, int, Value, 18);
TEST_MAP_INSERT_IV(18, LinearFlatMap, int, Value);
TEST_MAP_INSERT_IV(18, InlineLinearFlatMap, int, Value, 18);

TEST_MAP_INSERT_IV(36, map, int, Value);
TEST_MAP_INSERT_IV(36, unordered_map, int, Value);
TEST_MAP_INSERT_IV(36, OrderedFlatMap, int, Value);
TEST_MAP_INSERT_IV(36, InlineOrderedFlatMap, int, Value, 36);
TEST_MAP_INSERT_IV(36, LinearFlatMap, int, Value);
TEST_MAP_INSERT_IV(36, InlineLinearFlatMap, int, Value, 36);

TEST_MAP_INSERT_IV(256, map, int, Value);
TEST_MAP_INSERT_IV(256, unordered_map, int, Value);
TEST_MAP_INSERT_IV(256, OrderedFlatMap, int, Value);
TEST_MAP_INSERT_IV(256, InlineOrderedFlatMap, int, Value, 256);
TEST_MAP_INSERT_IV(256, LinearFlatMap, int, Value);
TEST_MAP_INSERT_IV(256, InlineLinearFlatMap, int, Value, 256);

/**
 * <lepus::String, lepus::String>
 * data count
 *   0~80: LinearFlatMap best
 *   =24: std::unordered_map == OrderedFlatMap
 *   > 80: std::unordered_map best
 */
TEST_MAP_INSERT_SS(8, map, String, String);
TEST_MAP_INSERT_SS(8, unordered_map, String, String);
TEST_MAP_INSERT_SS(8, OrderedFlatMap, String, String);
TEST_MAP_INSERT_SS(8, InlineOrderedFlatMap, String, String, 8);
TEST_MAP_INSERT_SS(8, LinearFlatMap, String, String);
TEST_MAP_INSERT_SS(8, InlineLinearFlatMap, String, String, 8);

TEST_MAP_INSERT_SS(24, map, String, String);
TEST_MAP_INSERT_SS(24, unordered_map, String, String);
TEST_MAP_INSERT_SS(24, OrderedFlatMap, String, String);
TEST_MAP_INSERT_SS(24, InlineOrderedFlatMap, String, String, 24);
TEST_MAP_INSERT_SS(24, LinearFlatMap, String, String);
TEST_MAP_INSERT_SS(24, InlineLinearFlatMap, String, String, 24);

TEST_MAP_INSERT_SS(80, map, String, String);
TEST_MAP_INSERT_SS(80, unordered_map, String, String);
TEST_MAP_INSERT_SS(80, OrderedFlatMap, String, String);
TEST_MAP_INSERT_SS(80, InlineOrderedFlatMap, String, String, 80);
TEST_MAP_INSERT_SS(80, LinearFlatMap, String, String);
TEST_MAP_INSERT_SS(80, InlineLinearFlatMap, String, String, 80);

#define TEST_MAP_FIND_I(DATA_COUNT, NOT_FOUND_COUNT, MAP, ...)         \
  TEST_FUNC_MAP_FIND(I, DATA_COUNT, NOT_FOUND_COUNT, MAP, __VA_ARGS__) \
  BENCHMARK(BM_##MAP##_find_##DATA_COUNT##_I)

#define TEST_MAP_FIND_s(DATA_COUNT, NOT_FOUND_COUNT, MAP, ...)         \
  TEST_FUNC_MAP_FIND(s, DATA_COUNT, NOT_FOUND_COUNT, MAP, __VA_ARGS__) \
  BENCHMARK(BM_##MAP##_find_##DATA_COUNT##_s)

#define TEST_MAP_FIND_S(DATA_COUNT, NOT_FOUND_COUNT, MAP, ...)         \
  TEST_FUNC_MAP_FIND(S, DATA_COUNT, NOT_FOUND_COUNT, MAP, __VA_ARGS__) \
  BENCHMARK(BM_##MAP##_find_##DATA_COUNT##_S)

TEST_MAP_FIND_I(2, 1, map, int, int);
TEST_MAP_FIND_I(2, 1, unordered_map, int, int);  // std::unordered_map is faster
TEST_MAP_FIND_I(2, 1, OrderedFlatMap, int, int);
TEST_MAP_FIND_I(2, 1, LinearFlatMap, int, int);

TEST_MAP_FIND_I(12, 2, map, int, int);
TEST_MAP_FIND_I(12, 2, unordered_map, int, int);
TEST_MAP_FIND_I(12, 2, OrderedFlatMap, int, int);
TEST_MAP_FIND_I(12, 2, LinearFlatMap, int,
                int);  // 12 equivalent to OrderedFlatMap

TEST_MAP_FIND_I(128, 16, map, int, int);
TEST_MAP_FIND_I(128, 16, unordered_map, int, int);
TEST_MAP_FIND_I(128, 16, OrderedFlatMap, int, int);
TEST_MAP_FIND_I(128, 16, LinearFlatMap, int, int);

TEST_MAP_FIND_s(3, 1, map, string, string);
TEST_MAP_FIND_s(3, 1, unordered_map, string, string);
TEST_MAP_FIND_s(3, 1, OrderedFlatMap, string, string);
TEST_MAP_FIND_s(
    3, 1, LinearFlatMap, string,
    string);  // equivalent to std::unordered_map, faster than OrderedFlatMap

// If most keys share the same prefix substring, LinearFlatMap scores would be
// worse
TEST_MAP_FIND_s(30, 4, map, string, string);
TEST_MAP_FIND_s(30, 4, unordered_map, string, string);
TEST_MAP_FIND_s(30, 4, OrderedFlatMap, string, string);
TEST_MAP_FIND_s(
    30, 4, LinearFlatMap, string,
    string);  // 30 equivalent to ordered, less than 30 LinearFlatMap is faster

TEST_MAP_FIND_S(2, 1, map, String, String);
TEST_MAP_FIND_S(2, 1, unordered_map, String, String);
TEST_MAP_FIND_S(2, 1, OrderedFlatMap, String, String);
TEST_MAP_FIND_S(
    2, 1, LinearFlatMap, String,
    String);  // equivalent to std::unordered_map, faster than OrderedFlatMap

TEST_MAP_FIND_S(6, 1, map, String, String);
TEST_MAP_FIND_S(6, 1, unordered_map, String, String);
TEST_MAP_FIND_S(6, 1, OrderedFlatMap, String, String);
TEST_MAP_FIND_S(6, 1, LinearFlatMap, String,
                String);  // a little slower than std::unordered_map, faster
                          // than OrderedFlatMap

// If most keys share the same prefix substring, LinearFlatMap scores would be
// worse
TEST_MAP_FIND_S(80, 4, map, String, String);
TEST_MAP_FIND_S(80, 4, unordered_map, String, String);
TEST_MAP_FIND_S(80, 4, OrderedFlatMap, String, String);
TEST_MAP_FIND_S(
    80, 4, LinearFlatMap, String,
    String);  // 80 equivalent to ordered, less than 80 LinearFlatMap is faster
}  // namespace base
}  // namespace lynx

#pragma clang diagnostic pop

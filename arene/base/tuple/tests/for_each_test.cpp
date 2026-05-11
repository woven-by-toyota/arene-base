// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/tuple/for_each.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/pair.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/stdlib_choice/tuple_size.hpp"

namespace {

using ::arene::base::for_each;
using ::arene::base::for_each_index;

/// @test `for_each` correctly loops through tuple values.
TEST(TupleForEachTest, LoopsThroughTupleValuesCorrectly) {
  constexpr std::int32_t int_val = 7324;
  constexpr float float_val = 654.32F;
  constexpr std::uint16_t ushort_val = 344U;
  std::tuple<std::int32_t, float, std::uint16_t> tup{int_val, float_val, ushort_val};
  std::size_t idx = 0UL;
  for_each(tup, [&tup, &idx](auto&& element) {
    using type = typename std::remove_reference_t<decltype(element)>;
    ASSERT_EQ(&std::get<type>(tup), &element);
    ++idx;
  });
  ASSERT_EQ(idx, std::tuple_size<decltype(tup)>());
}

/// @test `for_each` correctly loops through pair values.
TEST(TupleForEachTest, LoopsThroughPairValuesCorrectly) {
  constexpr std::int32_t int_val = 7324;
  constexpr float float_val = 654.32F;
  std::pair<std::int32_t, float> pair{int_val, float_val};
  std::size_t idx = 0UL;
  for_each(pair, [&pair, &idx](auto&& element) {
    using type = typename std::remove_reference_t<decltype(element)>;
    ASSERT_EQ(&std::get<type>(pair), &element);
    ++idx;
  });
  ASSERT_EQ(idx, 2);
}

/// @test `for_each` correctly loops through array values.
TEST(TupleForEachTest, LoopsThroughArrayValuesCorrectly) {
  arene::base::array<std::int32_t, 5UL> arr{8, 9, 4, 3, 2};
  std::size_t idx = 0UL;
  for_each(arr, [&idx, &arr](auto&& element) {
    ASSERT_EQ(arr[idx], element);
    ++idx;
  });
  ASSERT_EQ(idx, arr.size());
}

/// @test `for_each_index` correctly loops through tuple values with index.
TEST(TupleForEachIndexTest, LoopsThroughTupleValuesWithIndexCorrectly) {
  constexpr std::int32_t int_val = 73278;
  constexpr float float_val = 6324.32F;
  constexpr std::uint16_t ushort_val = 142U;
  std::tuple<std::int32_t, float, std::uint16_t> tup{int_val, float_val, ushort_val};
  std::size_t idx = 0UL;
  for_each_index(tup, [&idx, &tup](std::size_t idx_val, auto&& element) {
    ASSERT_EQ(idx, idx_val);
    using type = typename std::remove_reference_t<decltype(element)>;
    ASSERT_EQ(&std::get<type>(tup), &element);
    ++idx;
  });
  ASSERT_EQ(idx, std::tuple_size<decltype(tup)>());
}

/// @test `for_each_index` correctly loops through pair values with index.
TEST(TupleForEachIndexTest, LoopsThroughPairValuesWithIndexCorrectly) {
  constexpr std::int32_t int_val = 7324;
  constexpr float float_val = 654.32F;
  std::pair<std::int32_t, float> pair{int_val, float_val};
  std::size_t idx = 0UL;
  for_each_index(pair, [&pair, &idx](std::size_t idx_val, auto&& element) {
    ASSERT_EQ(idx, idx_val);
    using type = typename std::remove_reference_t<decltype(element)>;
    ASSERT_EQ(&std::get<type>(pair), &element);
    ++idx;
  });
  ASSERT_EQ(idx, 2);
}

/// @test `for_each_index` correctly loops through array values with index.
TEST(TupleForEachIndexTest, LoopsThroughArrayValuesWithIndexCorrectly) {
  arene::base::array<std::int32_t, 5UL> arr{8, 9, 4, 3, 2};
  std::size_t idx = 0UL;
  for_each_index(arr, [&idx, &arr](std::size_t idx_val, auto&& element) {
    ASSERT_EQ(idx, idx_val);
    ASSERT_EQ(arr[idx_val], element);
    ++idx;
  });
  ASSERT_GT(idx, arr.size() - 1);
}
}  // namespace

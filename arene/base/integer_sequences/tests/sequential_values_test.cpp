// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/array.hpp"
#include "arene/base/compare/compare_three_way.hpp"
#include "arene/base/integer_sequences.hpp"
#include "arene/base/iterator/next.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/max_element.hpp"
#include "arene/base/stdlib_choice/min_element.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_swappable.hpp"

namespace {

using ::arene::base::sequential_values_detail::is_valid_size_v;
using ::arene::base::sequential_values_detail::make_index_sequence_array;
using ::arene::base::sequential_values_detail::max_number_of_elements;

using ::arene::base::array;
using ::arene::base::sequential_values;
using ::arene::base::sequential_values_between;
using ::arene::base::sequential_values_from;

/// @test `is_valid_size_v` is true for values up to `max_number_of_elements`.
TEST(IsValidSizeV, IsTrueForValuesUpToMaxNumberOfElements) {
  STATIC_ASSERT_TRUE(is_valid_size_v<0>);
  STATIC_ASSERT_TRUE(is_valid_size_v<10>);
  STATIC_ASSERT_TRUE(is_valid_size_v<256>);
  STATIC_ASSERT_TRUE(is_valid_size_v<max_number_of_elements - 1>);
  STATIC_ASSERT_TRUE(is_valid_size_v<max_number_of_elements>);
}

/// @test `is_valid_size_v` is false for values greater than `max_number_of_elements`.
TEST(IsValidSizeV, IsFalseForValuesGreaterThanMaxNumberOfElements) {
  STATIC_ASSERT_FALSE(is_valid_size_v<max_number_of_elements + 1>);
  STATIC_ASSERT_FALSE(is_valid_size_v<max_number_of_elements + 10>);
}

/// A simple wrapper around a value so we can use typed tests for integral value template params
template <std::size_t Value>
using count = std::integral_constant<decltype(Value), Value>;

/// The set of counts to test across
using valid_counts = ::testing::Types<
    count<1>,                      // single element list
    count<10>,                     // multi element list
    count<256>,                    // 2^8 size list
    count<max_number_of_elements>  // 2^16-1 size list, max size for a static array on some platforms
    >;

template <typename Count>
class MakeIndexSequenceArrayTest : public ::testing::Test {};

TYPED_TEST_SUITE(MakeIndexSequenceArrayTest, valid_counts, );

/// @test `make_index_sequence_array` returns arrays of any size up to `max_number_of_elements`.
TYPED_TEST(MakeIndexSequenceArrayTest, ReturnsArrayOfSizeTOfSizeCount) {
  constexpr auto count = TypeParam{}();
  auto values = make_index_sequence_array(std::make_integer_sequence<typename TypeParam::value_type, count>());
  ::testing::StaticAssertTypeEq<decltype(values), array<typename TypeParam::value_type, count>>();
}

/// @test The minimum index in `make_index_sequence_array` is always zero.
TYPED_TEST(MakeIndexSequenceArrayTest, MinIndexIsZero) {
  constexpr auto count = TypeParam{}();
  auto values = make_index_sequence_array(std::make_integer_sequence<typename TypeParam::value_type, count>());
  EXPECT_EQ(*std::min_element(values.begin(), values.end()), 0);
}

/// @test The maximum index in `make_index_sequence_array` is always the array size - 1.
TYPED_TEST(MakeIndexSequenceArrayTest, MaxIndexIsCountMinusOne) {
  constexpr auto count = TypeParam{}();
  auto values = make_index_sequence_array(std::make_integer_sequence<typename TypeParam::value_type, count>());
  EXPECT_EQ(*std::max_element(values.begin(), values.end()), count - 1);
}

/// @test Elements in `make_index_sequence_array` are sequential.
TYPED_TEST(MakeIndexSequenceArrayTest, ElementsInArrayAreSequential) {
  constexpr auto count = TypeParam{}();
  auto values = make_index_sequence_array(std::make_integer_sequence<typename TypeParam::value_type, count>());
  std::size_t last_idx = 0;
  for (std::size_t current_idx = 1; current_idx < values.size(); ++current_idx, ++last_idx) {
    EXPECT_EQ(values[current_idx] - values[last_idx], 1);
  }
}

/// @test `make_index_sequence_array` returns an empty array for a size of 0.
TEST(EmptyMakeIndexSequenceArrayTest, IsEmpty) {
  STATIC_ASSERT_TRUE((make_index_sequence_array<std::size_t>(std::make_index_sequence<0>()).empty()));
}

template <typename Count>
class SequentialValuesTest : public ::testing::Test {};

TYPED_TEST_SUITE(SequentialValuesTest, valid_counts, );

/// @test `sequential_values` returns arrays of any size up to `max_number_of_elements`.
TYPED_TEST(SequentialValuesTest, ReturnsArrayOfSizeTOfSizeCount) {
  constexpr auto count = TypeParam{}();
  auto values = sequential_values<typename TypeParam::value_type, count>;
  ::testing::StaticAssertTypeEq<decltype(values), array<typename TypeParam::value_type, count>>();
}

/// @test The minimum index in `sequential_values` is always zero.
TYPED_TEST(SequentialValuesTest, MinIndexIsZero) {
  constexpr auto count = TypeParam{}();
  auto const values = sequential_values<typename TypeParam::value_type, count>;
  EXPECT_EQ(*std::min_element(values.begin(), values.end()), 0);
}

/// @test The maximum index in `sequential_values` is always the array size - 1.
TYPED_TEST(SequentialValuesTest, MaxIndexIsCountMinusOne) {
  constexpr auto count = TypeParam{}();
  auto const values = sequential_values<typename TypeParam::value_type, count>;
  EXPECT_EQ(*std::max_element(values.begin(), values.end()), count - 1);
}

/// @test Elements in `sequential_values` are sequential.
TYPED_TEST(SequentialValuesTest, ElementsInArrayAreSequential) {
  constexpr auto count = TypeParam{}();
  auto const values = sequential_values<typename TypeParam::value_type, count>;
  std::size_t last_idx = 0;
  for (std::size_t current_idx = 1; current_idx < values.size(); ++current_idx, ++last_idx) {
    EXPECT_EQ(values[current_idx] - values[last_idx], 1);
  }
}

/// @test `sequential_values` returns an empty array for a size of 0.
TEST(EmptySequentialValues, IsEmpty) { STATIC_ASSERT_TRUE((sequential_values<std::size_t, 0>.empty())); }

template <typename Count>
class SequentialValuesFromTest : public ::testing::Test {};

TYPED_TEST_SUITE(SequentialValuesFromTest, valid_counts, );

/// @test `sequential_values_from` returns arrays of any size up to `max_number_of_elements`.
TYPED_TEST(SequentialValuesFromTest, ReturnsArrayOfSizeTOfSizeCount) {
  constexpr auto count = TypeParam{}();
  auto values = sequential_values_from<typename TypeParam::value_type, 0, count>;
  ::testing::StaticAssertTypeEq<decltype(values), array<typename TypeParam::value_type, count>>();
}

/// @test `sequential_values_from` with begin=0 has minimum value of zero.
TYPED_TEST(SequentialValuesFromTest, MinValueIsBeginWhenBeginIsZero) {
  constexpr auto count = TypeParam{}();
  auto const values = sequential_values_from<typename TypeParam::value_type, 0, count>;
  EXPECT_EQ(*std::min_element(values.begin(), values.end()), 0);
}

/// @test `sequential_values_from` with begin=0 has maximum value of count - 1.
TYPED_TEST(SequentialValuesFromTest, MaxValueIsCountMinusOneWhenBeginIsZero) {
  constexpr auto count = TypeParam{}();
  auto const values = sequential_values_from<typename TypeParam::value_type, 0, count>;
  EXPECT_EQ(*std::max_element(values.begin(), values.end()), count - 1);
}

/// @test `sequential_values_from` with begin=10 has minimum value of 10.
TYPED_TEST(SequentialValuesFromTest, MinValueIsBeginWhenBeginIsPositive) {
  constexpr auto count = TypeParam{}();
  auto const values = sequential_values_from<typename TypeParam::value_type, 10, count>;
  EXPECT_EQ(*std::min_element(values.begin(), values.end()), 10);
}

/// @test `sequential_values_from` with begin=10 has maximum value of 10 + count - 1.
TYPED_TEST(SequentialValuesFromTest, MaxValueIsBeginPlusCountMinusOneWhenBeginIsPositive) {
  constexpr auto count = TypeParam{}();
  auto const values = sequential_values_from<typename TypeParam::value_type, 10, count>;
  EXPECT_EQ(*std::max_element(values.begin(), values.end()), 10 + count - 1);
}

/// @test `sequential_values_from` with begin=-10 has maximum value of -10 + count - 1.
TYPED_TEST(SequentialValuesFromTest, MaxValueIsBeginPlusCountMinusOneWhenBeginIsNegative) {
  constexpr auto count = TypeParam{}();
  auto const values = sequential_values_from<std::int64_t, -10, count>;
  EXPECT_EQ(*std::max_element(values.begin(), values.end()), -10 + std::int64_t{count} - 1);
}

/// @test Elements in `sequential_values_from` are sequential.
TYPED_TEST(SequentialValuesFromTest, ElementsInArrayAreSequential) {
  constexpr auto count = TypeParam{}();
  auto const values = sequential_values_from<typename TypeParam::value_type, 5, count>;
  std::size_t last_idx = 0;
  for (std::size_t current_idx = 1; current_idx < values.size(); ++current_idx, ++last_idx) {
    EXPECT_EQ(values[current_idx] - values[last_idx], 1);
  }
}

/// @test `sequential_values_from` returns an empty array for a size of 0.
TYPED_TEST(SequentialValuesFromTest, IsEmpty) {
  STATIC_ASSERT_TRUE((sequential_values_from<typename TypeParam::value_type, 0, 0>.empty()));
  STATIC_ASSERT_TRUE((sequential_values_from<typename TypeParam::value_type, 42, 0>.empty()));
}

template <typename Count>
class SequentialValuesBetweenTest : public ::testing::Test {};

TYPED_TEST_SUITE(SequentialValuesBetweenTest, valid_counts, );

/// @test `sequential_values_between` returns arrays of any size up to `max_number_of_elements`.
TYPED_TEST(SequentialValuesBetweenTest, ReturnsArrayOfSizeTOfSizeEndMinusBegin) {
  constexpr auto count = TypeParam{}();
  auto values = sequential_values_between<typename TypeParam::value_type, 0, count>;
  ::testing::StaticAssertTypeEq<decltype(values), array<typename TypeParam::value_type, count>>();
}

/// @test `sequential_values_between` has minimum value equal to begin.
TYPED_TEST(SequentialValuesBetweenTest, MinValueIsBegin) {
  constexpr auto count = TypeParam{}();
  auto const values = sequential_values_between<typename TypeParam::value_type, 10, 10 + count>;
  EXPECT_EQ(*std::min_element(values.begin(), values.end()), 10);
}

/// @test `sequential_values_between` has minimum value equal to begin when begin is negative.
TYPED_TEST(SequentialValuesBetweenTest, MinValueIsBeginNegative) {
  constexpr auto count = TypeParam{}();
  auto const values = sequential_values_between<std::int64_t, -10, std::int64_t{count} - 10>;
  EXPECT_EQ(*std::min_element(values.begin(), values.end()), -10);
}

/// @test `sequential_values_between` has maximum value equal to end - 1.
TYPED_TEST(SequentialValuesBetweenTest, MaxValueIsEndMinusOne) {
  constexpr auto count = TypeParam{}();
  auto const values = sequential_values_between<typename TypeParam::value_type, 10, 10 + count>;
  EXPECT_EQ(*std::max_element(values.begin(), values.end()), 10 + count - 1);
}

/// @test Elements in `sequential_values_between` are sequential.
TYPED_TEST(SequentialValuesBetweenTest, ElementsInArrayAreSequential) {
  constexpr auto count = TypeParam{}();
  auto const values = sequential_values_between<typename TypeParam::value_type, 5, 5 + count>;
  std::size_t last_idx = 0;
  for (std::size_t current_idx = 1; current_idx < values.size(); ++current_idx, ++last_idx) {
    EXPECT_EQ(values[current_idx] - values[last_idx], 1);
  }
}

/// @test `sequential_values_between` returns an empty array when begin equals end.
TYPED_TEST(SequentialValuesBetweenTest, IsEmptyWhenBeginEqualsEnd) {
  STATIC_ASSERT_TRUE((sequential_values_between<typename TypeParam::value_type, 0, 0>.empty()));
  STATIC_ASSERT_TRUE((sequential_values_between<typename TypeParam::value_type, 42, 42>.empty()));
}

}  // namespace

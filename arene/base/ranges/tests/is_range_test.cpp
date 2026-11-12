// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/ranges/is_range.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/inline_container/vector.hpp"
#include "arene/base/ranges/tests/ranges_test_helpers.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

using ::arene::base::ranges::is_range;
using ::arene::base::ranges::is_range_v;

template <typename TypeParam>
class IsRangeForValidRangesTest : public ::testing::Test {};

TYPED_TEST_SUITE(IsRangeForValidRangesTest, ranges_test::valid_range_types, );

/// @test Given a type whose `ranges::begin` and `ranges::end` expressions are well-formed and whose results denote a
/// range, when `is_range` is instantiated, then its `value` is `true`.
TYPED_TEST(IsRangeForValidRangesTest, IsTrue) { STATIC_ASSERT_TRUE(is_range<TypeParam>::value); }

template <typename TypeParam>
class IsRangeForNonBorrowedRangeTypesTest : public ::testing::Test {};

TYPED_TEST_SUITE(IsRangeForNonBorrowedRangeTypesTest, ranges_test::non_borrowed_range_types, );

/// @test Given a non-borrowed range type passed as an unqualified or rvalue type, when `is_range` is instantiated, then
/// its `value` is `false`.
TYPED_TEST(IsRangeForNonBorrowedRangeTypesTest, IsFalse) { STATIC_ASSERT_FALSE(is_range<TypeParam>::value); }

template <typename T>
class IsRangeForNonRangeTypes : public ::testing::Test {};

TYPED_TEST_SUITE(IsRangeForNonRangeTypes, ranges_test::all_qualified_non_range_types, );

/// @test Given a type `Rng` which does not satisfy one or more of the requirements of `is_range`, when `is_range` is
/// instantiated, then its `value` is `false`.
TYPED_TEST(IsRangeForNonRangeTypes, IsFalse) { STATIC_ASSERT_FALSE(is_range<TypeParam>::value); }

template <typename TypeParam>
class IsRangeVTest : public ::testing::Test {};

TYPED_TEST_SUITE(IsRangeVTest, ranges_test::all_range_and_non_range_types, );

/// @test Given any type `Rng`, when `is_range_v<Rng>` is evaluated, then it equals `is_range<Rng>::value`.
TYPED_TEST(IsRangeVTest, MatchesIsRangeValue) { STATIC_ASSERT_EQ(is_range_v<TypeParam>, is_range<TypeParam>::value); }

}  // namespace

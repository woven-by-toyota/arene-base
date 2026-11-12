// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/ranges/is_borrowed_range.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/inline_container/vector.hpp"
#include "arene/base/ranges/tests/ranges_test_helpers.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/concat.hpp"
#include "arene/base/type_list/remove_duplicates.hpp"

namespace {

using ::arene::base::ranges::is_borrowed_range;
using ::arene::base::ranges::is_borrowed_range_v;

template <typename TypeParam>
class IsBorrowedRangeForExplicitlyOptedInRangesTest : public ::testing::Test {};

TYPED_TEST_SUITE(IsBorrowedRangeForExplicitlyOptedInRangesTest, ranges_test::all_explicit_borrowed_range_types, );

/// @test Given a type `Rng` which satisfies `is_range<Rng>` and for which `enable_borrowed_range<Rng>` is `true`, when
/// `is_borrowed_range<Rng>` is instantiated, then its `value` is `true`.
TYPED_TEST(IsBorrowedRangeForExplicitlyOptedInRangesTest, IsTrue) {
  STATIC_ASSERT_TRUE(is_borrowed_range<TypeParam>::value);
}

template <typename TypeParam>
class IsBorrowedRangeForLvalueReferencesTest : public ::testing::Test {};

using all_lvalue_range_types = ::arene::base::type_lists::remove_duplicates_t<::arene::base::type_lists::concat_t<
    ranges_test::lvalue_reference_range_types,
    ranges_test::const_lvalue_reference_range_types>>;

TYPED_TEST_SUITE(IsBorrowedRangeForLvalueReferencesTest, all_lvalue_range_types, );

/// @test Given a type `Rng` which satisfies `is_range<Rng>` and which is an lvalue reference but has not specialized
/// `enable_borrowed_range<Rng>`, when `is_borrowed_range<Rng>` is instantiated, then its `value` is `true`.
TYPED_TEST(IsBorrowedRangeForLvalueReferencesTest, IsTrue) { STATIC_ASSERT_TRUE(is_borrowed_range<TypeParam>::value); }

template <typename TypeParam>
class IsBorrowedRangeForNonRangesTest : public ::testing::Test {};

TYPED_TEST_SUITE(IsBorrowedRangeForNonRangesTest, ranges_test::all_qualified_non_range_types, );

/// @test Given a type `Rng` which does not satisfy `is_range<Rng>`, when `is_borrowed_range<Rng>` is instantiated, then
/// its `value` is `false`.
TYPED_TEST(IsBorrowedRangeForNonRangesTest, IsFalse) { STATIC_ASSERT_FALSE(is_borrowed_range<TypeParam>::value); }

template <typename TypeParam>
class IsBorrowedRangeForNonBorrowedRangesTest : public ::testing::Test {};

TYPED_TEST_SUITE(IsBorrowedRangeForNonBorrowedRangesTest, ranges_test::non_borrowed_range_types, );

/// @test Given a type `Rng` which satisfies `is_range<Rng>` and which is not and lvalue reference and has not
/// specialized `enable_borrowed_range<Rng>`, when `is_borrowed_range<Rng>` is instantiated, then its `value` is
/// `false`.
TYPED_TEST(IsBorrowedRangeForNonBorrowedRangesTest, IsFalse) {
  STATIC_ASSERT_FALSE(is_borrowed_range<TypeParam>::value);
}

template <typename TypeParam>
class IsBorrowedRangeVTest : public ::testing::Test {};

TYPED_TEST_SUITE(IsBorrowedRangeVTest, ranges_test::all_range_and_non_range_types, );

/// @test Given any type `Rng`, when `is_borrowed_range_v<Rng>` is evaluated, then it equals
/// `is_borrowed_range<Rng>::value`.
TYPED_TEST(IsBorrowedRangeVTest, MatchesIsBorrowedRangeValue) {
  STATIC_ASSERT_EQ(is_borrowed_range_v<TypeParam>, is_borrowed_range<TypeParam>::value);
}

}  // namespace

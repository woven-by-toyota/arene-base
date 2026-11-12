// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/ranges/borrowed_iterator.hpp"

#include <gtest/gtest.h>

#include "arene/base/ranges/begin_end.hpp"
#include "arene/base/ranges/dangling.hpp"
#include "arene/base/ranges/tests/ranges_test_helpers.hpp"

namespace {

using ::arene::base::ranges::borrowed_iterator_t;

template <typename Rng>
class BorrowedIteratorTBorrowedRangeTest : public ::testing::Test {};

TYPED_TEST_SUITE(BorrowedIteratorTBorrowedRangeTest, ranges_test::all_qualified_borrowed_range_types, );

/// @test Given a type `Rng` which satisfies `arene::base::ranges::is_borrowed_range_v<Rng>`, then
/// `arene::base::ranges::borrowed_iterator_t<Rng>` is `arene::base::ranges::iterator_t<Rng>`.
TYPED_TEST(BorrowedIteratorTBorrowedRangeTest, IsIteratorT) {
  ::testing::StaticAssertTypeEq<borrowed_iterator_t<TypeParam>, ::arene::base::ranges::iterator_t<TypeParam>>();
}

template <typename Rng>
class BorrowedIteratorTNonBorrowedRangeTest : public ::testing::Test {};

TYPED_TEST_SUITE(BorrowedIteratorTNonBorrowedRangeTest, ranges_test::non_borrowed_range_types, );

/// @test Given a type `Rng` which satisfies `arene::base::is_range_v<Rng>` but does not satisfy
/// `arene::base::ranges::is_borrowed_range_v<Rng>`, then `arene::base::ranges::borrowed_iterator_t<Rng>` is
/// `arene::base::ranges::dangling`.
TYPED_TEST(BorrowedIteratorTNonBorrowedRangeTest, IsDangling) {
  ::testing::StaticAssertTypeEq<borrowed_iterator_t<TypeParam>, ::arene::base::ranges::dangling>();
}

template <typename Rng>
class BorrowedIteratorTNonRangeTest : public ::testing::Test {};

TYPED_TEST_SUITE(BorrowedIteratorTNonRangeTest, ranges_test::all_qualified_non_range_types, );

/// @test Given a type `Rng` which does not satisfy `arene::base::ranges::is_range_v<Rng>`, then
/// `arene::base::ranges::borrowed_iterator_t<Rng>` is `arene::base::ranges::dangling`.
TYPED_TEST(BorrowedIteratorTNonRangeTest, IsDangling) {
  ::testing::StaticAssertTypeEq<borrowed_iterator_t<TypeParam>, ::arene::base::ranges::dangling>();
}

}  // namespace

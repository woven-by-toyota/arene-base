// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/ranges/enable_borrowed_range.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

using ::arene::base::ranges::enable_borrowed_range;

struct non_borrowed_range {};

// NOLINTNEXTLINE(hicpp-avoid-c-arrays) explicitly testing for behavior of a c-array.
using non_borrowed_range_types = ::testing::Types<int, int*, int[4], non_borrowed_range>;

template <typename T>
class EnableBorrowedRangeDefaultTest : public ::testing::Test {};

TYPED_TEST_SUITE(EnableBorrowedRangeDefaultTest, non_borrowed_range_types, );

/// @test Given a type `T` which does not specialize `enable_borrowed_range`, then the value of
/// `enable_borrowed_range_v<T>` is `false`
TYPED_TEST(EnableBorrowedRangeDefaultTest, IsFalse) {
  STATIC_ASSERT_FALSE(::arene::base::ranges::enable_borrowed_range_v<TypeParam>);
}

struct borrowed_range_enabled {};

}  // namespace

namespace arene {

template <>
class base::ranges::enable_borrowed_range<borrowed_range_enabled> : public std::true_type {};
}  // namespace arene

namespace {
/// @test Given a type `T` which specializes `enable_borrowed_range`, when `enable_borrowed_range_v<T>` is queried, then
/// its value is `true`.
TEST(EnableBorrowedRangeSpecializationTest, IsTrue) {
  STATIC_ASSERT_TRUE(::arene::base::ranges::enable_borrowed_range_v<borrowed_range_enabled>);
}
}  // namespace

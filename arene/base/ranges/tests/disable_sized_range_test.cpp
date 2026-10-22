// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/ranges/disable_sized_range.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

using ::arene::base::ranges::disable_sized_range;
using ::arene::base::ranges::disable_sized_range_v;

struct sized_range {};

// NOLINTNEXTLINE(hicpp-avoid-c-arrays) explicitly testing for behavior of a c-array.
using sized_range_types = ::testing::Types<int, int*, int[4], sized_range>;

template <typename T>
class DisableSizedRangeDefaultTest : public ::testing::Test {};

TYPED_TEST_SUITE(DisableSizedRangeDefaultTest, sized_range_types, );

/// @test Given a type `T` which does not specialize `disable_sized_range`, then both the primary template's
/// `disable_sized_range<T>::value` and the accessor `disable_sized_range_v<T>` are `false`
TYPED_TEST(DisableSizedRangeDefaultTest, IsFalse) {
  STATIC_ASSERT_FALSE(disable_sized_range<TypeParam>::value);
  STATIC_ASSERT_FALSE(disable_sized_range_v<TypeParam>);
}

struct disabled_sized_range {};

}  // namespace

template <>
class arene::base::ranges::disable_sized_range<disabled_sized_range> : public std::true_type {};

namespace {
/// @test Given a type `T` which specializes `disable_sized_range`, then both the struct's
/// `disable_sized_range<T>::value` and the accessor `disable_sized_range_v<T>` are `true`
TEST(DisableSizedRangeSpecializationTest, IsTrue) {
  STATIC_ASSERT_TRUE(disable_sized_range<disabled_sized_range>::value);
  STATIC_ASSERT_TRUE(disable_sized_range_v<disabled_sized_range>);
}
}  // namespace

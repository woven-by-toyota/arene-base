// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/diagnostics.hpp"
#include "testlibs/minitest/minitest.hpp"

namespace {

using float_types = ::testing::Types<float, double, long double>;

template <typename T>
class FloatingPointTest : public testing::Test {};

TYPED_TEST_SUITE(FloatingPointTest, float_types, );

template <typename TypeParam>
constexpr TypeParam infinity{__builtin_inff()};

template <typename TypeParam>
constexpr TypeParam nan{__builtin_nan("0")};

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "We're testing the specific behaviour of floating point equality");

/// @test For two equal values, they are both equal and near each other
TYPED_TEST(FloatingPointTest, EqualValuesExact) {
  ASSERT_EQ(TypeParam{1L}, TypeParam{1L});
}

/// @test For two equal values, they are both equal and near each other
TYPED_TEST(FloatingPointTest, EqualValuesNear) {
  ASSERT_NEAR(TypeParam{1L}, TypeParam{1L}, TypeParam{0L});
}

/// @test For two nearby values, they are not equal but are near
TYPED_TEST(FloatingPointTest, NearbyValuesExact) {
  ASSERT_EQ(TypeParam{-1L}, TypeParam{-1.5L});
}

/// @test For two nearby values, they are not equal but are near
TYPED_TEST(FloatingPointTest, NearbyValuesNear) {
  ASSERT_NEAR(TypeParam{-1L}, TypeParam{-1.5L}, TypeParam{0.5L});
}

/// @test For two far-apart values, they are neither equal nor near each other
TYPED_TEST(FloatingPointTest, FarValuesExact) {
  ASSERT_EQ(static_cast<TypeParam>(0.3L), static_cast<TypeParam>(0.9L));
}

/// @test For two far-apart values, they are neither equal nor near each other
TYPED_TEST(FloatingPointTest, FarValuesNear) {
  ASSERT_NEAR(static_cast<TypeParam>(0.3L), static_cast<TypeParam>(0.9L), static_cast<TypeParam>(0.5L));
}

/// @test Two infinities are equal but are not near each other because their difference is NaN
TYPED_TEST(FloatingPointTest, InfinityExact) {
  ASSERT_EQ(infinity<TypeParam>, infinity<TypeParam>);
}

/// @test Two infinities are equal but are not near each other because their difference is NaN
TYPED_TEST(FloatingPointTest, InfinityNear) {
  ASSERT_NEAR(infinity<TypeParam>, infinity<TypeParam>, infinity<TypeParam>);
}

/// @test NaN values are neither equal to nor near any value
TYPED_TEST(FloatingPointTest, NaNWithZeroExact) {
  ASSERT_EQ(nan<TypeParam>, TypeParam{0});
}

/// @test NaN values are neither equal to nor near any value
TYPED_TEST(FloatingPointTest, NaNWithZeroNear) {
  ASSERT_NEAR(nan<TypeParam>, TypeParam{0}, TypeParam{1});
}

/// @test NaN values are neither equal to nor near themselves
TYPED_TEST(FloatingPointTest, NaNWithNaNExact) {
  ASSERT_EQ(nan<TypeParam>, nan<TypeParam>);
}

/// @test NaN values are neither equal to nor near themselves
TYPED_TEST(FloatingPointTest, NaNWithNaNNear) {
  ASSERT_NEAR(nan<TypeParam>, nan<TypeParam>, TypeParam{1});
}

ARENE_IGNORE_END();

}  // namespace

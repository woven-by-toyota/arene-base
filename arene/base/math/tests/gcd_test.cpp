// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/math/gcd.hpp"

#include <gtest/gtest.h>

#include "arene/base/testing/gtest.hpp"

namespace {
/// @test `gcd` returns the expected value, simple case.
TEST(GCD, Simple) { STATIC_ASSERT_EQ(arene::base::gcd(1, 2), 1); }

/// @test `gcd` returns the expected value.
TEST(GCD, CalculatedValueCorrect) {
  STATIC_ASSERT_EQ(arene::base::gcd(42, 56), 14);
  STATIC_ASSERT_EQ(arene::base::gcd(461952, 116298), 18);
  STATIC_ASSERT_EQ(arene::base::gcd(7966496, 314080416), 32);
  STATIC_ASSERT_EQ(arene::base::gcd(24826148, 45296490), 526);
}

/// @test `gcd` returns the expected value, using 0 as numerator and denominator.
TEST(GCD, ZeroArgument) {
  STATIC_ASSERT_EQ(arene::base::gcd(12, 0), 12);
  STATIC_ASSERT_EQ(arene::base::gcd(0, 12), 12);
  STATIC_ASSERT_EQ(arene::base::gcd(0, 0), 0);
}

/// @test `gcd` returns the expected value, using prime numbers.
TEST(GCD, PrimeNumbers) {
  STATIC_ASSERT_EQ(arene::base::gcd(1001639, 5001707), 1);
  STATIC_ASSERT_EQ(arene::base::gcd(1001639, 1001639), 1001639);
}

/// @test `gcd` returns positive values even when operands are negative.
TEST(GCD, NegativeOperands) {
  STATIC_ASSERT_EQ(arene::base::gcd(-42, 56), 14);
  STATIC_ASSERT_EQ(arene::base::gcd(461952, -116298), 18);
  STATIC_ASSERT_EQ(arene::base::gcd(-7966496, -314080416), 32);

  STATIC_ASSERT_EQ(arene::base::gcd(-7, 0), 7);
  STATIC_ASSERT_EQ(arene::base::gcd(0, -7), 7);
  STATIC_ASSERT_EQ(arene::base::gcd(3, -2), 1);
  STATIC_ASSERT_EQ(arene::base::gcd(-3, 2), 1);
  STATIC_ASSERT_EQ(arene::base::gcd(-3, -2), 1);
}
}  // namespace

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/math/pi.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {
/// @test Check that the variable template has the right type
TEST(Pi, PiVHasExpectedType) {
  ::testing::StaticAssertTypeEq<decltype(arene::base::numbers::pi_v<float>), float const>();
  ::testing::StaticAssertTypeEq<decltype(arene::base::numbers::pi_v<double>), double const>();
}

/// @test Check that the non-templated variable has the right type
TEST(Pi, PiHasExpectedType) { ::testing::StaticAssertTypeEq<decltype(arene::base::numbers::pi), double const&>(); }

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "Test code: float equal comparison is checking a constant");

/// @test Check that the variable template has the right value
TEST(Pi, PiVHasExpectedValue) {
  STATIC_ASSERT_EQ(
      arene::base::numbers::pi_v<float>,
      3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679821480865132823066470938446095505822317253594081284811174502841027019385211055596446229489549303819644288109756659334461284756482337867831652712F
  );
  STATIC_ASSERT_EQ(
      arene::base::numbers::pi_v<double>,
      3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679821480865132823066470938446095505822317253594081284811174502841027019385211055596446229489549303819644288109756659334461284756482337867831652712
  );
}

/// @test Check that the non-templated variable has the right value
TEST(Pi, PiHasExpectedValue) {
  STATIC_ASSERT_EQ(
      arene::base::numbers::pi,
      3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679821480865132823066470938446095505822317253594081284811174502841027019385211055596446229489549303819644288109756659334461284756482337867831652712
  );
}

ARENE_IGNORE_END();

}  // namespace

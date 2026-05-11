// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/algorithm/clamp.hpp"
#include "arene/base/stdlib_choice/greater.hpp"
#include "arene/base/testing/gtest.hpp"
#include "testlibs/utilities/configurable_value.hpp"

namespace {

// NOLINTBEGIN(hicpp-avoid-c-arrays) Needs to work in constexpr without depending on array.

template <typename T>
constexpr T low_mid_high[3U]{T{}, T{}, T{}};

template <>
constexpr int low_mid_high<int>[3U] { -1256, 8323, 6773415 };

template <>
constexpr float low_mid_high<float>[3U] { -3.14F, -0.0F, 8'000'000.36F };

using user_defined_type = ::testing::configurable_value<int>;

template <>
constexpr user_defined_type low_mid_high<user_defined_type>[3U] {
  user_defined_type{-1256}, user_defined_type{8323}, user_defined_type { 6773415 }
};

// NOLINTEND(hicpp-avoid-c-arrays) Needs to work in constexpr without depending on array.

template <typename T>
class ClampDeathTest : public ::testing::Test {
 protected:
  static constexpr T const& low = low_mid_high<T>[0];
  static constexpr T const& mid = low_mid_high<T>[1];
  static constexpr T const& high = low_mid_high<T>[2];
};

using test_types = ::testing::Types<int, float, user_defined_type>;

TYPED_TEST_SUITE(ClampDeathTest, test_types, );

// NOLINTBEGIN(readability-suspicious-call-argument) Intentionally testing the behavior of swapped arguments.

/// @test Calls to clamp crash with a precondition violation if low is strictly greater than high
TYPED_TEST(ClampDeathTest, HighLessThanLowIsAPreconditionViolation) {
  EXPECT_DEATH(::arene::base::clamp(TestFixture::mid, TestFixture::high, TestFixture::low), "Precondition");
  EXPECT_DEATH(
      ::arene::base::clamp(TestFixture::mid, TestFixture::low, TestFixture::high, std::greater<TypeParam>{}),
      "Precondition"
  );
}

// NOLINTEND(readability-suspicious-call-argument)

}  // namespace

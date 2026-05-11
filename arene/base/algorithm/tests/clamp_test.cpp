// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/algorithm/clamp.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/stdlib_choice/greater.hpp"
#include "arene/base/stdlib_choice/less.hpp"
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
class ClampTest : public ::testing::Test {
 protected:
  static constexpr T const& low = low_mid_high<T>[0];
  static constexpr T const& mid = low_mid_high<T>[1];
  static constexpr T const& high = low_mid_high<T>[2];
};

using test_types = ::testing::Types<int, float, user_defined_type>;
TYPED_TEST_SUITE(ClampTest, test_types, );

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "These tests don't perform arithmetic, so equality is OK even for floating point");

// NOLINTBEGIN(readability-suspicious-call-argument) We are testing what happens if you put the args in various orders

/// @test Validate that `clamp` returns a reference of the same type as its inputs.
TYPED_TEST(ClampTest, ClampReturnsReferenceToInputTypes) {
  ::testing::StaticAssertTypeEq<
      decltype(::arene::base::clamp(TestFixture::low, TestFixture::mid, TestFixture::high)),
      TypeParam const&>();
}

/// @test Clamping an input between two values which it's between returns the input.
TYPED_TEST(ClampTest, ClampInBetweenReturnsSelf) {
  STATIC_ASSERT_EQ(::arene::base::clamp(TestFixture::mid, TestFixture::low, TestFixture::high), TestFixture::mid);
  STATIC_ASSERT_EQ(
      ::arene::base::clamp(TestFixture::mid, TestFixture::high, TestFixture::low, std::greater<TypeParam>{}),
      TestFixture::mid
  );
}

/// @test Clamping an input between two values, both lower, returns the lower bound.
TYPED_TEST(ClampTest, ClampBelowLowReturnsLow) {
  STATIC_ASSERT_EQ(::arene::base::clamp(TestFixture::low, TestFixture::mid, TestFixture::high), TestFixture::mid);
  STATIC_ASSERT_EQ(
      ::arene::base::clamp(TestFixture::low, TestFixture::high, TestFixture::mid, std::greater<TypeParam>{}),
      TestFixture::mid
  );
}

/// @test Clamping an input between two values, both lower, returns the upper bound.
TYPED_TEST(ClampTest, ClampAboveHighReturnsHigh) {
  STATIC_ASSERT_EQ(::arene::base::clamp(TestFixture::high, TestFixture::low, TestFixture::mid), TestFixture::mid);
  STATIC_ASSERT_EQ(
      ::arene::base::clamp(TestFixture::high, TestFixture::mid, TestFixture::low, std::greater<TypeParam>{}),
      TestFixture::mid
  );
}

/// @test Clamping an input between two values, with input equal to the lower, returns the input.
TYPED_TEST(ClampTest, ClampEqualToLowReturnsSelf) {
  constexpr TypeParam val = TestFixture::low;

  STATIC_ASSERT_EQ(::arene::base::clamp(val, TestFixture::low, TestFixture::high), val);
  STATIC_ASSERT_EQ(::arene::base::clamp(val, TestFixture::high, TestFixture::low, std::greater<TypeParam>{}), val);
}

/// @test Clamping an input between two values, with input equal to the higher, returns the input.
TYPED_TEST(ClampTest, ClampEqualToHighReturnsSelf) {
  constexpr TypeParam val = TestFixture::high;

  STATIC_ASSERT_EQ(::arene::base::clamp(val, TestFixture::low, TestFixture::high), val);
  STATIC_ASSERT_EQ(::arene::base::clamp(val, TestFixture::high, TestFixture::low, std::greater<TypeParam>{}), val);
}

/// @test Clamping an input between two equal values returns a reference to the input.
TYPED_TEST(ClampTest, ClampWithAllEqualReturnsSelf) {
  constexpr TypeParam equal_low = TestFixture::mid;
  constexpr TypeParam equal_high = TestFixture::mid;

  STATIC_ASSERT_EQ(::arene::base::clamp(TestFixture::mid, equal_low, equal_high), TestFixture::mid);
  STATIC_ASSERT_EQ(
      ::arene::base::clamp(TestFixture::mid, equal_high, equal_low, std::greater<TypeParam>{}),
      TestFixture::mid
  );
}

/// @test clamp is noexcept if and only if the comparator is
TYPED_TEST(ClampTest, ConditionalNoexceptMatchesComparator) {
  struct noexcept_less {
    constexpr auto operator()(TypeParam const& left, TypeParam const& right) noexcept -> bool { return left < right; }
  };

  struct throwing_less {
    // This can't be constexpr because a bug in GCC8 causes it to be incorrectly marked as noexcept(true).
    auto operator()(TypeParam const& left, TypeParam const& right) noexcept(false) -> bool { return left < right; }
  };

  // This value could be anything; we're just giving it a name because it's easier to read than a bunch of declvals.
  constexpr TypeParam const& val = low_mid_high<TypeParam>[0];

  // The throwing assertion can't be static because of the aforementioned GCC8 bug. It works in GCC9+ and Clang.
  STATIC_ASSERT_TRUE(noexcept(::arene::base::clamp(val, val, val, noexcept_less{})));
  ASSERT_FALSE(noexcept(::arene::base::clamp(val, val, val, throwing_less{})));

  // std::less is not specified to be marked noexcept so whether it is or not is up to the implementation.
  constexpr bool less_is_noexcept = noexcept(std::less<TypeParam>{}(val, val));
  STATIC_ASSERT_EQ(noexcept(::arene::base::clamp(val, val, val)), less_is_noexcept);
}

// NOLINTEND(readability-suspicious-call-argument)

ARENE_IGNORE_END();

}  // namespace

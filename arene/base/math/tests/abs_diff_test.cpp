// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/math/abs_diff.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/math/abs.hpp"
#include "arene/base/math/float_properties.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/is_signed.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/remove_duplicates.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

namespace {
template <typename T>
class AbsDiffTypedTest : public testing::Test {};

using arithmetic_types = arene::base::type_lists::remove_duplicates_t<::testing::Types<
    std::int8_t,
    std::int16_t,
    std::int32_t,
    std::int64_t,
    std::intmax_t,
    std::uint8_t,
    std::uint16_t,
    std::uint32_t,
    std::uint64_t,
    std::uintmax_t,
    float,
    double>>;

TYPED_TEST_SUITE(AbsDiffTypedTest, arithmetic_types, );

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "Test code: float equal comparison is on purpose");

/// @test `abs_diff` can only be called with arithmetic types.
TYPED_TEST(AbsDiffTypedTest, Constraints) {
  using T = TypeParam;

  auto const invoker = [](auto lhs, auto rhs) -> decltype(arene::base::abs_diff(lhs, rhs)) {
    return arene::base::abs_diff(lhs, rhs);
  };

  STATIC_ASSERT_TRUE((arene::base::is_invocable_r_v<T, decltype(invoker), T, T>));

  // Must be called with the same type.
  STATIC_ASSERT_FALSE((arene::base::is_invocable_r_v<T, decltype(invoker), std::int8_t, std::int32_t>));

  // Both arguments must be arithmetic.
  STATIC_ASSERT_FALSE((arene::base::is_invocable_r_v<T, decltype(invoker), T*, T>));
  STATIC_ASSERT_FALSE((arene::base::is_invocable_r_v<T, decltype(invoker), T, T*>));
  STATIC_ASSERT_FALSE((arene::base::is_invocable_r_v<T, decltype(invoker), T*, T*>));

  struct s {};
  STATIC_ASSERT_FALSE((arene::base::is_invocable_r_v<T, decltype(invoker), s, s>));
}

/// @test `abs_diff` returns the expected values with integer types.
TYPED_TEST(AbsDiffTypedTest, SmokeTest) {
  using T = TypeParam;

  constexpr auto zero = static_cast<T>(0);
  constexpr auto positive = static_cast<T>(42);
  constexpr auto negative_or_large_unsigned = static_cast<T>(-42);

  STATIC_ASSERT_EQ(arene::base::abs_diff(zero, zero), zero);

  STATIC_ASSERT_EQ(arene::base::abs_diff(positive, zero), positive);
  STATIC_ASSERT_EQ(arene::base::abs_diff(zero, positive), positive);
  STATIC_ASSERT_EQ(arene::base::abs_diff(positive, positive), zero);

  STATIC_ASSERT_EQ(arene::base::abs_diff(negative_or_large_unsigned, negative_or_large_unsigned), zero);
  STATIC_ASSERT_EQ(
      arene::base::abs_diff(negative_or_large_unsigned, zero),
      arene::base::abs(negative_or_large_unsigned)
  );
  STATIC_ASSERT_EQ(
      arene::base::abs_diff(zero, negative_or_large_unsigned),
      arene::base::abs(negative_or_large_unsigned)
  );
}

/// @test `abs_diff` returns the expected values with mixed sign integers
TYPED_TEST(AbsDiffTypedTest, MixedSignTest) {
  using T = TypeParam;

  constexpr auto positive = static_cast<T>(42);
  constexpr auto negative_or_large_unsigned = static_cast<T>(-42);

  auto expected = T{};

  if (std::is_signed<T>::value) {
    expected = positive * 2;
  } else {
    expected = negative_or_large_unsigned - positive;
  }

  ASSERT_EQ(arene::base::abs_diff(positive, negative_or_large_unsigned), expected);
  ASSERT_EQ(arene::base::abs_diff(negative_or_large_unsigned, positive), expected);
}

/// @test `abs_diff` returns the expected values at the extremes
TYPED_TEST(AbsDiffTypedTest, Extremes) {
  using T = TypeParam;

  constexpr auto zero = static_cast<T>(0);
  constexpr auto one = static_cast<T>(1);
  constexpr auto max = std::numeric_limits<T>::max();
  constexpr auto min = std::numeric_limits<T>::min();

  STATIC_ASSERT_EQ(arene::base::abs_diff(max, max), zero);
  STATIC_ASSERT_EQ(arene::base::abs_diff(min, min), zero);

  STATIC_ASSERT_EQ(arene::base::abs_diff(static_cast<T>(max - one), zero), static_cast<T>(max - one));
  STATIC_ASSERT_EQ(arene::base::abs_diff(static_cast<T>(min + one), zero), arene::base::abs(static_cast<T>(min + one)));
}

template <typename T>
class AbsDiffFloatTypedTest : public AbsDiffTypedTest<T> {};

using float_types = ::testing::Types<float, double>;

TYPED_TEST_SUITE(AbsDiffFloatTypedTest, float_types, );

/// @test `abs_diff` returns the expected values with floating point NaN and Infinity.
TYPED_TEST(AbsDiffFloatTypedTest, FloatingNumberExtremes) {
  using T = TypeParam;

  static_assert(
      std::numeric_limits<T>::is_iec559,
      "This test is only valid for IEEE 754 defined floating point values, as it relies on specific NaN behavior."
  );

  constexpr auto zero = static_cast<T>(0);
  constexpr auto inf = std::numeric_limits<T>::infinity();
  constexpr auto neg_inf = -std::numeric_limits<T>::infinity();
  constexpr auto nan = std::numeric_limits<T>::quiet_NaN();
  constexpr auto epsilon = std::numeric_limits<T>::epsilon();
  constexpr auto denorm_min = std::numeric_limits<T>::denorm_min();
  constexpr auto max = std::numeric_limits<T>::max();

  STATIC_ASSERT_EQ(arene::base::abs_diff(inf, zero), inf);
  STATIC_ASSERT_EQ(arene::base::abs_diff(zero, inf), inf);
  ASSERT_TRUE(arene::base::isnan(arene::base::abs_diff(inf, inf)));

  STATIC_ASSERT_EQ(arene::base::abs_diff(neg_inf, zero), inf);
  STATIC_ASSERT_EQ(arene::base::abs_diff(zero, neg_inf), inf);
  ASSERT_TRUE(arene::base::isnan(arene::base::abs_diff(neg_inf, neg_inf)));

  ASSERT_TRUE(arene::base::isnan(arene::base::abs_diff(nan, zero)));
  ASSERT_TRUE(arene::base::isnan(arene::base::abs_diff(zero, nan)));
  ASSERT_TRUE(arene::base::isnan(arene::base::abs_diff(nan, nan)));

  STATIC_ASSERT_EQ(arene::base::abs_diff(zero, epsilon), epsilon);
  STATIC_ASSERT_EQ(arene::base::abs_diff(epsilon, zero), epsilon);
  STATIC_ASSERT_EQ(arene::base::abs_diff(epsilon, epsilon), zero);

  STATIC_ASSERT_EQ(arene::base::abs_diff(zero, denorm_min), denorm_min);
  STATIC_ASSERT_EQ(arene::base::abs_diff(denorm_min, zero), denorm_min);
  STATIC_ASSERT_EQ(arene::base::abs_diff(denorm_min, denorm_min), zero);

  STATIC_ASSERT_EQ(arene::base::abs_diff(max, -epsilon), max);
  STATIC_ASSERT_EQ(arene::base::abs_diff(max, epsilon), max);
}

ARENE_IGNORE_END();

}  // namespace

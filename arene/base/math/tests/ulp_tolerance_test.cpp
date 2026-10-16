// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/math/ulp_tolerance.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/platform_queries/has_constexpr_bit_cast.hpp"
#include "arene/base/compiler_support/preprocessor/define_configuration.hpp"
#include "arene/base/math/tests/float_utilities.hpp"  // IWYU pragma: keep
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "testlibs/utilities/float32.hpp"

namespace {

using arene::base::invoke_result_t;
using arene::base::is_invocable_v;
using arene::base::is_nothrow_invocable_v;
using arene::base::ulp_tolerance;

template <typename T>
class UlpTolerance : public ::testing::Test {};

using test_types = ::testing::Types<float, double, ::testing::float32>;

TYPED_TEST_SUITE(UlpTolerance, test_types, );

/// @test @c ulp_tolerance must use an integer type to set the tolerance
TEST(UlpTolerance, ToleranceCanBeIntegerOnly) {
  EXPECT_TRUE((is_nothrow_invocable_v<decltype(ulp_tolerance), signed char>));
  EXPECT_TRUE((is_nothrow_invocable_v<decltype(ulp_tolerance), short>));
  EXPECT_TRUE((is_nothrow_invocable_v<decltype(ulp_tolerance), int>));
  EXPECT_TRUE((is_nothrow_invocable_v<decltype(ulp_tolerance), long>));
  EXPECT_TRUE((is_nothrow_invocable_v<decltype(ulp_tolerance), long long>));

  EXPECT_TRUE((is_invocable_v<decltype(ulp_tolerance), unsigned char>));
  EXPECT_TRUE((is_invocable_v<decltype(ulp_tolerance), unsigned short>));
  EXPECT_TRUE((is_invocable_v<decltype(ulp_tolerance), unsigned int>));
  EXPECT_TRUE((is_invocable_v<decltype(ulp_tolerance), unsigned long>));
  EXPECT_TRUE((is_invocable_v<decltype(ulp_tolerance), unsigned long long>));

  EXPECT_FALSE((is_invocable_v<decltype(ulp_tolerance), float>));
  EXPECT_FALSE((is_invocable_v<decltype(ulp_tolerance), double>));
  EXPECT_FALSE((is_invocable_v<decltype(ulp_tolerance), long double>));
}

/// @test The comparator created by @c ulp_tolerance can only be used on certain enabled floating point types
TEST(UlpTolerance, ComparatorArgsMustBeEnabledFloatingPointTypes) {
  EXPECT_TRUE((is_nothrow_invocable_v<decltype(ulp_tolerance), std::int8_t>));
  using comparator_8 = invoke_result_t<decltype(ulp_tolerance), std::int8_t>;

  EXPECT_TRUE((is_nothrow_invocable_v<comparator_8, float, float>));
  EXPECT_TRUE((is_nothrow_invocable_v<comparator_8, double, double>));
  EXPECT_FALSE((is_invocable_v<comparator_8, long double, long double>));
  EXPECT_FALSE((is_invocable_v<comparator_8, int, int>));
  EXPECT_FALSE((is_invocable_v<comparator_8, unsigned long, unsigned long>));

  EXPECT_TRUE((is_nothrow_invocable_v<decltype(ulp_tolerance), std::int16_t>));
  using comparator_16 = invoke_result_t<decltype(ulp_tolerance), std::int16_t>;

  EXPECT_TRUE((is_nothrow_invocable_v<comparator_16, float, float>));
  EXPECT_TRUE((is_nothrow_invocable_v<comparator_16, double, double>));
  EXPECT_FALSE((is_invocable_v<comparator_16, long double, long double>));
  EXPECT_FALSE((is_invocable_v<comparator_16, int, int>));
  EXPECT_FALSE((is_invocable_v<comparator_16, unsigned long, unsigned long>));

  EXPECT_TRUE((is_nothrow_invocable_v<decltype(ulp_tolerance), std::int32_t>));
  using comparator_32 = invoke_result_t<decltype(ulp_tolerance), std::int32_t>;

  EXPECT_TRUE((is_nothrow_invocable_v<comparator_32, float, float>));
  EXPECT_TRUE((is_nothrow_invocable_v<comparator_32, double, double>));
  EXPECT_FALSE((is_invocable_v<comparator_32, long double, long double>));
  EXPECT_FALSE((is_invocable_v<comparator_32, int, int>));
  EXPECT_FALSE((is_invocable_v<comparator_32, unsigned long, unsigned long>));

  EXPECT_TRUE((is_nothrow_invocable_v<decltype(ulp_tolerance), std::int64_t>));
  using comparator_64 = invoke_result_t<decltype(ulp_tolerance), std::int64_t>;

  EXPECT_TRUE((is_nothrow_invocable_v<comparator_64, float, float>));
  EXPECT_TRUE((is_nothrow_invocable_v<comparator_64, double, double>));
  EXPECT_FALSE((is_invocable_v<comparator_64, long double, long double>));
  EXPECT_FALSE((is_invocable_v<comparator_64, int, int>));
  EXPECT_FALSE((is_invocable_v<comparator_64, unsigned long, unsigned long>));
}

/// @test When the tolerance is set to zero, the behaviour is that of exact equality
CONDITIONALLY_CONSTEXPR_TYPED_TEST(
    UlpTolerance,
    ComparatorWithZeroIsExactEquality,
    ARENE_IS_ON(ARENE_HAS_CONSTEXPR_BIT_CAST)
) {
  constexpr TypeParam zero{};
  constexpr TypeParam one{1.0};
  constexpr TypeParam denorm_min = std::numeric_limits<TypeParam>::denorm_min();
  constexpr TypeParam min = std::numeric_limits<TypeParam>::min();
  constexpr TypeParam inf = std::numeric_limits<TypeParam>::infinity();
  constexpr auto zero_tolerance = ulp_tolerance(0);

  CONSTEXPR_ASSERT(zero_tolerance(zero, zero));
  CONSTEXPR_ASSERT(zero_tolerance(one, one));
  CONSTEXPR_ASSERT(zero_tolerance(denorm_min, denorm_min));
  CONSTEXPR_ASSERT(zero_tolerance(min, min));
  CONSTEXPR_ASSERT(zero_tolerance(inf, inf));

  CONSTEXPR_ASSERT(zero_tolerance(-zero, -zero));
  CONSTEXPR_ASSERT(zero_tolerance(-one, -one));
  CONSTEXPR_ASSERT(zero_tolerance(-denorm_min, -denorm_min));
  CONSTEXPR_ASSERT(zero_tolerance(-min, -min));
  CONSTEXPR_ASSERT(zero_tolerance(-inf, -inf));

  CONSTEXPR_ASSERT(zero_tolerance(-zero, zero));
  CONSTEXPR_ASSERT(zero_tolerance(zero, -zero));
  CONSTEXPR_ASSERT_FALSE(zero_tolerance(zero, denorm_min));
  CONSTEXPR_ASSERT_FALSE(zero_tolerance(-zero, denorm_min));
}

/// @test @c ulp_tolerance comparisons are symmetrical in the order of the floating point parameters
CONDITIONALLY_CONSTEXPR_TYPED_TEST(UlpTolerance, ComparisonIsSymmetrical, ARENE_IS_ON(ARENE_HAS_CONSTEXPR_BIT_CAST)) {
  constexpr TypeParam zero{};
  constexpr TypeParam one{1.0};
  constexpr TypeParam denorm_min = std::numeric_limits<TypeParam>::denorm_min();
  constexpr TypeParam min = std::numeric_limits<TypeParam>::min();
  constexpr TypeParam inf = std::numeric_limits<TypeParam>::infinity();
  constexpr auto one_tolerance = ulp_tolerance(1);
  constexpr auto two_tolerance = ulp_tolerance(2);

  CONSTEXPR_ASSERT(one_tolerance(zero, zero));
  CONSTEXPR_ASSERT(one_tolerance(one, one));
  CONSTEXPR_ASSERT(one_tolerance(denorm_min, denorm_min));
  CONSTEXPR_ASSERT(one_tolerance(min, min));
  CONSTEXPR_ASSERT(one_tolerance(inf, inf));

  CONSTEXPR_ASSERT(one_tolerance(-zero, zero));
  CONSTEXPR_ASSERT(one_tolerance(zero, -zero));

  CONSTEXPR_ASSERT(one_tolerance(zero, denorm_min));
  CONSTEXPR_ASSERT(one_tolerance(-zero, denorm_min));
  CONSTEXPR_ASSERT(one_tolerance(denorm_min, zero));
  CONSTEXPR_ASSERT(one_tolerance(denorm_min, -zero));
  CONSTEXPR_ASSERT(one_tolerance(zero, -denorm_min));
  CONSTEXPR_ASSERT(one_tolerance(-zero, -denorm_min));
  CONSTEXPR_ASSERT(one_tolerance(-denorm_min, zero));
  CONSTEXPR_ASSERT(one_tolerance(-denorm_min, -zero));

  CONSTEXPR_ASSERT_FALSE(one_tolerance(zero, 2 * denorm_min));
  CONSTEXPR_ASSERT_FALSE(one_tolerance(-zero, 2 * denorm_min));
  CONSTEXPR_ASSERT_FALSE(one_tolerance(2 * denorm_min, zero));
  CONSTEXPR_ASSERT_FALSE(one_tolerance(2 * denorm_min, -zero));
  CONSTEXPR_ASSERT_FALSE(one_tolerance(zero, -2 * denorm_min));
  CONSTEXPR_ASSERT_FALSE(one_tolerance(-zero, -2 * denorm_min));
  CONSTEXPR_ASSERT_FALSE(one_tolerance(-2 * denorm_min, zero));
  CONSTEXPR_ASSERT_FALSE(one_tolerance(-2 * denorm_min, -zero));

  CONSTEXPR_ASSERT(two_tolerance(zero, 2 * denorm_min));
  CONSTEXPR_ASSERT(two_tolerance(-zero, 2 * denorm_min));
  CONSTEXPR_ASSERT(two_tolerance(2 * denorm_min, zero));
  CONSTEXPR_ASSERT(two_tolerance(2 * denorm_min, -zero));
  CONSTEXPR_ASSERT(two_tolerance(zero, -2 * denorm_min));
  CONSTEXPR_ASSERT(two_tolerance(-zero, -2 * denorm_min));
  CONSTEXPR_ASSERT(two_tolerance(-2 * denorm_min, zero));
  CONSTEXPR_ASSERT(two_tolerance(-2 * denorm_min, -zero));
}

/// @test @c ulp_tolerance comparisons involving NaNs are always false regardless of the tolerance
CONDITIONALLY_CONSTEXPR_TYPED_TEST(UlpTolerance, NanIsAlwaysUnequal, ARENE_IS_ON(ARENE_HAS_CONSTEXPR_BIT_CAST)) {
  constexpr TypeParam zero{};
  constexpr TypeParam qnan = std::numeric_limits<TypeParam>::quiet_NaN();
  constexpr TypeParam snan = std::numeric_limits<TypeParam>::signaling_NaN();

  for (std::int32_t const tol : {std::int32_t{0}, std::int32_t{1}, std::numeric_limits<std::int32_t>::max()}) {
    CONSTEXPR_ASSERT_FALSE(ulp_tolerance(tol)(qnan, zero));
    CONSTEXPR_ASSERT_FALSE(ulp_tolerance(tol)(zero, qnan));
    CONSTEXPR_ASSERT_FALSE(ulp_tolerance(tol)(qnan, qnan));

    CONSTEXPR_ASSERT_FALSE(ulp_tolerance(tol)(snan, zero));
    CONSTEXPR_ASSERT_FALSE(ulp_tolerance(tol)(zero, snan));
    CONSTEXPR_ASSERT_FALSE(ulp_tolerance(tol)(snan, snan));

    CONSTEXPR_ASSERT_FALSE(ulp_tolerance(tol)(qnan, snan));
    CONSTEXPR_ASSERT_FALSE(ulp_tolerance(tol)(snan, qnan));
  }
}

/// @test @c ulp_tolerance comparisons come back false without crashing even when the difference is very large
CONDITIONALLY_CONSTEXPR_TYPED_TEST(
    UlpTolerance,
    HugeDifferenceIsJustNormallyFalse,
    ARENE_IS_ON(ARENE_HAS_CONSTEXPR_BIT_CAST)
) {
  constexpr TypeParam inf = std::numeric_limits<TypeParam>::infinity();

  for (std::int32_t const tol : {std::int32_t{0}, std::int32_t{1}, std::numeric_limits<std::int32_t>::max()}) {
    CONSTEXPR_ASSERT_FALSE(ulp_tolerance(tol)(inf, -inf));
    CONSTEXPR_ASSERT_FALSE(ulp_tolerance(tol)(-inf, inf));
  }
}

/// @test @c ulp_tolerance comparisons come back false when the difference is larger than the tolerance type can hold
CONDITIONALLY_CONSTEXPR_TEST(UlpTolerance, DoubleDifferenceOverflowsEasily, ARENE_IS_ON(ARENE_HAS_CONSTEXPR_BIT_CAST)) {
  constexpr double zero{};
  constexpr double max = std::numeric_limits<double>::max();
  constexpr double inf = std::numeric_limits<double>::infinity();

  for (std::int32_t const tol : {std::int32_t{0}, std::int32_t{1}, std::numeric_limits<std::int32_t>::max()}) {
    // For double, these go out of bounds of what's representable in an int32.
    CONSTEXPR_ASSERT_FALSE(ulp_tolerance(tol)(zero, max));
    CONSTEXPR_ASSERT_FALSE(ulp_tolerance(tol)(max, zero));

    CONSTEXPR_ASSERT_FALSE(ulp_tolerance(tol)(zero, inf));
    CONSTEXPR_ASSERT_FALSE(ulp_tolerance(tol)(inf, zero));

    CONSTEXPR_ASSERT_FALSE(ulp_tolerance(tol)(zero, -max));
    CONSTEXPR_ASSERT_FALSE(ulp_tolerance(tol)(-max, zero));

    CONSTEXPR_ASSERT_FALSE(ulp_tolerance(tol)(zero, -inf));
    CONSTEXPR_ASSERT_FALSE(ulp_tolerance(tol)(-inf, zero));
  }
}

}  // namespace

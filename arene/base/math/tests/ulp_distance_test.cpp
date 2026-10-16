// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/math/ulp_distance.hpp"

#include <gtest/gtest.h>

#include "arene/base/bit/bit_cast.hpp"
#include "arene/base/compiler_support/platform_queries/has_constexpr_bit_cast.hpp"
#include "arene/base/compiler_support/preprocessor/define_configuration.hpp"
#include "arene/base/stdlib_choice/conditional.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "testlibs/utilities/float32.hpp"

/// @cond
template <>
constexpr auto arene::base::enable_ulp_distance_comparable<::testing::float32> = true;
/// @endcond

namespace {

using arene::base::bit_cast;
using arene::base::is_invocable_v;
using arene::base::ulp_distance;
using arene::base::ulp_distance_error;

/// @test @c ulp_distance cannot be used with 'long double'
TEST(UlpDistance, NotInvocableWithLongDouble) {
  static_assert(
      !is_invocable_v<decltype(ulp_distance<>), long double, long double>,
      "must not be invocable with long double"
  );
}

/// @test @c ulp_distance cannot be used with integer types
TEST(UlpDistance, NotInvocableWithIntegers) {
  static_assert(  //
      !is_invocable_v<decltype(ulp_distance<>), int, int>,
      "must not be invocable with integer types"
  );
  static_assert(
      !is_invocable_v<decltype(ulp_distance<>), std::int32_t, std::int64_t>,
      "must not be invocable with integer types"
  );
  static_assert(
      !is_invocable_v<decltype(ulp_distance<>), std::int64_t, std::int64_t>,
      "must not be invocable with integer types"
  );
  static_assert(
      !is_invocable_v<decltype(ulp_distance<>), std::uint32_t, std::uint64_t>,
      "must not be invocable with integer types"
  );
  static_assert(
      !is_invocable_v<decltype(ulp_distance<>), std::uint64_t, std::uint64_t>,
      "must not be invocable with integer types"
  );
}

/// @test @c ulp_distance requires both arguments to be the same
/// floating-point type, regardless of cv-qualification or value category.
TEST(UlpDistance, RequiresMatchingTypes) {
  static_assert(  //
      is_invocable_v<decltype(ulp_distance<>), float, float const>,
      "should accept the same type with different cv-qualifications"
  );
  static_assert(  //
      is_invocable_v<decltype(ulp_distance<>), float&, float&&>,
      "should accept the same type with different value categories"
  );
  static_assert(  //
      !is_invocable_v<decltype(ulp_distance<>), float, double>,
      "must not be invocable with mixed float and double arguments"
  );
}

template <class T>
struct UlpDistance : ::testing::Test {};

using test_types = ::testing::Types<float, double, ::testing::float32>;

TYPED_TEST_SUITE(UlpDistance, test_types, );

/// @test @c ulp_distance returns zero for all combinations of @c +0.0 and
/// @c -0.0, consistent with IEEE 754 equality.
CONDITIONALLY_CONSTEXPR_TYPED_TEST(UlpDistance, AcrossZero, ARENE_IS_ON(ARENE_HAS_CONSTEXPR_BIT_CAST)) {
  using T = TypeParam;

  CONSTEXPR_ASSERT_EQ(0, ulp_distance<>(+T{}, +T{}).value());
  CONSTEXPR_ASSERT_EQ(0, ulp_distance<>(-T{}, +T{}).value());
  CONSTEXPR_ASSERT_EQ(0, ulp_distance<>(+T{}, -T{}).value());
  CONSTEXPR_ASSERT_EQ(0, ulp_distance<>(-T{}, -T{}).value());
}

/// @test @c ulp_distance correctly computes a distance of 1 ULP for
/// adjacent values near 1 using @c epsilon for float and double.
CONDITIONALLY_CONSTEXPR_TYPED_TEST(UlpDistance, EpsilonFromOne, ARENE_IS_ON(ARENE_HAS_CONSTEXPR_BIT_CAST)) {
  using F = std::numeric_limits<TypeParam>;

  constexpr auto eps = F::epsilon();
  constexpr auto one = TypeParam{1};

  CONSTEXPR_ASSERT_EQ(-1, ulp_distance<>(one + eps, one).value());
  CONSTEXPR_ASSERT_EQ(+1, ulp_distance<>(one, one + eps).value());
}

/// @test @c ulp_distance correctly computes large distances spanning the
/// entire representable float range, including between @c max and @c lowest
/// and between positive and negative infinity.
CONDITIONALLY_CONSTEXPR_TEST(UlpDistance, FarAwayFloats, ARENE_IS_ON(ARENE_HAS_CONSTEXPR_BIT_CAST)) {
  using F = std::numeric_limits<float>;

  auto const dist_from_plus_zero = std::int64_t{bit_cast<std::int32_t>(F::max())};

  CONSTEXPR_ASSERT_EQ(-(2 * dist_from_plus_zero), ulp_distance<>(F::max(), F::lowest()).value());
  CONSTEXPR_ASSERT_EQ(+(2 * dist_from_plus_zero), ulp_distance<>(F::lowest(), F::max()).value());

  CONSTEXPR_ASSERT_EQ(-(2 * dist_from_plus_zero + 2), ulp_distance<>(+F::infinity(), -F::infinity()).value());
  CONSTEXPR_ASSERT_EQ(+(2 * dist_from_plus_zero + 2), ulp_distance<>(-F::infinity(), +F::infinity()).value());
}

/// @test passing quiet_NaN to @c ulp_distance results in an error
CONDITIONALLY_CONSTEXPR_TYPED_TEST(UlpDistance, ArgIsQuietNaN, ARENE_IS_ON(ARENE_HAS_CONSTEXPR_BIT_CAST)) {
  using T = TypeParam;
  constexpr auto nan = std::numeric_limits<T>::quiet_NaN();

  CONSTEXPR_ASSERT_EQ(ulp_distance_error::unordered_argument, ulp_distance<>(nan, T{}).error());
  CONSTEXPR_ASSERT_EQ(ulp_distance_error::unordered_argument, ulp_distance<>(T{}, nan).error());
  CONSTEXPR_ASSERT_EQ(ulp_distance_error::unordered_argument, ulp_distance<>(nan, nan).error());
}

/// @test passing signaling_NaN to @c ulp_distance results in an error
CONDITIONALLY_CONSTEXPR_TYPED_TEST(UlpDistance, ArgIsSignalingNaN, ARENE_IS_ON(ARENE_HAS_CONSTEXPR_BIT_CAST)) {
  using T = TypeParam;
  constexpr auto nan = std::numeric_limits<T>::signaling_NaN();

  CONSTEXPR_ASSERT_EQ(ulp_distance_error::unordered_argument, ulp_distance<>(nan, T{}).error());
  CONSTEXPR_ASSERT_EQ(ulp_distance_error::unordered_argument, ulp_distance<>(T{}, nan).error());
  CONSTEXPR_ASSERT_EQ(ulp_distance_error::unordered_argument, ulp_distance<>(nan, nan).error());
}

/// @test using @c ulp_distance where distance is not representable in the
/// explicit result type is a precondition violation
CONDITIONALLY_CONSTEXPR_TEST(UlpDistance, DistanceNotRepresentable, ARENE_IS_ON(ARENE_HAS_CONSTEXPR_BIT_CAST)) {
  constexpr auto inf = std::numeric_limits<double>::infinity();

  CONSTEXPR_ASSERT_EQ(ulp_distance_error::distance_not_representable, ulp_distance<std::int32_t>(-inf, +inf).error());
}

/// @test The maximum positive distance representable as a std::int8_t result is 127 ULPs.
CONDITIONALLY_CONSTEXPR_TYPED_TEST(UlpDistance, PositiveBoundaryForInt8, ARENE_IS_ON(ARENE_HAS_CONSTEXPR_BIT_CAST)) {
  using integer_type = std::conditional_t<sizeof(TypeParam) == 4U, std::int32_t, std::int64_t>;
  constexpr auto bit_cast_to_floating_point = bit_cast<TypeParam>;

  auto const low = bit_cast_to_floating_point(integer_type{0});
  auto const high = bit_cast_to_floating_point(integer_type{127});

  auto const result1 = ulp_distance<std::int8_t>(low, high);
  CONSTEXPR_ASSERT_EQ(127, result1.value());

  auto const too_high = bit_cast_to_floating_point(integer_type{128});
  auto const result2 = ulp_distance<std::int8_t>(low, too_high);
  CONSTEXPR_ASSERT_EQ(ulp_distance_error::distance_not_representable, result2.error());
}

/// @test The maximum negative distance representable as a std::int8_t result is -128 ULPs.
CONDITIONALLY_CONSTEXPR_TYPED_TEST(UlpDistance, NegativeBoundaryForInt8, ARENE_IS_ON(ARENE_HAS_CONSTEXPR_BIT_CAST)) {
  using integer_type = std::conditional_t<sizeof(TypeParam) == 4U, std::int32_t, std::int64_t>;
  constexpr auto bit_cast_to_floating_point = bit_cast<TypeParam>;

  auto const low = bit_cast_to_floating_point(integer_type{0});
  auto const high = bit_cast_to_floating_point(integer_type{128});

  // NOLINTNEXTLINE(readability-suspicious-call-argument) We're deliberately checking the distance from high to low
  auto const result1 = ulp_distance<std::int8_t>(high, low);
  CONSTEXPR_ASSERT_EQ(-128, result1.value());

  auto const too_high = bit_cast_to_floating_point(integer_type{129});
  auto const result2 = ulp_distance<std::int8_t>(too_high, low);
  CONSTEXPR_ASSERT_EQ(ulp_distance_error::distance_not_representable, result2.error());
}

}  // namespace

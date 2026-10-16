// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/math/detail/less_nan_highest.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/attributes/maybe_unused.hpp"
#include "arene/base/math/float_properties.hpp"
#include "arene/base/stdlib_choice/less.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/testing/bitwise_uniform_distribution.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/testing/nan_rejecting_distribution.hpp"
#include "arene/base/testing/property_assertions.hpp"
#include "arene/base/testing/property_macros.hpp"
#include "arene/base/type_list/concat_unique.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "testlibs/utilities/common_test_types.hpp"
#include "testlibs/utilities/configurable_value.hpp"

namespace {

using ::arene::base::is_invocable_v;
using ::arene::base::is_nothrow_invocable_v;
using ::arene::base::math_detail::less_nan_highest;
using arene::base::testing::bitwise_uniform_distribution;
using arene::base::testing::nan_rejecting_distribution;

/// @test @c less_nan_highest is invocable on types that work with @c operator<
TEST(LessNanHighest, InvocableForTypesWithOperatorLess) {
  ASSERT_TRUE((is_invocable_v<decltype(less_nan_highest), int, int>));
  ASSERT_TRUE((is_invocable_v<decltype(less_nan_highest), float, float>));

  struct has_operator_less {
    auto operator<(has_operator_less) const -> bool;
  };
  ASSERT_TRUE((is_invocable_v<decltype(less_nan_highest), has_operator_less, has_operator_less>));

  struct no_operator_less {};
  ASSERT_FALSE((is_invocable_v<decltype(less_nan_highest), no_operator_less, no_operator_less>));
}

struct throws_on_isnan {
  auto operator<(throws_on_isnan) const noexcept -> bool { return true; }
  ARENE_MAYBE_UNUSED friend auto isnan(throws_on_isnan) noexcept(false) -> bool { return true; }
};

/// @test @c less_nan_highest is @c noexcept(false) if the type comparison is @c
/// noexcept(false) or @c isnan is @c noexcept(false)
TEST(LessNanHighest, Noexcept) {
  ASSERT_TRUE((is_invocable_v<decltype(less_nan_highest), int, int>));
  ASSERT_TRUE((is_nothrow_invocable_v<decltype(less_nan_highest), int, int>));

  using nothrow = testing::configurable_value<int, testing::throws_on::nothing>;

  ASSERT_TRUE((is_invocable_v<decltype(less_nan_highest), nothrow, nothrow>));
  ASSERT_TRUE((is_nothrow_invocable_v<decltype(less_nan_highest), nothrow, nothrow>));

  using throws_on_less = testing::configurable_value<int, testing::throws_on::less>;

  ASSERT_TRUE((is_invocable_v<decltype(less_nan_highest), throws_on_less, throws_on_less>));
  ASSERT_FALSE((is_nothrow_invocable_v<decltype(less_nan_highest), throws_on_less, throws_on_less>));

  ASSERT_TRUE((is_invocable_v<decltype(less_nan_highest), throws_on_isnan, throws_on_isnan>));
  ASSERT_FALSE((is_nothrow_invocable_v<decltype(less_nan_highest), throws_on_isnan, throws_on_isnan>));
}

template <class T>
struct LessNanHighestValuesWithoutNan : testing::Test {};

using value_types_without_nan =
    arene::base::type_lists::concat_unique_t<testing::signed_integer_types, testing::unsigned_integer_types>;

TYPED_TEST_SUITE(LessNanHighestValuesWithoutNan, value_types_without_nan, );

/// @test @c less_nan_highest matches @c std::less<> for types without NaN
ARENE_TYPED_PROPERTY(LessNanHighestValuesWithoutNan, MatchesStdLessForTypesWithoutNaN) {
  bitwise_uniform_distribution<TypeParam> const dist{};
  auto const lhs = dist(prng);
  auto const rhs = dist(prng);

  ARENE_PROP_ASSERT(less_nan_highest(lhs, rhs) == std::less<>{}(lhs, rhs));
}

struct type_with_nan {
  float value;

  constexpr auto operator-() const -> type_with_nan { return {-value}; }
  friend constexpr auto operator<(type_with_nan lhs, type_with_nan rhs) -> bool { return lhs.value < rhs.value; }
  friend constexpr auto isnan(type_with_nan arg) -> bool { return arene::base::isnan(arg.value); }
};

template <class T>
struct LessNanHighest : testing::Test {};

template <class Value, class Underlying = Value>
struct test_case {
  using value_type = Value;
  using underlying_type = Underlying;
};

using test_types = ::testing::Types<  //
    test_case<float>,
    test_case<double>,
    test_case<type_with_nan, float>>;

TYPED_TEST_SUITE(LessNanHighest, test_types, );

/// @test @c less_nan_highest matches @c std::less<> for types with NaN, excluding NaN values
ARENE_TYPED_PROPERTY(LessNanHighest, MatchesStdLessExcludingNaNValues) {
  using value_type = typename TypeParam::value_type;
  using underlying_type = typename TypeParam::underlying_type;

  nan_rejecting_distribution<bitwise_uniform_distribution<underlying_type>> const dist{};
  auto const lhs = value_type{dist(prng)};
  auto const rhs = value_type{dist(prng)};

  ARENE_PROP_ASSERT(less_nan_highest(lhs, rhs) == std::less<>{}(lhs, rhs));
}

/// @test NaN is ordered highest with @c less_nan_highest
/// @note comparison with NaN is not constexpr with GCC
CONSTEXPR_TYPED_TEST(LessNanHighest, NanOrderedHighest) {
  using value_type = typename TypeParam::value_type;
  using underlying_type = typename TypeParam::underlying_type;

  auto const nan = value_type{std::numeric_limits<underlying_type>::quiet_NaN()};
  auto const inf = value_type{std::numeric_limits<underlying_type>::infinity()};

  CONSTEXPR_ASSERT(less_nan_highest(value_type{}, nan));
  CONSTEXPR_ASSERT(less_nan_highest(inf, nan));
  CONSTEXPR_ASSERT(less_nan_highest(-value_type{}, nan));
  CONSTEXPR_ASSERT(less_nan_highest(-inf, nan));

  CONSTEXPR_ASSERT_FALSE(less_nan_highest(nan, value_type{}));
  CONSTEXPR_ASSERT_FALSE(less_nan_highest(nan, inf));
  CONSTEXPR_ASSERT_FALSE(less_nan_highest(nan, -value_type{}));
  CONSTEXPR_ASSERT_FALSE(less_nan_highest(nan, -inf));

  CONSTEXPR_ASSERT_FALSE(less_nan_highest(nan, nan));
}

/// @test negative NaN is ordered highest with @c less_nan_highest; it is equivalent to NaN
/// @note comparison with NaN is not constexpr with GCC
CONSTEXPR_TYPED_TEST(LessNanHighest, NegativeNaNEquivalentToNaN) {
  using value_type = typename TypeParam::value_type;
  using underlying_type = typename TypeParam::underlying_type;

  auto const nan = value_type{std::numeric_limits<underlying_type>::quiet_NaN()};
  auto const inf = value_type{std::numeric_limits<underlying_type>::infinity()};

  CONSTEXPR_ASSERT(less_nan_highest(value_type{}, -nan));
  CONSTEXPR_ASSERT(less_nan_highest(inf, -nan));
  CONSTEXPR_ASSERT(less_nan_highest(-value_type{}, -nan));
  CONSTEXPR_ASSERT(less_nan_highest(-inf, -nan));

  CONSTEXPR_ASSERT_FALSE(less_nan_highest(-nan, value_type{}));
  CONSTEXPR_ASSERT_FALSE(less_nan_highest(-nan, inf));
  CONSTEXPR_ASSERT_FALSE(less_nan_highest(-nan, -value_type{}));
  CONSTEXPR_ASSERT_FALSE(less_nan_highest(-nan, -inf));

  CONSTEXPR_ASSERT_FALSE(less_nan_highest(-nan, -nan));
  CONSTEXPR_ASSERT_FALSE(less_nan_highest(nan, -nan));
  CONSTEXPR_ASSERT_FALSE(less_nan_highest(-nan, nan));
}

}  // namespace

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/testing/nan_rejecting_distribution.hpp"

#include <gtest/gtest.h>  // IWYU pragma: keep

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/compiler_support/platform_queries/has_constexpr_bit_cast.hpp"
#include "arene/base/compiler_support/preprocessor/define_configuration.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/make_unsigned.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/testing/bitwise_uniform_distribution.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/testing/prng_xoshiro.hpp"
#include "arene/base/testing/tests/float_test_patterns.hpp"
#include "arene/base/testing/tests/sequenced_mock_prng.hpp"

namespace {

using arene::base::testing::bitwise_uniform_distribution;
using arene::base::testing::nan_rejecting_distribution;
using arene::base::testing::prng_xoshiro;
using arene::base::testing::tests::float_test_patterns;
using arene::base::testing::tests::sequenced_mock_prng;

template <class T>
class NanRejectingDistributionTest : public ::testing::Test {};

using floating_point_types = ::testing::Types<float, double>;
TYPED_TEST_SUITE(NanRejectingDistributionTest, floating_point_types, );

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "These tests don't perform arithmetic, so equality is OK even for floating point");

/// @test Non-NaN values pass through the adaptor unchanged
CONDITIONALLY_CONSTEXPR_TYPED_TEST(
    NanRejectingDistributionTest,
    PassesThroughNonNan,
    ARENE_IS_ON(ARENE_HAS_CONSTEXPR_BIT_CAST)
) {
  using value_type = TypeParam;
  using patterns = float_test_patterns<value_type>;
  using bits_type = typename patterns::bits_type;
  using inner_type = bitwise_uniform_distribution<value_type>;

  auto const dist = nan_rejecting_distribution<inner_type>{inner_type{}};
  sequenced_mock_prng<bits_type>
      prng{patterns::one, patterns::zero, patterns::negative_one, patterns::positive_infinity, patterns::denorm_min};

  CONSTEXPR_ASSERT_EQ(dist(prng), static_cast<value_type>(1));
  CONSTEXPR_ASSERT_EQ(dist(prng), static_cast<value_type>(0));
  CONSTEXPR_ASSERT_EQ(dist(prng), static_cast<value_type>(-1));
  CONSTEXPR_ASSERT_EQ(dist(prng), std::numeric_limits<value_type>::infinity());
  CONSTEXPR_ASSERT_EQ(dist(prng), std::numeric_limits<value_type>::denorm_min());
}

/// @test A single NaN draw is discarded and the next value is returned
CONDITIONALLY_CONSTEXPR_TYPED_TEST(
    NanRejectingDistributionTest,
    DiscardsNanAndRedraws,
    ARENE_IS_ON(ARENE_HAS_CONSTEXPR_BIT_CAST)
) {
  using value_type = TypeParam;
  using patterns = float_test_patterns<value_type>;
  using bits_type = typename patterns::bits_type;
  using inner_type = bitwise_uniform_distribution<value_type>;

  auto const dist = nan_rejecting_distribution<inner_type>{inner_type{}};
  sequenced_mock_prng<bits_type> prng{patterns::quiet_nan, patterns::one};

  CONSTEXPR_ASSERT_EQ(dist(prng), static_cast<value_type>(1));
}

/// @test A run of consecutive NaN draws is discarded before a non-NaN is returned
CONDITIONALLY_CONSTEXPR_TYPED_TEST(
    NanRejectingDistributionTest,
    DiscardsRunOfNans,
    ARENE_IS_ON(ARENE_HAS_CONSTEXPR_BIT_CAST)
) {
  using value_type = TypeParam;
  using patterns = float_test_patterns<value_type>;
  using bits_type = typename patterns::bits_type;
  using inner_type = bitwise_uniform_distribution<value_type>;

  auto const dist = nan_rejecting_distribution<inner_type>{inner_type{}};
  sequenced_mock_prng<bits_type> prng{patterns::quiet_nan, patterns::quiet_nan, patterns::quiet_nan, patterns::one};

  CONSTEXPR_ASSERT_EQ(dist(prng), static_cast<value_type>(1));
}

/// @test Both quiet and signaling NaN values are discarded
CONDITIONALLY_CONSTEXPR_TYPED_TEST(
    NanRejectingDistributionTest,
    DiscardsQuietAndSignalingNan,
    ARENE_IS_ON(ARENE_HAS_CONSTEXPR_BIT_CAST)
) {
  using value_type = TypeParam;
  using patterns = float_test_patterns<value_type>;
  using bits_type = typename patterns::bits_type;
  using inner_type = bitwise_uniform_distribution<value_type>;

  auto const dist = nan_rejecting_distribution<inner_type>{inner_type{}};
  sequenced_mock_prng<bits_type> prng{patterns::signaling_nan, patterns::quiet_nan, patterns::one};

  CONSTEXPR_ASSERT_EQ(dist(prng), static_cast<value_type>(1));
}

/// @test Negative NaN values (sign bit set) are also discarded
CONDITIONALLY_CONSTEXPR_TYPED_TEST(
    NanRejectingDistributionTest,
    DiscardsNegativeNan,
    ARENE_IS_ON(ARENE_HAS_CONSTEXPR_BIT_CAST)
) {
  using value_type = TypeParam;
  using patterns = float_test_patterns<value_type>;
  using bits_type = typename patterns::bits_type;
  using inner_type = bitwise_uniform_distribution<value_type>;

  auto const dist = nan_rejecting_distribution<inner_type>{inner_type{}};
  sequenced_mock_prng<bits_type> prng{patterns::negative_nan, patterns::one};

  CONSTEXPR_ASSERT_EQ(dist(prng), static_cast<value_type>(1));
}

/// @test The adaptor's @c operator() noexcept-specification matches the wrapped distribution's
TYPED_TEST(NanRejectingDistributionTest, NoexceptSpecMatchesInnerDistribution) {
  using value_type = TypeParam;
  using inner_type = bitwise_uniform_distribution<value_type>;
  using adaptor_type = nan_rejecting_distribution<inner_type>;

  STATIC_ASSERT_EQ(
      noexcept(std::declval<adaptor_type const&>()(std::declval<prng_xoshiro&>())),
      noexcept(std::declval<inner_type const&>()(std::declval<prng_xoshiro&>()))
  );
}

ARENE_IGNORE_END();

template <class T>
class NanRejectingDistributionIntegerTest : public ::testing::Test {};

// NOLINTBEGIN(google-runtime-int) We want to test with all *distinct types*, not all *bit widths*
using integer_types = ::testing::Types<
    char,
    signed char,
    short,
    int,
    long,
    long long,
    unsigned char,
    unsigned short,
    unsigned int,
    unsigned long,
    unsigned long long>;
// NOLINTEND(google-runtime-int)
TYPED_TEST_SUITE(NanRejectingDistributionIntegerTest, integer_types, );

/// @test When used with an integral type, @c nan_rejecting_distribution passes the generated value through unmodified
TYPED_TEST(NanRejectingDistributionIntegerTest, IntegerValuesArePassedThrough) {
  using inner_type = bitwise_uniform_distribution<TypeParam>;
  auto const dist = nan_rejecting_distribution<inner_type>{inner_type{}};

  std::initializer_list<std::make_unsigned_t<TypeParam>> const values{0x0U, 0x1U, 0xFU, 0xF0U, 0xFFU};
  sequenced_mock_prng<std::make_unsigned_t<TypeParam>> prng{values};

  for (auto value : values) {
    CONSTEXPR_ASSERT_EQ(dist(prng), static_cast<TypeParam>(value));
  }
}

}  // namespace

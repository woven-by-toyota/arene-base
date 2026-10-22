// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/math/complex_norm.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/math/abs_cpo.hpp"
#include "arene/base/math/arithmetic_identities.hpp"
#include "arene/base/math/tests/complex_utilities.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"
#include "arene/base/stdlib_choice/is_signed.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/testing/bitwise_uniform_distribution.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/testing/nan_rejecting_distribution.hpp"
#include "arene/base/testing/property_assertions.hpp"
#include "arene/base/testing/property_macros.hpp"
#include "arene/base/type_list/concat_unique.hpp"
#include "testlibs/utilities/configurable_value.hpp"

namespace testing {

// @c abs function for <c>configurable_value</c>; needs to be defined in the @c testing namespace to be found by ADL
template <class T, throws_on ThrowsOn, disable Disable, is_constexpr Constexpr>
constexpr auto abs(configurable_value<T, ThrowsOn, Disable, Constexpr> const& value
) noexcept(noexcept(arene::base::math::abs(value.value))) -> configurable_value<T, ThrowsOn, Disable, Constexpr> {
  return configurable_value<T, ThrowsOn, Disable, Constexpr>{arene::base::math::abs(value.value)};
}

}  // namespace testing

namespace {

namespace math = ::arene::base::math;

using arene::base::complex_l1_norm;
using arene::base::numbers::zero_v;
using arene::base::testing::bitwise_uniform_distribution;
using arene::base::testing::nan_rejecting_distribution;
using complex_testing::complex_with_adl;
using complex_testing::complex_with_members;

template <class T>
auto is_not_lowest_if_signed_integer(T value) -> bool {
  constexpr bool is_signed_integer = std::is_signed<T>::value && std::is_integral<T>::value;
  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "Exact equality is correct and safe to determine the lowest signed integer value.");
  return !(is_signed_integer && (value == std::numeric_limits<T>::lowest()));
  ARENE_IGNORE_END();
}

template <class T>
class ComplexNormTest : public testing::Test {};

// NOLINTBEGIN(google-runtime-int) Testing with all distinct types rather than distinct bit widths
using signed_integer_types = ::testing::Types<signed char, short, int, long, long long>;
using unsigned_integer_types =
    ::testing::Types<unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long>;
// NOLINTEND(google-runtime-int)
using integral_types = arene::base::type_lists::concat_unique_t<signed_integer_types, unsigned_integer_types>;

using floating_point_types = ::testing::Types<float, double>;
using arithmetic_types = arene::base::type_lists::concat_unique_t<integral_types, floating_point_types>;

TYPED_TEST_SUITE(ComplexNormTest, arithmetic_types, );

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "This is a single arithmetic step that's reproducible enough to assert");

/// @test @c complex_l1_norm on a purely real complex number returns the absolute value of the real part
ARENE_TYPED_PROPERTY(ComplexNormTest, PureRealReturnsAbsOfRealPart) {
  nan_rejecting_distribution<bitwise_uniform_distribution<TypeParam>> const dist{};
  TypeParam const real_part{dist(prng)};
  TypeParam const imag_part{zero_v<TypeParam>};

  ARENE_PROP_PRE(is_not_lowest_if_signed_integer(real_part));

  TypeParam const unwrapped{real_part};
  complex_with_members<TypeParam> const with_members{real_part, imag_part};
  complex_with_adl<TypeParam> const with_adl{real_part, imag_part};

  ARENE_PROP_ASSERT(complex_l1_norm(unwrapped) == math::abs(real_part));
  ARENE_PROP_ASSERT(complex_l1_norm(with_members) == math::abs(real_part));
  ARENE_PROP_ASSERT(complex_l1_norm(with_adl) == math::abs(real_part));
}

/// @test @c complex_l1_norm on a purely imaginary complex number returns the absolute value of the imaginary part
ARENE_TYPED_PROPERTY(ComplexNormTest, PureImaginaryReturnsAbsOfImaginaryPart) {
  nan_rejecting_distribution<bitwise_uniform_distribution<TypeParam>> const dist{};
  TypeParam const real_part{zero_v<TypeParam>};
  TypeParam const imag_part{dist(prng)};

  ARENE_PROP_PRE(is_not_lowest_if_signed_integer(imag_part));

  TypeParam const unwrapped{real_part};
  complex_with_members<TypeParam> const with_members{real_part, imag_part};
  complex_with_adl<TypeParam> const with_adl{real_part, imag_part};

  ARENE_PROP_ASSERT(complex_l1_norm(unwrapped) == zero_v<TypeParam>);
  ARENE_PROP_ASSERT(complex_l1_norm(with_members) == math::abs(imag_part));
  ARENE_PROP_ASSERT(complex_l1_norm(with_adl) == math::abs(imag_part));
}

/// @test @c complex_l1_norm on an arbitrary complex number returns the sum of absolute values of its parts
ARENE_TYPED_PROPERTY(ComplexNormTest, ComplexReturnsSumOfAbsOfParts) {
  constexpr TypeParam max_random{100};
  constexpr TypeParam random_divisor = std::numeric_limits<TypeParam>::max() / max_random;

  nan_rejecting_distribution<bitwise_uniform_distribution<TypeParam>> const dist{};
  auto const real_part = static_cast<TypeParam>(dist(prng) / random_divisor);
  auto const imag_part = static_cast<TypeParam>(dist(prng) / random_divisor);

  ARENE_PROP_PRE(is_not_lowest_if_signed_integer(real_part));
  ARENE_PROP_PRE(is_not_lowest_if_signed_integer(imag_part));

  TypeParam const unwrapped{real_part};
  complex_with_members<TypeParam> const with_members{real_part, imag_part};
  complex_with_adl<TypeParam> const with_adl{real_part, imag_part};

  ARENE_PROP_ASSERT(complex_l1_norm(unwrapped) == math::abs(real_part));
  ARENE_PROP_ASSERT(complex_l1_norm(with_members) == math::abs(real_part) + math::abs(imag_part));
  ARENE_PROP_ASSERT(complex_l1_norm(with_adl) == math::abs(real_part) + math::abs(imag_part));
}

ARENE_IGNORE_END();

/// @test If @c real or @c imag are @c noexcept(false) for a type then its @c complex_l1_norm is @c noexcept(false) too
TYPED_TEST(ComplexNormTest, ComplexOneNormIsNoexceptFalseIfRealOrImagAre) {
  using non_throwing_type = ::testing::configurable_value<TypeParam>;
  using throwing_type = ::testing::configurable_value<TypeParam, ::testing::throws_on::everything>;

  // complex_l1_norm is noexcept(true) when the underlying type and the complex accessors are
  STATIC_ASSERT_TRUE(noexcept(complex_l1_norm(std::declval<non_throwing_type const&>())));
  STATIC_ASSERT_TRUE(noexcept(complex_l1_norm(std::declval<complex_with_members<non_throwing_type> const&>())));
  STATIC_ASSERT_TRUE(noexcept(complex_l1_norm(std::declval<complex_with_adl<non_throwing_type> const&>())));

  // complex_l1_norm needs to call real and imag on complex types, so its noexcept specification is affected by those
  // functions, even when the contained value type is entirely noexcept.
  STATIC_ASSERT_FALSE(noexcept(complex_l1_norm(std::declval<complex_with_members<non_throwing_type, false> const&>())));
  STATIC_ASSERT_FALSE(noexcept(complex_l1_norm(std::declval<complex_with_adl<non_throwing_type, false> const&>())));

  // Even if real and imag don't throw themselves, doing complex_l1_norm on a complex value requires an addition of the
  // underlying type, so the norm can throw if the addition can. Real values don't have this addition.
  STATIC_ASSERT_TRUE(noexcept(complex_l1_norm(std::declval<throwing_type const&>())));
  STATIC_ASSERT_FALSE(noexcept(complex_l1_norm(std::declval<complex_with_members<throwing_type> const&>())));
  STATIC_ASSERT_FALSE(noexcept(complex_l1_norm(std::declval<complex_with_adl<throwing_type> const&>())));
}

}  // namespace

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/math/complex_decomp.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/math/arithmetic_identities.hpp"
#include "arene/base/math/tests/complex_utilities.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/testing/bitwise_uniform_distribution.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/testing/nan_rejecting_distribution.hpp"
#include "arene/base/testing/property_assertions.hpp"
#include "arene/base/testing/property_macros.hpp"
#include "arene/base/type_list/concat_unique.hpp"
#include "testlibs/utilities/configurable_value.hpp"

// The complex decomposition properties compare generated values with exact equality, so NaN draws must be rejected.
/// @cond
template <>
constexpr auto arene::base::testing::default_distribution<float> =
    arene::base::testing::nan_rejecting_distribution<arene::base::testing::bitwise_uniform_distribution<float>>{};
template <>
constexpr auto arene::base::testing::default_distribution<double> =
    arene::base::testing::nan_rejecting_distribution<arene::base::testing::bitwise_uniform_distribution<double>>{};
/// @endcond

namespace {

namespace math = ::arene::base::math;

using complex_testing::complex_with_adl;
using complex_testing::complex_with_members;
using complex_testing::configurable_complex;
using complex_testing::real_imag_tag;

template <class T>
class ComplexDecompTest : public testing::Test {};

using character_types = ::testing::Types<char, signed char, unsigned char, wchar_t, char16_t, char32_t>;
// NOLINTBEGIN(google-runtime-int) Testing with all distinct types rather than distinct bit widths
using signed_integer_types = ::testing::Types<signed char, short, int, long, long long>;
using unsigned_integer_types =
    ::testing::Types<unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long>;
// NOLINTEND(google-runtime-int)
using integral_types =
    arene::base::type_lists::concat_unique_t<signed_integer_types, unsigned_integer_types, character_types>;

using floating_point_types = ::testing::Types<float, double>;
using arithmetic_types = arene::base::type_lists::concat_unique_t<integral_types, floating_point_types>;

TYPED_TEST_SUITE(ComplexDecompTest, arithmetic_types, );

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "No arithmetic is performed so exact equality is OK even for floating point");

/// @test Calling @c math::real on any built-in type gets the real part and @c math::imag gets the imaginary part
ARENE_TYPED_PROPERTY(ComplexDecompTest, RealAndImagGetParts, (TypeParam real_part, TypeParam imag_part)) {
  TypeParam const unwrapped{real_part};
  complex_with_members<TypeParam> const with_members{real_part, imag_part};
  complex_with_adl<TypeParam> const with_adl{real_part, imag_part};

  ARENE_PROP_ASSERT(math::real(unwrapped) == real_part);
  ARENE_PROP_ASSERT(math::real(with_members) == real_part);
  ARENE_PROP_ASSERT(math::real(with_adl) == real_part);

  ARENE_PROP_ASSERT(math::imag(unwrapped) == arene::base::numbers::zero_v<TypeParam>);
  ARENE_PROP_ASSERT(math::imag(with_members) == imag_part);
  ARENE_PROP_ASSERT(math::imag(with_adl) == imag_part);
}

/// @test Calling @c math::real on a user-defined type gets the real part and @c math::imag gets the imaginary part
ARENE_TYPED_PROPERTY(
    ComplexDecompTest,
    RealAndImagGetPartsForUserDefinedType,
    (TypeParam generated_real, TypeParam generated_imag)
) {
  using user_type = ::testing::configurable_value<TypeParam>;

  user_type const real_part{generated_real};
  user_type const imag_part{generated_imag};

  user_type const& unwrapped{real_part};
  complex_with_members<user_type> const with_members{real_part, imag_part};
  complex_with_adl<user_type> const with_adl{real_part, imag_part};

  ARENE_PROP_ASSERT(math::real(unwrapped) == real_part);
  ARENE_PROP_ASSERT(math::real(with_members) == real_part);
  ARENE_PROP_ASSERT(math::real(with_adl) == real_part);

  ARENE_PROP_ASSERT(math::imag(unwrapped) == arene::base::numbers::zero_v<user_type>);
  ARENE_PROP_ASSERT(math::imag(with_members) == imag_part);
  ARENE_PROP_ASSERT(math::imag(with_adl) == imag_part);
}

/// @test A type with only one member overload will fall back to ADL rather than use that one
ARENE_TYPED_PROPERTY(
    ComplexDecompTest,
    PartialMemberImplementationIsNotSelected,
    (TypeParam member_real_part, TypeParam member_imag_part, TypeParam adl_real_part, TypeParam adl_imag_part)
) {
  using member_real_only_t = configurable_complex<TypeParam, real_imag_tag::real, real_imag_tag::both>;
  using member_imag_only_t = configurable_complex<TypeParam, real_imag_tag::imag, real_imag_tag::both>;

  member_real_only_t const member_real_only{member_real_part, member_imag_part, adl_real_part, adl_imag_part};
  member_imag_only_t const member_imag_only{member_real_part, member_imag_part, adl_real_part, adl_imag_part};

  ARENE_PROP_ASSERT(math::real(member_real_only) == adl_real_part);
  ARENE_PROP_ASSERT(math::real(member_imag_only) == adl_real_part);

  ARENE_PROP_ASSERT(math::imag(member_real_only) == adl_imag_part);
  ARENE_PROP_ASSERT(math::imag(member_imag_only) == adl_imag_part);
}

/// @test A type with neither a complete set of member or ADL overloads will be treated as real
ARENE_TYPED_PROPERTY(
    ComplexDecompTest,
    MustHaveOneFullImplementation,
    (TypeParam member_real_part, TypeParam member_imag_part, TypeParam adl_real_part, TypeParam adl_imag_part)
) {
  using real_imag_t = configurable_complex<TypeParam, real_imag_tag::real, real_imag_tag::imag>;
  using imag_real_t = configurable_complex<TypeParam, real_imag_tag::imag, real_imag_tag::real>;
  using none_real_t = configurable_complex<TypeParam, real_imag_tag::none, real_imag_tag::real>;
  using none_imag_t = configurable_complex<TypeParam, real_imag_tag::none, real_imag_tag::imag>;
  using real_none_t = configurable_complex<TypeParam, real_imag_tag::real, real_imag_tag::none>;
  using imag_none_t = configurable_complex<TypeParam, real_imag_tag::imag, real_imag_tag::none>;

  real_imag_t const real_imag{member_real_part, member_imag_part, adl_real_part, adl_imag_part};
  imag_real_t const imag_real{member_real_part, member_imag_part, adl_real_part, adl_imag_part};
  none_real_t const none_real{member_real_part, member_imag_part, adl_real_part, adl_imag_part};
  none_imag_t const none_imag{member_real_part, member_imag_part, adl_real_part, adl_imag_part};
  real_none_t const real_none{member_real_part, member_imag_part, adl_real_part, adl_imag_part};
  imag_none_t const imag_none{member_real_part, member_imag_part, adl_real_part, adl_imag_part};

  // These types are all treated as not complex, so we just get back the original input from math::real
  ARENE_PROP_ASSERT(math::real(real_imag).real() == member_real_part);
  ARENE_PROP_ASSERT(math::real(imag_real).imag() == member_imag_part);
  ARENE_PROP_ASSERT(real(math::real(none_real)) == adl_real_part);
  ARENE_PROP_ASSERT(imag(math::real(none_imag)) == adl_imag_part);
  ARENE_PROP_ASSERT(math::real(real_none).real() == member_real_part);
  ARENE_PROP_ASSERT(math::real(imag_none).imag() == member_imag_part);

  // These types are all treated as not complex, so we get zero_v from math::imag
  ARENE_PROP_ASSERT(math::imag(real_imag).real() == arene::base::numbers::zero_v<TypeParam>);
  ARENE_PROP_ASSERT(math::imag(imag_real).imag() == arene::base::numbers::zero_v<TypeParam>);
  ARENE_PROP_ASSERT(real(math::imag(none_real)) == arene::base::numbers::zero_v<TypeParam>);
  ARENE_PROP_ASSERT(imag(math::imag(none_imag)) == arene::base::numbers::zero_v<TypeParam>);
  ARENE_PROP_ASSERT(math::imag(real_none).real() == arene::base::numbers::zero_v<TypeParam>);
  ARENE_PROP_ASSERT(math::imag(imag_none).imag() == arene::base::numbers::zero_v<TypeParam>);
}

ARENE_IGNORE_END();

/// @test @c math::real and @c math::imag are @c noexcept on any built-in arithmetic type
TYPED_TEST(ComplexDecompTest, RealAndImagAreNoexceptOnBuiltInTypes) {
  STATIC_ASSERT_TRUE(noexcept(math::real(std::declval<TypeParam const&>())));
  STATIC_ASSERT_TRUE(noexcept(math::real(std::declval<complex_with_members<TypeParam> const&>())));
  STATIC_ASSERT_TRUE(noexcept(math::real(std::declval<complex_with_adl<TypeParam> const&>())));

  STATIC_ASSERT_TRUE(noexcept(math::imag(std::declval<TypeParam const&>())));
  STATIC_ASSERT_TRUE(noexcept(math::imag(std::declval<complex_with_members<TypeParam> const&>())));
  STATIC_ASSERT_TRUE(noexcept(math::imag(std::declval<complex_with_adl<TypeParam> const&>())));
}

/// @test @c math::real and @c math::imag have the expected @c noexcept on user-defined types
TYPED_TEST(ComplexDecompTest, RealAndImagHaveCorrectNoexceptOnUserDefinedTypes) {
  using non_throwing_type = ::testing::configurable_value<TypeParam>;

  STATIC_ASSERT_TRUE(noexcept(math::real(std::declval<non_throwing_type const&>())));
  STATIC_ASSERT_TRUE(noexcept(math::real(std::declval<complex_with_members<non_throwing_type> const&>())));
  STATIC_ASSERT_TRUE(noexcept(math::real(std::declval<complex_with_adl<non_throwing_type> const&>())));

  STATIC_ASSERT_TRUE(noexcept(math::imag(std::declval<non_throwing_type const&>())));
  STATIC_ASSERT_TRUE(noexcept(math::imag(std::declval<complex_with_members<non_throwing_type> const&>())));
  STATIC_ASSERT_TRUE(noexcept(math::imag(std::declval<complex_with_adl<non_throwing_type> const&>())));

  // real and imag on a non-complex type return refs, so they're noexcept even when every operation on the type throws.
  using throwing_type = ::testing::configurable_value<TypeParam, ::testing::throws_on::everything>;
  STATIC_ASSERT_TRUE(noexcept(math::real(std::declval<throwing_type const&>())));
  STATIC_ASSERT_TRUE(noexcept(math::imag(std::declval<throwing_type const&>())));

  // real and imag on complex types need to call the underlying real and imag functions, so their noexcept
  // specifications match those functions, even when the contained value type is entirely noexcept.
  STATIC_ASSERT_FALSE(noexcept(math::real(std::declval<complex_with_members<non_throwing_type, false> const&>())));
  STATIC_ASSERT_FALSE(noexcept(math::real(std::declval<complex_with_adl<non_throwing_type, false> const&>())));
  STATIC_ASSERT_FALSE(noexcept(math::imag(std::declval<complex_with_members<non_throwing_type, false> const&>())));
  STATIC_ASSERT_FALSE(noexcept(math::imag(std::declval<complex_with_adl<non_throwing_type, false> const&>())));
}

/// @test @c is_complex_v is @c true for types with either a full set of member or ADL @c real and @c imag functions
TYPED_TEST(ComplexDecompTest, TypeTraitWorks) {
  STATIC_ASSERT_FALSE(arene::base::math::is_complex_v<TypeParam>);
  STATIC_ASSERT_TRUE(arene::base::math::is_complex_v<complex_with_members<TypeParam>>);
  STATIC_ASSERT_TRUE(arene::base::math::is_complex_v<complex_with_adl<TypeParam>>);
}

}  // namespace

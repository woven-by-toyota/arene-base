// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/math/detail/abs_if_needed.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/math/abs.hpp"
#include "arene/base/math/float_properties.hpp"
#include "arene/base/math/float_sign.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

using ::arene::base::math_detail::abs_if_needed;

template <class T>
class AbsIfNeededUnsignedTest : public testing::Test {};
template <class T>
class AbsIfNeededSignedTest : public testing::Test {};
template <class T>
class AbsIfNeededFloatTest : public testing::Test {};

// NOLINTBEGIN(google-runtime-int)
using unsigned_integer_types =
    ::testing::Types<unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long>;

using signed_integer_types = ::testing::Types<signed char, short, int, long, long long>;
// NOLINTEND(google-runtime-int)

using floating_point_types = ::testing::Types<float, double>;

TYPED_TEST_SUITE(AbsIfNeededUnsignedTest, unsigned_integer_types, );
TYPED_TEST_SUITE(AbsIfNeededSignedTest, signed_integer_types, );
TYPED_TEST_SUITE(AbsIfNeededFloatTest, floating_point_types, );

/// @test Calling @c abs_if_needed on an unsigned integer type does nothing
CONSTEXPR_TYPED_TEST(AbsIfNeededUnsignedTest, AbsIsNoOp) {
  CONSTEXPR_ASSERT_EQ(abs_if_needed(TypeParam{0}), TypeParam{0});
  CONSTEXPR_ASSERT_EQ(abs_if_needed(TypeParam{1}), TypeParam{1});
  CONSTEXPR_ASSERT_EQ(abs_if_needed(TypeParam{100}), TypeParam{100});

  CONSTEXPR_ASSERT(noexcept(abs_if_needed(TypeParam{0})));
}

/// @test Calling @c abs_if_needed on a signed integer type that happens to be positive returns the input
CONSTEXPR_TYPED_TEST(AbsIfNeededSignedTest, AbsOfNonNegativeIsNonNegative) {
  CONSTEXPR_ASSERT_EQ(abs_if_needed(TypeParam{0}), TypeParam{0});
  CONSTEXPR_ASSERT_EQ(abs_if_needed(TypeParam{1}), TypeParam{1});
  CONSTEXPR_ASSERT_EQ(abs_if_needed(TypeParam{100}), TypeParam{100});

  CONSTEXPR_ASSERT(noexcept(abs_if_needed(TypeParam{0})));
}

/// @test Calling @c abs_if_needed on a signed integer type that happens to be negative returns the absolute value
CONSTEXPR_TYPED_TEST(AbsIfNeededSignedTest, AbsOfNegativeIsPositive) {
  CONSTEXPR_ASSERT_EQ(abs_if_needed(TypeParam{-1}), TypeParam{1});
  CONSTEXPR_ASSERT_EQ(abs_if_needed(TypeParam{-100}), TypeParam{100});
}

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "Explicitly testing the results of an arithmetic operation");

/// @brief Helper function to test floating point numbers
/// @tparam TestAs Convert the input literal type to this type before calling @c abs_if_needed
/// @tparam TypeOfLiteral The type of the literal, used to avoid implicit conversion warnings
/// @param value The value to test, probably populated by a literal in the test case
/// @param should_negate Whether or not @c abs_if_needed is expected to negate @c value
template <typename TestAs, typename TypeOfLiteral>
constexpr auto test_float_equality(TypeOfLiteral value, bool should_negate) noexcept -> void {
  TestAs abs_result = abs_if_needed(static_cast<TestAs>(value));

  CONSTEXPR_ASSERT_EQ(abs_result, static_cast<TestAs>(should_negate ? -value : value));
  CONSTEXPR_ASSERT_EQ(arene::base::signbit(abs_result), false);
  CONSTEXPR_ASSERT_EQ(arene::base::isnan(abs_result), false);

  CONSTEXPR_ASSERT(noexcept(abs_if_needed(static_cast<TestAs>(value))));
}

ARENE_IGNORE_END();

/// @test Calling @c abs_if_needed on a floating point type that happens to be positive returns the input
CONSTEXPR_TYPED_TEST(AbsIfNeededFloatTest, AbsOfPositiveIsPositive) {
  test_float_equality<TypeParam>(0.0, false);
  test_float_equality<TypeParam>(1.0, false);
  test_float_equality<TypeParam>(100.0, false);

  test_float_equality<TypeParam>(0.5, false);
  test_float_equality<TypeParam>(1.20202, false);
  test_float_equality<TypeParam>(8.125, false);

  test_float_equality<TypeParam>(std::numeric_limits<TypeParam>::infinity(), false);
}

/// @test Calling @c abs_if_needed on a floating point type that happens to be negative returns the absolute value
CONSTEXPR_TYPED_TEST(AbsIfNeededFloatTest, AbsOfNegativeIsPositive) {
  test_float_equality<TypeParam>(-0.0, true);
  test_float_equality<TypeParam>(-1.0, true);
  test_float_equality<TypeParam>(-100.0, true);

  test_float_equality<TypeParam>(-0.5, true);
  test_float_equality<TypeParam>(-1.20202, true);
  test_float_equality<TypeParam>(-8.125, true);

  test_float_equality<TypeParam>(-std::numeric_limits<TypeParam>::infinity(), true);
}

/// @test Calling @c abs_if_needed on a floating point type that happens to be NaN returns a NaN of some kind
CONSTEXPR_TYPED_TEST(AbsIfNeededFloatTest, AbsOfNanIsNan) {
  CONSTEXPR_ASSERT(arene::base::isnan(abs_if_needed(std::numeric_limits<TypeParam>::quiet_NaN())));
  CONSTEXPR_ASSERT(arene::base::isnan(abs_if_needed(std::numeric_limits<TypeParam>::signaling_NaN())));
}

/// @brief A wrapped integer that only has @c operator== and @c abs
template <typename T>
struct wrapped {
  T value;

  constexpr auto operator==(wrapped const& other) const noexcept -> bool { return value == other.value; }
};

/// @brief ADL @c abs implementation for our wrapped integer type
constexpr auto abs(wrapped<int> wrapped_value) noexcept -> wrapped<int> {
  return {arene::base::abs(wrapped_value.value)};
}

/// @test Calling @c abs_if_needed on a custom type with an ADL @c abs function uses that ADL @c abs function
CONSTEXPR_TEST(AbsIfNeededTest, CustomTypeWithAbs) {
  CONSTEXPR_ASSERT_EQ(abs_if_needed(wrapped<int>{0}), wrapped<int>{0});
  CONSTEXPR_ASSERT_EQ(abs_if_needed(wrapped<int>{3}), wrapped<int>{3});
  CONSTEXPR_ASSERT_EQ(abs_if_needed(wrapped<int>{-8}), wrapped<int>{8});

  CONSTEXPR_ASSERT(noexcept(abs_if_needed(wrapped<int>{0})));
}

/// @brief ADS @c abs implementation that can throw, so we can check that the noexcept specification propagates
///
/// @note This is not @c constexpr in order to work around a GCC8 bug which sometimes treats @c constexpr functions as
/// also being @c noexcept even when they're marked otherwise
// NOLINTNEXTLINE(google-runtime-int)
auto abs(wrapped<long> wrapped_value) noexcept(false) -> wrapped<long> {
  return {arene::base::abs(wrapped_value.value)};
}

/// @test @c abs_if_needed on a custom type with a @c noexcept(false) ADL @c abs function is also @c noexcept(false)
TEST(AbsIfNeededTest, CustomTypeWithThrowingAbs) {
  ASSERT_EQ(abs_if_needed(wrapped<long>{0}), wrapped<long>{0});
  ASSERT_EQ(abs_if_needed(wrapped<long>{3}), wrapped<long>{3});
  ASSERT_EQ(abs_if_needed(wrapped<long>{-8}), wrapped<long>{8});

  ASSERT_FALSE(noexcept(abs_if_needed(wrapped<long>{0})));
}

}  // namespace

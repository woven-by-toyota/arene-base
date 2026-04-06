// Copyright 2025, Toyota Motor Corporation

#include "arene/base/math/float_sign.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/byte/byte.hpp"
#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/endian/endian.hpp"
#include "arene/base/math/abs.hpp"
#include "arene/base/math/float_properties.hpp"
#include "arene/base/stdlib_choice/climits.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

/// @brief Check if a NaN is signaling or not, following the ISO/IEC 60559 standard
/// @tparam T The type of the NaN to check
/// @tparam SignalingFlagBitIndex The index of the bit containing the signaling flag, counted from the big-endian right
/// @param value The NaN to check
/// @return @c true if @c value is a signaling NaN, @c false if @c value is a quiet NaN
/// @pre @c value must be a NaN or the result will be meaningless
/// @{
template <typename T, std::size_t SignalingFlagBitIndex>
auto nan_is_signaling_impl(T const& value) noexcept -> bool {
  constexpr std::size_t signaling_flag_byte_index{SignalingFlagBitIndex / CHAR_BIT};
  constexpr arene::base::byte signaling_flag_bit_mask{arene::base::to_byte(1U) << (SignalingFlagBitIndex % CHAR_BIT)};

  arene::base::array<arene::base::byte, sizeof(value)> as_bytes{};
  arene::base::write_little_endian<T>(value, as_bytes);

  // ISO/IEC 60559 says that a NaN is signaling iff the most significant bit of its significand is 0.
  return (as_bytes[signaling_flag_byte_index] & signaling_flag_bit_mask) == arene::base::byte{};
}

auto nan_is_signaling(float const& value) noexcept -> bool {
  // For floats, there are 23 digits of encoded precision, so the most significant bit is number 22.
  return nan_is_signaling_impl<float, 22U>(value);
}

auto nan_is_signaling(double const& value) noexcept -> bool {
  // For doubles, there are 52 digits of encoded precision, so the most significant bit is number 51.
  return nan_is_signaling_impl<double, 51U>(value);
}
/// @}

template <typename T>
constexpr auto positive_values = arene::base::to_array(
    {static_cast<T>(0.0),
     static_cast<T>(1.0),
     static_cast<T>(1.1),
     std::numeric_limits<T>::denorm_min(),
     std::numeric_limits<T>::epsilon(),
     std::numeric_limits<T>::min(),
     std::numeric_limits<T>::max(),
     std::numeric_limits<T>::infinity()}
);

template <typename T>
constexpr auto nan_values = arene::base::to_array(
    {std::numeric_limits<T>::quiet_NaN(),
     std::numeric_limits<T>::signaling_NaN(),
     -std::numeric_limits<T>::quiet_NaN(),
     -std::numeric_limits<T>::signaling_NaN()}
);

template <typename Params>
class FloatSignTest : public testing::Test {};

using test_types = ::testing::Types<float, double>;
TYPED_TEST_SUITE(FloatSignTest, test_types, );

/// @test Floating point sign functions are always marked as non-throwing
TYPED_TEST(FloatSignTest, IsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(arene::base::signbit(TypeParam{})));
  STATIC_ASSERT_TRUE(noexcept(arene::base::copysign(TypeParam{}, TypeParam{})));
}

/// @test `signbit` returns `false` for positive numbers (including positive zero)
TYPED_TEST(FloatSignTest, PositiveValuesHaveFalseSignbit) {
  STATIC_ASSERT_FALSE(arene::base::signbit(static_cast<TypeParam>(0.0)));
  STATIC_ASSERT_FALSE(arene::base::signbit(static_cast<TypeParam>(1.0)));
  STATIC_ASSERT_FALSE(arene::base::signbit(static_cast<TypeParam>(1.1)));

  STATIC_ASSERT_FALSE(arene::base::signbit(std::numeric_limits<TypeParam>::denorm_min()));
  STATIC_ASSERT_FALSE(arene::base::signbit(std::numeric_limits<TypeParam>::epsilon()));
  STATIC_ASSERT_FALSE(arene::base::signbit(std::numeric_limits<TypeParam>::min()));
  STATIC_ASSERT_FALSE(arene::base::signbit(std::numeric_limits<TypeParam>::max()));

  STATIC_ASSERT_FALSE(arene::base::signbit(std::numeric_limits<TypeParam>::infinity()));
}

/// @test `signbit` returns `true` for negative numbers (including negative zero)
TYPED_TEST(FloatSignTest, NegativeValuesHaveTrueSignbit) {
  STATIC_ASSERT_TRUE(arene::base::signbit(static_cast<TypeParam>(-0.0)));
  STATIC_ASSERT_TRUE(arene::base::signbit(static_cast<TypeParam>(-1.0)));
  STATIC_ASSERT_TRUE(arene::base::signbit(static_cast<TypeParam>(-1.1)));

  STATIC_ASSERT_TRUE(arene::base::signbit(std::numeric_limits<TypeParam>::lowest()));
  STATIC_ASSERT_TRUE(arene::base::signbit(-std::numeric_limits<TypeParam>::denorm_min()));
  STATIC_ASSERT_TRUE(arene::base::signbit(-std::numeric_limits<TypeParam>::epsilon()));
  STATIC_ASSERT_TRUE(arene::base::signbit(-std::numeric_limits<TypeParam>::min()));
  STATIC_ASSERT_TRUE(arene::base::signbit(-std::numeric_limits<TypeParam>::max()));

  STATIC_ASSERT_TRUE(arene::base::signbit(-std::numeric_limits<TypeParam>::infinity()));
}

/// @test `signbit` has consistent behaviour for NaN values, whatever their base sign is
TYPED_TEST(FloatSignTest, NanValuesHaveConsistentSignbit) {
  // C++ and ISO/IEC 60559 do not require this to have a positive sign bit (though it generally does in practice).
  constexpr bool base_sign{arene::base::signbit(std::numeric_limits<TypeParam>::quiet_NaN())};

  // Negating a NaN flips its sign bit even though both signs behave the same in arithmetic.
  STATIC_ASSERT_NE(base_sign, arene::base::signbit(-std::numeric_limits<TypeParam>::quiet_NaN()));

  // This is also not actually required to be the same as quiet_NaN() but it would be very odd if it weren't.
  STATIC_ASSERT_EQ(base_sign, arene::base::signbit(std::numeric_limits<TypeParam>::signaling_NaN()));
  STATIC_ASSERT_NE(base_sign, arene::base::signbit(-std::numeric_limits<TypeParam>::signaling_NaN()));
}

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "We're specifically testing floating point operations");

/// @test `copysign` between two non-NaN values returns a value with the magnitude of the left operand and the sign of
/// the right operand
CONSTEXPR_TYPED_TEST(FloatSignTest, CopySignForNonNanValues) {
  for (TypeParam const& left : positive_values<TypeParam>) {
    for (TypeParam const& right : positive_values<TypeParam>) {
      CONSTEXPR_ASSERT_EQ(arene::base::copysign(left, right), left);
      CONSTEXPR_ASSERT_EQ(arene::base::copysign(-left, right), left);

      CONSTEXPR_ASSERT_EQ(arene::base::copysign(left, -right), -left);
      CONSTEXPR_ASSERT_EQ(arene::base::copysign(-left, -right), -left);
    }
  }
}

/// @test `copysign` with a NaN value on the right returns a value with the magnitude of the left operand and
/// unspecified sign
CONSTEXPR_TYPED_TEST(FloatSignTest, CopySignForRightNan) {
  for (TypeParam const& left : positive_values<TypeParam>) {
    for (TypeParam const& right : nan_values<TypeParam>) {
      CONSTEXPR_ASSERT_EQ(arene::base::abs(arene::base::copysign(left, right)), left);
      CONSTEXPR_ASSERT_EQ(arene::base::abs(arene::base::copysign(-left, right)), left);
    }
  }
}

ARENE_IGNORE_END();

/// @test `copysign` with a NaN value on the left returns the same kind of NaN with the sign bit changed
CONSTEXPR_TYPED_TEST(FloatSignTest, CopySignForLeftNan) {
  for (TypeParam const& left : nan_values<TypeParam>) {
    for (TypeParam const& right : positive_values<TypeParam>) {
      // The right operand is positive so the result should be a positive NaN regardless of the sign of quiet_NaN()
      CONSTEXPR_ASSERT_FALSE(arene::base::signbit(arene::base::copysign(left, right)));
      CONSTEXPR_ASSERT(arene::base::isnan(arene::base::copysign(left, right)));

      // The right operand is negative so the result should be a negative NaN regardless of the sign of quiet_NaN()
      CONSTEXPR_ASSERT(arene::base::signbit(arene::base::copysign(left, -right)));
      CONSTEXPR_ASSERT(arene::base::isnan(arene::base::copysign(left, -right)));
    }
  }
}

/// @test `copysign` with two NaN values returns the same kind of NaN as the left operand, with unspecified sign
TYPED_TEST(FloatSignTest, CopySignForBothNan) {
  // This test can not be CONSTEXPR_ because we need to inspect a value's bit pattern, which we can only do at runtime.

  // Check to make sure the nan_is_signaling helper function is correct for this type.
  ASSERT_FALSE(nan_is_signaling(std::numeric_limits<TypeParam>::quiet_NaN()));
  ASSERT_TRUE(nan_is_signaling(std::numeric_limits<TypeParam>::signaling_NaN()));

  for (TypeParam const& left : nan_values<TypeParam>) {
    for (TypeParam const& right : nan_values<TypeParam>) {
      EXPECT_TRUE(arene::base::isnan(arene::base::copysign(left, right)));
      EXPECT_EQ(nan_is_signaling(arene::base::copysign(left, right)), nan_is_signaling(left));
    }
  }
}

}  // namespace

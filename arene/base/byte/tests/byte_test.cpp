// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/byte/byte.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/climits.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/is_copy_assignable.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/stdlib_choice/is_destructible.hpp"
#include "arene/base/stdlib_choice/is_move_assignable.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/is_trivially_copyable.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/remove_duplicates.hpp"  // IWYU pragma: keep

namespace {

using ::arene::base::byte;
using ::arene::base::to_byte;
using ::arene::base::to_integer;
using ::arene::base::literals::operator""_byte;

/// @test `char` is an 8 bits wide type.
TEST(Byte, CharBitIs8) { STATIC_ASSERT_EQ(CHAR_BIT, 8); }

/// @test `byte` is a trivial class.
TEST(Byte, IsTrivial) {
  STATIC_ASSERT_TRUE(std::is_trivially_default_constructible<byte>::value);
  STATIC_ASSERT_TRUE(std::is_trivially_destructible<byte>::value);
  STATIC_ASSERT_TRUE(std::is_trivially_copy_constructible<byte>::value);
  STATIC_ASSERT_TRUE(std::is_trivially_copy_assignable<byte>::value);
  STATIC_ASSERT_TRUE(std::is_trivially_move_constructible<byte>::value);
  STATIC_ASSERT_TRUE(std::is_trivially_move_assignable<byte>::value);
  STATIC_ASSERT_TRUE(std::is_trivially_copyable<byte>::value);
}

/// @test `byte` is a 1 byte wide type.
TEST(Byte, SizeOfIs1) { STATIC_ASSERT_EQ(sizeof(byte), 1); }

using integer_conversion_types = arene::base::type_lists::remove_duplicates_t<::testing::Types<
    unsigned char,
    char,
    std::uint8_t,
    std::int8_t,
    std::uint16_t,
    std::int16_t,
    std::uint32_t,
    std::int32_t,
    std::uint64_t,
    std::int64_t>>;

template <typename T>
class ToIntegerTest : public ::testing::Test {};

TYPED_TEST_SUITE(ToIntegerTest, integer_conversion_types, );

/// @test `to_integer` returns the same value that a cast to the requested integer type would.
TYPED_TEST(ToIntegerTest, ReturnsValueEquivalentToCastingInputValueToOutputType) {
  STATIC_ASSERT_EQ(
      to_integer<TypeParam>(static_cast<byte>(0b0100'0000)),
      static_cast<TypeParam>(static_cast<byte>(0b0100'0000))
  );
  STATIC_ASSERT_EQ(
      to_integer<TypeParam>(static_cast<byte>(0b1100'0000)),
      static_cast<TypeParam>(static_cast<byte>(0b1100'0000))
  );
}

template <typename T>
class ToByteTest : public ::testing::Test {};

TYPED_TEST_SUITE(ToByteTest, integer_conversion_types, );

/// @test `to_byte` returns the same value that a cast to the `byte` type would.
TYPED_TEST(ToByteTest, ReturnsValueEquivalentToCastingInputValueToByte) {
  STATIC_ASSERT_EQ(
      to_byte(std::numeric_limits<TypeParam>::lowest()),
      static_cast<byte>(static_cast<std::uint8_t>(std::numeric_limits<TypeParam>::lowest()))
  );
  STATIC_ASSERT_EQ(
      to_byte(std::numeric_limits<TypeParam>::max()),
      static_cast<byte>(static_cast<std::uint8_t>(std::numeric_limits<TypeParam>::max()))
  );
}

/// @test The `byte` literal produces the same value that a call to `to_byte` would.
TEST(ByteLiteral, IsEquivalentToByteCreatedWithToByteFromLiteral) {
  STATIC_ASSERT_EQ(0_byte, to_byte(0));
  STATIC_ASSERT_EQ(10_byte, to_byte(10));
  STATIC_ASSERT_EQ(255_byte, to_byte(255));

  // if literal is larger than byte, aliases the same way as to_byte
  STATIC_ASSERT_EQ(256_byte, to_byte(256));
}

/// @test The `byte` literal does not throw exceptions.
TEST(ByteLiteral, IsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(0_byte));
  STATIC_ASSERT_TRUE(noexcept(10_byte));
  STATIC_ASSERT_TRUE(noexcept(255_byte));
}

/// @test The `byte` OR operator behaves like the standard bitwise OR operator.
TEST(Byte, BitOrIsStandardBinaryOr) {
  STATIC_ASSERT_EQ(0b0_byte | 0b0_byte, 0b0_byte);
  STATIC_ASSERT_EQ(0b000_byte | 0b101_byte, 0b101_byte);
  STATIC_ASSERT_EQ(0b010_byte | 0b110_byte, 0b110_byte);
  STATIC_ASSERT_EQ(0xF0_byte | 0x1F_byte, 0xFF_byte);
}

/// @test The `byte` OR assignment operator behaves like the standard bitwise OR assignment operator.
TEST(Byte, BitOrAssignmentIsBitOrThenAssign) {
  auto value = 0b0000'0000_byte;
  value |= 0b0000'0110_byte;
  ASSERT_EQ(value, 0b0000'0110_byte);
  value |= 0b0000'0010_byte;
  ASSERT_EQ(value, 0b0000'0110_byte);
  value |= 0b1111'0000_byte;
  ASSERT_EQ(value, 0b1111'0110_byte);
  value |= 0b0000'0000_byte;
  ASSERT_EQ(value, 0b1111'0110_byte);
  value |= 0b1111'1111_byte;
  ASSERT_EQ(value, 0b1111'1111_byte);
}

/// @test The `byte` AND operator behaves like the standard bitwise AND operator.
TEST(Byte, BitAndIsStandardBinaryAnd) {
  STATIC_ASSERT_EQ(0b0_byte & 0b0_byte, 0b0_byte);
  STATIC_ASSERT_EQ(0b000_byte & 0b101_byte, 0b000_byte);
  STATIC_ASSERT_EQ(0b010_byte & 0b110_byte, 0b010_byte);
  STATIC_ASSERT_EQ(0xF0_byte & 0x1F_byte, 0x10_byte);
}

/// @test The `byte` AND assignment operator behaves like the standard bitwise AND assignment operator.
TEST(Byte, BitAndAssignmentIsBitAndThenAssignment) {
  auto value = 0b1111'1111_byte;
  value &= 0b0000'1111_byte;
  ASSERT_EQ(value, 0b0000'1111_byte);
  value &= 0b0000'0110_byte;
  ASSERT_EQ(value, 0b0000'0110_byte);
  value &= 0b1111'0000_byte;
  ASSERT_EQ(value, 0b0000'0000_byte);
  value &= 0b0000'0000_byte;
  ASSERT_EQ(value, 0b0000'0000_byte);
  value &= 0b1111'1111_byte;
  ASSERT_EQ(value, 0b0000'0000_byte);
}

/// @test The `byte` XOR operator behaves like the standard bitwise XOR operator.
TEST(Byte, BitXOrIsStandardBinaryXOr) {
  STATIC_ASSERT_EQ(0b0_byte ^ 0b0_byte, 0b0_byte);
  STATIC_ASSERT_EQ(0b000_byte ^ 0b101_byte, 0b101_byte);
  STATIC_ASSERT_EQ(0b010_byte ^ 0b110_byte, 0b100_byte);
  STATIC_ASSERT_EQ(0b1111'1111_byte ^ 0b0001'1111_byte, 0b1110'0000_byte);
}

/// @test The `byte` XOR assignment operator behaves like the standard bitwise XOR assignment operator.
TEST(Byte, BitXorAssignmentIsBitXorThenAssignment) {
  auto value = 0b0000'0000_byte;
  value ^= 0b0000'0110_byte;
  ASSERT_EQ(value, 0b0000'0110_byte);
  value ^= 0b0000'0010_byte;
  ASSERT_EQ(value, 0b0000'0100_byte);
  value ^= 0b1111'0000_byte;
  ASSERT_EQ(value, 0b1111'0100_byte);
  value ^= 0b0000'0000_byte;
  ASSERT_EQ(value, 0b1111'0100_byte);
  value ^= 0b1111'1111_byte;
  ASSERT_EQ(value, 0b0000'1011_byte);
}

/// @test The `byte` NOT operator behaves like the standard bitwise NOT operator.
TEST(Byte, NegationIsBinaryNegation) {
  STATIC_ASSERT_EQ(~0_byte, 0xFF_byte);
  STATIC_ASSERT_EQ(~128_byte, 127_byte);
  STATIC_ASSERT_EQ(~0b1010'1110_byte, 0b0101'0001_byte);
}

/// @test The `byte` left shift operator behaves like the standard bitwise left shift operator.
TEST(Byte, LeftShiftIsBinaryLeftShift) {
  STATIC_ASSERT_EQ(0_byte << 2U, 0_byte);
  STATIC_ASSERT_EQ(0_byte << 0U, 0_byte);

  STATIC_ASSERT_EQ(1_byte << 0U, 1_byte);
  STATIC_ASSERT_EQ(1_byte << 1U, 2_byte);
  STATIC_ASSERT_EQ(1_byte << 2U, 4_byte);
  STATIC_ASSERT_EQ(1_byte << 7U, 128_byte);

  STATIC_ASSERT_EQ(0b1111'1111_byte << 0U, 0b1111'1111_byte);
  STATIC_ASSERT_EQ(0b1111'1111_byte << 1U, 0b1111'1110_byte);
  STATIC_ASSERT_EQ(0b1111'1111_byte << 2U, 0b1111'1100_byte);
  STATIC_ASSERT_EQ(0b1111'1111_byte << 3U, 0b1111'1000_byte);
  STATIC_ASSERT_EQ(0b1111'1111_byte << 4U, 0b1111'0000_byte);
  STATIC_ASSERT_EQ(0b1111'1111_byte << 5U, 0b1110'0000_byte);

  STATIC_ASSERT_EQ(0b0000'1111_byte << 0U, 0b0000'1111_byte);
  STATIC_ASSERT_EQ(0b0000'1111_byte << 1U, 0b0001'1110_byte);
  STATIC_ASSERT_EQ(0b0000'1111_byte << 2U, 0b0011'1100_byte);
  STATIC_ASSERT_EQ(0b0000'1111_byte << 3U, 0b0111'1000_byte);
  STATIC_ASSERT_EQ(0b0000'1111_byte << 4U, 0b1111'0000_byte);
  STATIC_ASSERT_EQ(0b0000'1111_byte << 5U, 0b1110'0000_byte);
}

/// @test The `byte` left shift assignment operator behaves like the standard bitwise left shift assignment operator.
TEST(Byte, LeftShiftAssignment) {
  auto value = 0b0000'1111_byte;
  value <<= 0U;
  ASSERT_EQ(value, 0b0000'1111_byte);
  value <<= 1U;
  ASSERT_EQ(value, 0b0001'1110_byte);
  value <<= 2U;
  ASSERT_EQ(value, 0b0111'1000_byte);
  value <<= 4U;
  ASSERT_EQ(value, 0b1000'0000_byte);
}

/// @test The `byte` right shift operator behaves like the standard bitwise right shift operator.
TEST(Byte, RightShift) {
  STATIC_ASSERT_EQ(0_byte >> 5U, 0_byte);
  STATIC_ASSERT_EQ(67_byte >> 2U, 16_byte);

  // Expect right-shift to fill in high-bits with zeros
  STATIC_ASSERT_EQ(0b1111'0000_byte >> 0U, 0b1111'0000_byte);
  STATIC_ASSERT_EQ(0b1111'0000_byte >> 1U, 0b0111'1000_byte);
  STATIC_ASSERT_EQ(0b1111'0000_byte >> 2U, 0b0011'1100_byte);
  STATIC_ASSERT_EQ(0b1111'0000_byte >> 3U, 0b0001'1110_byte);
  STATIC_ASSERT_EQ(0b1111'0000_byte >> 6U, 0b0000'0011_byte);
  STATIC_ASSERT_EQ(0b1111'0000_byte >> 7U, 0b0000'0001_byte);
}

/// @test The `byte` right shift assignment operator behaves like the standard bitwise right shift assignment operator.
TEST(Byte, RightShiftAssignment) {
  auto value = 0b1111'0000_byte;
  value >>= 0U;
  ASSERT_EQ(value, 0b1111'0000_byte);
  value >>= 2U;
  ASSERT_EQ(value, 0b0011'1100_byte);
  value >>= 4U;
  ASSERT_EQ(value, 0b0000'0011_byte);
}

}  // namespace

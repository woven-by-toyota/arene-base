// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/strings/format/to_inline_string.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/strings/inline_string.hpp"
#include "arene/base/strings/string_view.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

using ::arene::base::inline_string;
using ::arene::base::string_view;
using ::arene::base::to_inline_string;
using ::arene::base::to_string_detail::digits_for_integral_value;

/// @test `digits_for_integral_value` returns the number of characters needed to represent a given integral type.
TEST(DigitsForIntegralValue, ReturnsTheNumberOfDigitsNeededForAnIntegralType) {
  // needs explicit test to get coverage credit as it's only evaluated in a constexpr context through the public API.
  STATIC_ASSERT_EQ(digits_for_integral_value<std::uint8_t>(), 3);
  STATIC_ASSERT_EQ(digits_for_integral_value<std::uint16_t>(), 5);
  STATIC_ASSERT_EQ(digits_for_integral_value<std::uint32_t>(), 10);
  STATIC_ASSERT_EQ(digits_for_integral_value<std::uint64_t>(), 20);
  STATIC_ASSERT_EQ(digits_for_integral_value<std::int8_t>(), 4);
  STATIC_ASSERT_EQ(digits_for_integral_value<std::int16_t>(), 6);
  STATIC_ASSERT_EQ(digits_for_integral_value<std::int32_t>(), 11);
  STATIC_ASSERT_EQ(digits_for_integral_value<std::int64_t>(), 20);
}

/// @test `to_inline_string` returns an inline string of the appropriate length to represent a number of a given
/// unsigned integer type.
TEST(ToInlineString, TheReturnTypeForUnsignedIntegersIsAnInlineStringOfTheAppropriateLength) {
  ::testing::StaticAssertTypeEq<decltype(to_inline_string(std::declval<std::uint8_t>())), inline_string<3>>();
  ::testing::StaticAssertTypeEq<decltype(to_inline_string(std::declval<std::uint16_t>())), inline_string<5>>();
  ::testing::StaticAssertTypeEq<decltype(to_inline_string(std::declval<std::uint32_t>())), inline_string<10>>();
  ::testing::StaticAssertTypeEq<decltype(to_inline_string(std::declval<std::uint64_t>())), inline_string<20>>();
}

/// @test `to_inline_string` returns an inline string of the appropriate length to represent a number of a given signed
/// integer type.
TEST(ToInlineString, TheReturnTypeForSignedIntegersIsAnInlineStringOfTheAppropriateLength) {
  ::testing::StaticAssertTypeEq<decltype(to_inline_string(std::declval<std::int8_t>())), inline_string<4>>();
  ::testing::StaticAssertTypeEq<decltype(to_inline_string(std::declval<std::int16_t>())), inline_string<6>>();
  ::testing::StaticAssertTypeEq<decltype(to_inline_string(std::declval<std::int32_t>())), inline_string<11>>();
  ::testing::StaticAssertTypeEq<decltype(to_inline_string(std::declval<std::int64_t>())), inline_string<20>>();
}

/// @test `to_inline_string` properly converts 0 for all integer types.
TEST(ToInlineString, ZeroIsZero) {
  STATIC_ASSERT_EQ(to_inline_string(static_cast<std::uint8_t>(0)), "0");
  STATIC_ASSERT_EQ(to_inline_string(static_cast<std::uint16_t>(0)), "0");
  STATIC_ASSERT_EQ(to_inline_string(static_cast<std::uint32_t>(0)), "0");
  STATIC_ASSERT_EQ(to_inline_string(static_cast<std::uint64_t>(0)), "0");
  STATIC_ASSERT_EQ(to_inline_string(static_cast<std::int8_t>(0)), "0");
  STATIC_ASSERT_EQ(to_inline_string(static_cast<std::int16_t>(0)), "0");
  STATIC_ASSERT_EQ(to_inline_string(static_cast<std::int32_t>(0)), "0");
  STATIC_ASSERT_EQ(to_inline_string(static_cast<std::int64_t>(0)), "0");
}

/// @test `to_inline_string` properly converts single-digit numbers for all integer types.
TEST(ToInlineString, SingleDigitsConvertOK) {
  STATIC_ASSERT_EQ(to_inline_string(static_cast<std::uint8_t>(1)), "1");
  STATIC_ASSERT_EQ(to_inline_string(static_cast<std::uint16_t>(2)), "2");
  STATIC_ASSERT_EQ(to_inline_string(static_cast<std::uint32_t>(3)), "3");
  STATIC_ASSERT_EQ(to_inline_string(static_cast<std::uint64_t>(4)), "4");
  STATIC_ASSERT_EQ(to_inline_string(static_cast<std::int8_t>(5)), "5");
  STATIC_ASSERT_EQ(to_inline_string(static_cast<std::int16_t>(6)), "6");
  STATIC_ASSERT_EQ(to_inline_string(static_cast<std::int32_t>(7)), "7");
  STATIC_ASSERT_EQ(to_inline_string(static_cast<std::int64_t>(8)), "8");
}

/// @test `to_inline_string` properly converts double-digit numbers for all integer types.
TEST(ToInlineString, DoubleDigitsConvertOK) {
  STATIC_ASSERT_EQ(to_inline_string(static_cast<std::uint8_t>(10)), "10");
  STATIC_ASSERT_EQ(to_inline_string(static_cast<std::uint16_t>(29)), "29");
  STATIC_ASSERT_EQ(to_inline_string(static_cast<std::uint32_t>(35)), "35");
  STATIC_ASSERT_EQ(to_inline_string(static_cast<std::uint64_t>(47)), "47");
  STATIC_ASSERT_EQ(to_inline_string(static_cast<std::int8_t>(53)), "53");
  STATIC_ASSERT_EQ(to_inline_string(static_cast<std::int16_t>(68)), "68");
  STATIC_ASSERT_EQ(to_inline_string(static_cast<std::int32_t>(71)), "71");
  STATIC_ASSERT_EQ(to_inline_string(static_cast<std::int64_t>(82)), "82");
}

/// @test `to_inline_string` properly converts multiple-digit numbers for all integer types.
TEST(ToInlineString, MultipleDigitsConvertOK) {
  STATIC_ASSERT_EQ(to_inline_string(static_cast<std::uint8_t>(201)), "201");
  STATIC_ASSERT_EQ(to_inline_string(static_cast<std::uint16_t>(42923)), "42923");
  STATIC_ASSERT_EQ(to_inline_string(static_cast<std::uint32_t>(351267137)), "351267137");
  STATIC_ASSERT_EQ(to_inline_string(static_cast<std::uint64_t>(238746298462984618)), "238746298462984618");
  STATIC_ASSERT_EQ(to_inline_string(static_cast<std::int8_t>(101)), "101");
  STATIC_ASSERT_EQ(to_inline_string(static_cast<std::int16_t>(24321)), "24321");
  STATIC_ASSERT_EQ(to_inline_string(static_cast<std::int32_t>(38712494)), "38712494");
  STATIC_ASSERT_EQ(to_inline_string(static_cast<std::int64_t>(238472472473239)), "238472472473239");
}

/// @test `to_inline_string` properly converts negative numbers for all signed integer types.
TEST(ToInlineString, NegativeNumbersConvertOK) {
  STATIC_ASSERT_EQ(to_inline_string(static_cast<std::int8_t>(-123)), "-123");
  STATIC_ASSERT_EQ(to_inline_string(static_cast<std::int16_t>(-4567)), "-4567");
  STATIC_ASSERT_EQ(to_inline_string(static_cast<std::int32_t>(-123456)), "-123456");
  STATIC_ASSERT_EQ(to_inline_string(static_cast<std::int64_t>(-9876543210)), "-9876543210");
}

/// @test `to_inline_string` properly converts maximum values for all integer types.
TEST(ToInlineString, MaxValuesConvertOK) {
  STATIC_ASSERT_EQ(to_inline_string(std::numeric_limits<std::uint8_t>::max()), "255");
  STATIC_ASSERT_EQ(to_inline_string(std::numeric_limits<std::uint16_t>::max()), "65535");
  STATIC_ASSERT_EQ(to_inline_string(std::numeric_limits<std::uint32_t>::max()), "4294967295");
  STATIC_ASSERT_EQ(to_inline_string(std::numeric_limits<std::uint64_t>::max()), "18446744073709551615");
  STATIC_ASSERT_EQ(to_inline_string(std::numeric_limits<std::int8_t>::max()), "127");
  STATIC_ASSERT_EQ(to_inline_string(std::numeric_limits<std::int16_t>::max()), "32767");
  STATIC_ASSERT_EQ(to_inline_string(std::numeric_limits<std::int32_t>::max()), "2147483647");
  STATIC_ASSERT_EQ(to_inline_string(std::numeric_limits<std::int64_t>::max()), "9223372036854775807");
}

/// @test `to_inline_string` properly converts minimum values for all signed integer types.
TEST(ToInlineString, MinValuesConvertOK) {
  STATIC_ASSERT_EQ(to_inline_string(std::numeric_limits<std::int8_t>::min()), "-128");
  STATIC_ASSERT_EQ(to_inline_string(std::numeric_limits<std::int16_t>::min()), "-32768");
  STATIC_ASSERT_EQ(to_inline_string(std::numeric_limits<std::int32_t>::min()), "-2147483648");
  STATIC_ASSERT_EQ(to_inline_string(std::numeric_limits<std::int64_t>::min()), "-9223372036854775808");
}

/// @test `to_inline_string` properly converts negative maximum values for all signed integer types.
TEST(ToInlineString, NegativeMaxValuesConvertOK) {
  STATIC_ASSERT_EQ(to_inline_string(-std::numeric_limits<std::int8_t>::max()), "-127");
  STATIC_ASSERT_EQ(to_inline_string(-std::numeric_limits<std::int16_t>::max()), "-32767");
  STATIC_ASSERT_EQ(to_inline_string(-std::numeric_limits<std::int32_t>::max()), "-2147483647");
  STATIC_ASSERT_EQ(to_inline_string(-std::numeric_limits<std::int64_t>::max()), "-9223372036854775807");
}

/// @test `to_inline_string` returns an inline string of the appropriate length to represent booleans,
/// and properly converts boolean expressions.
TEST(ToInlineString, BooleansConvertToStringRepresentation) {
  STATIC_ASSERT_EQ(to_inline_string(true), string_view{"true"});
  STATIC_ASSERT_EQ(to_inline_string(false), string_view{"false"});

  ::testing::StaticAssertTypeEq<decltype(to_inline_string(true)), inline_string<5>>();
}

}  // namespace

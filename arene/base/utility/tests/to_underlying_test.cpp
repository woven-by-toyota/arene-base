// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/utility/to_underlying.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

constexpr std::size_t size_t_value = 45353;
// NOLINTNEXTLINE(google-runtime-int)
constexpr std::int16_t int16_value = 947;
// NOLINTNEXTLINE(google-runtime-int)
constexpr unsigned char uchar_one = 1;
// NOLINTNEXTLINE(google-runtime-int)
constexpr unsigned char uchar_two = 2;
// NOLINTNEXTLINE(google-runtime-int)
constexpr unsigned char uchar_forty = 40;

// NOLINTNEXTLINE(google-runtime-int)
enum class uchar_enum : unsigned char { value, value_plus_one, value_plus_2, forty = uchar_forty };
// NOLINTNEXTLINE(google-runtime-int)
enum class int16_enum : std::int16_t { value = int16_value };
enum class size_t_enum : std::size_t { value = size_t_value };

/// @test Given an input of enumeration type @c Enum , @c arene::base::to_underlying(enum) 's return type is equal to
/// the type underlying @c Enum .
TEST(ToUnderlyingTest, UnderlyingTypeEquivalent) {
  ::testing::StaticAssertTypeEq<decltype(arene::base::to_underlying(uchar_enum::value)), unsigned char>();
  ::testing::StaticAssertTypeEq<decltype(arene::base::to_underlying(int16_enum::value)), std::int16_t>();
  ::testing::StaticAssertTypeEq<decltype(arene::base::to_underlying(size_t_enum::value)), std::size_t>();
}
/// @test Given an input of enumeration type @c Enum , @c arene::base::to_underlying(enum) 's return value is equal to
/// the value of @c enum.
TEST(ToUnderlyingTest, UnderlyingValueEquivalent) {
  STATIC_ASSERT_EQ(arene::base::to_underlying(uchar_enum::value), 0);
  STATIC_ASSERT_EQ(arene::base::to_underlying(uchar_enum::value_plus_one), uchar_one);
  STATIC_ASSERT_EQ(arene::base::to_underlying(uchar_enum::value_plus_2), uchar_two);
  STATIC_ASSERT_EQ(arene::base::to_underlying(uchar_enum::forty), uchar_forty);
  STATIC_ASSERT_EQ(arene::base::to_underlying(int16_enum::value), int16_value);
  STATIC_ASSERT_EQ(arene::base::to_underlying(size_t_enum::value), size_t_value);
}

}  // namespace

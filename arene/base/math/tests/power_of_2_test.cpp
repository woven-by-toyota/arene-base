// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/math/power_of_2.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/climits.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {
/// @test `is_power_of_2` returns the expected values with signed integer types.
TEST(IsPowerOf2, SignedIntegers) {
  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(std::int8_t(0)));
  STATIC_ASSERT_TRUE(arene::base::is_power_of_2(std::int8_t(1)));
  STATIC_ASSERT_TRUE(arene::base::is_power_of_2(std::int8_t(2)));
  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(std::int8_t(3)));
  STATIC_ASSERT_TRUE(arene::base::is_power_of_2(std::int8_t(4)));
  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(std::int8_t(-1)));
  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(std::int8_t(-2)));
  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(std::int8_t(-3)));
  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(std::int8_t(-4)));
  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(std::int8_t(-5)));
  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(std::int8_t(-128)));

  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(0));
  STATIC_ASSERT_TRUE(arene::base::is_power_of_2(1));
  STATIC_ASSERT_TRUE(arene::base::is_power_of_2(2));
  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(3));
  STATIC_ASSERT_TRUE(arene::base::is_power_of_2(4));
  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(1073741823));
  STATIC_ASSERT_TRUE(arene::base::is_power_of_2(1073741824));
  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(-1));
  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(-2));
  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(-3));
  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(-4));
  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(-5));
  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(-6));
  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(-8));
  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(-2147483648));

  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(0LL));
  STATIC_ASSERT_TRUE(arene::base::is_power_of_2(1LL));
  STATIC_ASSERT_TRUE(arene::base::is_power_of_2(2LL));
  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(3LL));
  STATIC_ASSERT_TRUE(arene::base::is_power_of_2(4LL));
  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(1073741823LL));
  STATIC_ASSERT_TRUE(arene::base::is_power_of_2(1073741824LL));
  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(2147483647LL));
  STATIC_ASSERT_TRUE(arene::base::is_power_of_2(2147483648LL));
  STATIC_ASSERT_TRUE(arene::base::is_power_of_2(std::int64_t(1ULL << 62U)));
  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(std::int64_t(-1)));
  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(std::int64_t(-2)));
  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(std::int64_t(-128)));
}

// NOLINTNEXTLINE(google-runtime-int)
auto make_power_of_2(unsigned long value) -> unsigned long {
  value += 42;
  while (value > 3) {
    value >>= 1U;
  }
  value &= 0xFEU;
  return value * value;
}

/// @test `is_power_of_2` returns the expected values with unsigned integer types.
TEST(IsPowerOf2, UnsignedIntegers) {
  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(std::uint8_t(0)));
  STATIC_ASSERT_TRUE(arene::base::is_power_of_2(std::uint8_t(1)));
  STATIC_ASSERT_TRUE(arene::base::is_power_of_2(std::uint8_t(2)));
  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(std::uint8_t(3)));
  STATIC_ASSERT_TRUE(arene::base::is_power_of_2(std::uint8_t(4)));
  STATIC_ASSERT_TRUE(arene::base::is_power_of_2(std::uint8_t(128)));
  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(std::uint8_t(255)));

  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(0U));
  STATIC_ASSERT_TRUE(arene::base::is_power_of_2(1U));
  STATIC_ASSERT_TRUE(arene::base::is_power_of_2(2U));
  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(3U));
  STATIC_ASSERT_TRUE(arene::base::is_power_of_2(4U));
  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(1073741823U));
  STATIC_ASSERT_TRUE(arene::base::is_power_of_2(1073741824U));
  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(2147483647U));
  STATIC_ASSERT_TRUE(arene::base::is_power_of_2(2147483648U));

  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(0UL));
  STATIC_ASSERT_TRUE(arene::base::is_power_of_2(1UL));
  STATIC_ASSERT_TRUE(arene::base::is_power_of_2(2UL));
  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(3UL));
  STATIC_ASSERT_TRUE(arene::base::is_power_of_2(4UL));
  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(1073741823UL));
  STATIC_ASSERT_TRUE(arene::base::is_power_of_2(1073741824UL));
  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(2147483647UL));
  STATIC_ASSERT_TRUE(arene::base::is_power_of_2(2147483648UL));

  // NOLINTNEXTLINE(google-runtime-int)
  unsigned long value{12345UL};
  ASSERT_FALSE(arene::base::is_power_of_2(value));
  value = make_power_of_2(value);
  ASSERT_TRUE(arene::base::is_power_of_2(value));

  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(0ULL));
  STATIC_ASSERT_TRUE(arene::base::is_power_of_2(1ULL));
  STATIC_ASSERT_TRUE(arene::base::is_power_of_2(2ULL));
  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(3ULL));
  STATIC_ASSERT_TRUE(arene::base::is_power_of_2(4ULL));
  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(1073741823ULL));
  STATIC_ASSERT_TRUE(arene::base::is_power_of_2(1073741824ULL));
  STATIC_ASSERT_FALSE(arene::base::is_power_of_2(2147483647ULL));
  STATIC_ASSERT_TRUE(arene::base::is_power_of_2(2147483648ULL));
  STATIC_ASSERT_TRUE(arene::base::is_power_of_2(std::uint64_t(1U) << 63U));
}

/// @test `power_of_2` returns the expected value, simple case.
TEST(PowerOf2, BasicPowerOf2) {
  for (unsigned i = 0; i < (sizeof(std::size_t) * CHAR_BIT); ++i) {
    ASSERT_EQ(arene::base::power_of_2(i), static_cast<std::size_t>(1) << i);
  }
}

/// @test `power_of_2` returns the expected value, with `constexpr`.
TEST(PowerOf2, Constexpr) {
  STATIC_ASSERT_EQ(arene::base::power_of_2(0), 1);
  STATIC_ASSERT_EQ(
      arene::base::power_of_2(sizeof(std::size_t) * CHAR_BIT - 1),
      (static_cast<std::size_t>(1) << static_cast<unsigned>(sizeof(std::size_t) * CHAR_BIT - 1))
  );
  STATIC_ASSERT_EQ(arene::base::power_of_2(63), 9223372036854775808U);
}

/// @test `power_of_2` returns the expected value, with simple sequence.
TEST(PowerOf2, SimpleSequence) {
  constexpr auto sequence = arene::base::make_power_of_2_sequence<0, 0>();
  ::testing::StaticAssertTypeEq<decltype(sequence), std::index_sequence<> const>();

  constexpr auto sequence2 = arene::base::make_power_of_2_sequence<0, 2>();
  ::testing::StaticAssertTypeEq<decltype(sequence2), std::index_sequence<1, 2> const>();

  constexpr auto sequence3 = arene::base::make_power_of_2_sequence<0, 4>();
  ::testing::StaticAssertTypeEq<decltype(sequence3), std::index_sequence<1, 2, 4, 8> const>();
}

/// @test `power_of_2` returns the expected value, with non-zero minimum sequence.
TEST(PowerOf2, SequenceWithMin) {
  constexpr auto sequence = arene::base::make_power_of_2_sequence<10, 11>();
  ::testing::StaticAssertTypeEq<decltype(sequence), std::index_sequence<1024> const>();

  constexpr auto sequence2 = arene::base::make_power_of_2_sequence<10, 15>();
  ::testing::StaticAssertTypeEq<decltype(sequence2), std::index_sequence<1024, 2048, 4096, 8192, 16384> const>();
  auto sequence4 = arene::base::make_power_of_2_sequence<6, 9>();
  STATIC_ASSERT_NE(&sequence4, nullptr);
  sequence4 = arene::base::make_power_of_2_sequence<6, 9>();
  ::testing::StaticAssertTypeEq<decltype(sequence4), std::index_sequence<64, 128, 256>>();
}
}  // namespace

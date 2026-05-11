// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/endian/byte_swap.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

using ::arene::base::byte_swap;

/// @test `byte_swap` returns the expected value with `uint8_t` type.
TEST(ByteSwap, uint8) {
  STATIC_ASSERT_EQ(byte_swap(static_cast<std::uint8_t>(0)), static_cast<std::uint8_t>(0));
  STATIC_ASSERT_EQ(byte_swap(static_cast<std::uint8_t>(5)), static_cast<std::uint8_t>(5));
  STATIC_ASSERT_EQ(byte_swap(static_cast<std::uint8_t>(99)), static_cast<std::uint8_t>(99));
  STATIC_ASSERT_EQ(byte_swap(static_cast<std::uint8_t>(130)), static_cast<std::uint8_t>(130));
  STATIC_ASSERT_EQ(byte_swap(static_cast<std::uint8_t>(255)), static_cast<std::uint8_t>(255));
}

/// @test `byte_swap` returns the expected value with `uint16_t` type.
TEST(ByteSwap, uint16) {
  STATIC_ASSERT_EQ(byte_swap(static_cast<std::uint16_t>(0x0102)), static_cast<std::uint16_t>(0x0201));
  STATIC_ASSERT_EQ(byte_swap(static_cast<std::uint16_t>(0x5678)), static_cast<std::uint16_t>(0x7856));
}

/// @test `byte_swap` returns the expected value with `uint32_t` type.
TEST(ByteSwap, uint32) {
  STATIC_ASSERT_EQ(byte_swap(static_cast<std::uint32_t>(0x01020304U)), static_cast<std::uint32_t>(0x04030201U));
  STATIC_ASSERT_EQ(byte_swap(static_cast<std::uint32_t>(0x23456789U)), static_cast<std::uint32_t>(0x89674523U));
}

/// @test `byte_swap` returns the expected value with `uint64_t` type.
TEST(ByteSwap, uint64) {
  STATIC_ASSERT_EQ(
      byte_swap(static_cast<std::uint64_t>(0x0102030405060708ULL)),
      static_cast<std::uint64_t>(0x0807060504030201ULL)
  );
  STATIC_ASSERT_EQ(
      byte_swap(static_cast<std::uint64_t>(0x23456789'12345678ULL)),
      static_cast<std::uint64_t>(0x78563412'89674523ULL)
  );
}

/// @test `byte_swap` returns the expected value with `int8_t` type.
TEST(ByteSwap, int8) {
  STATIC_ASSERT_EQ(byte_swap(static_cast<std::int8_t>(0)), static_cast<std::int8_t>(0));
  STATIC_ASSERT_EQ(byte_swap(static_cast<std::int8_t>(5)), static_cast<std::int8_t>(5));
  STATIC_ASSERT_EQ(byte_swap(static_cast<std::int8_t>(99)), static_cast<std::int8_t>(99));
  STATIC_ASSERT_EQ(byte_swap(static_cast<std::int8_t>(127)), static_cast<std::int8_t>(127));
  STATIC_ASSERT_EQ(byte_swap(static_cast<std::int8_t>(-1)), static_cast<std::int8_t>(-1));
  STATIC_ASSERT_EQ(byte_swap(static_cast<std::int8_t>(-128)), static_cast<std::int8_t>(-128));
}

/// @test `byte_swap` returns the expected value with `int16_t` type.
TEST(ByteSwap, int16) {
  STATIC_ASSERT_EQ(byte_swap(static_cast<std::int16_t>(0)), static_cast<std::int16_t>(0));
  STATIC_ASSERT_EQ(byte_swap(static_cast<std::int16_t>(0x0102)), static_cast<std::int16_t>(0x0201));
  STATIC_ASSERT_EQ(byte_swap(static_cast<std::int16_t>(0x7788)), static_cast<std::int16_t>(0x8877));
}

/// @test `byte_swap` returns the expected value with `int32_t` type.
TEST(ByteSwap, int32) {
  STATIC_ASSERT_EQ(byte_swap(static_cast<std::int32_t>(0)), static_cast<std::int32_t>(0));
  STATIC_ASSERT_EQ(byte_swap(static_cast<std::int32_t>(0x11223344)), static_cast<std::int32_t>(0x44332211));
  STATIC_ASSERT_EQ(byte_swap(static_cast<std::int32_t>(0x99887766)), static_cast<std::int32_t>(0x66778899));
}

/// @test `byte_swap` returns the expected value with `int64_t` type.
TEST(ByteSwap, int64) {
  STATIC_ASSERT_EQ(
      byte_swap(static_cast<std::int64_t>(0x0102030405060708LL)),
      static_cast<std::int64_t>(0x0807060504030201LL)
  );
  STATIC_ASSERT_EQ(
      byte_swap(static_cast<std::int64_t>(0x23456789'12345678LL)),
      static_cast<std::int64_t>(0x78563412'89674523LL)
  );
}

}  // namespace

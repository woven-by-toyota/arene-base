// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/byte/byte.hpp"
#include "arene/base/compiler_support/diagnostics.hpp"
// This no_include can't be removed unless we also removed the private pragma in
// arene/base/endian/endian.hpp
// The mapping of the symbol to arene/base/endian.hpp cannot be overriden.
// IWYU pragma: no_include "arene/base/endian/detail/endian_specified_byte_order_impl.hpp"
#include "arene/base/endian/endian.hpp"
#include "arene/base/span/span.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"

namespace {

using arene::base::byte;
using arene::base::endian;
using arene::base::read_big_endian;
using arene::base::read_little_endian;
using arene::base::span;
using arene::base::write_big_endian;
using arene::base::write_little_endian;

static_assert(endian::big != endian::little, "");

/// @test `read_little_endian` returns the expected value with `uint8_t` type.
TEST(Endian, ReadLittleEndianUint8) {
  arene::base::array<byte, 2> const buffer = {static_cast<byte>(0x47), static_cast<byte>(0xC8)};
  span<byte const, 2> const spn(buffer);
  ASSERT_EQ(read_little_endian<std::uint8_t>(spn.subspan<0, 1>()), 0x47);
  ASSERT_EQ(read_little_endian<std::uint8_t>(spn.subspan<1, 1>()), 0xC8);
}

/// @test `read_little_endian` returns the expected value with `uint16_t` type.
TEST(Endian, ReadLittleEndianUint16) {
  arene::base::array<byte, 4> const buffer =
      {static_cast<byte>(0x47), static_cast<byte>(0xC8), static_cast<byte>(0x11), static_cast<byte>(0x22)};
  span<byte const, 4> const spn(buffer);
  ASSERT_EQ(read_little_endian<std::uint16_t>(spn.subspan<0, 2>()), 0xC847);
  ASSERT_EQ(read_little_endian<std::uint16_t>(spn.subspan<2, 2>()), 0x2211);
}

/// @test `read_little_endian` returns the expected value with `uint32_t` type.
TEST(Endian, ReadLittleEndianUint32) {
  arene::base::array<byte, 6> const buffer = {
      static_cast<byte>(0x47),
      static_cast<byte>(0xC8),
      static_cast<byte>(0x11),
      static_cast<byte>(0x22),
      static_cast<byte>(0x33),
      static_cast<byte>(0x44)
  };
  span<byte const, 6> const spn(buffer);
  ASSERT_EQ(read_little_endian<std::uint32_t>(spn.subspan<0, 4>()), 0x2211C847U);
  ASSERT_EQ(read_little_endian<std::uint32_t>(spn.subspan<2, 4>()), 0x44332211U);
}

/// @test `read_little_endian` returns the expected value with `uint64_t` type.
TEST(Endian, ReadLittleEndianUint64) {
  arene::base::array<byte, 10> const buffer = {
      static_cast<byte>(0x47),
      static_cast<byte>(0xC8),
      static_cast<byte>(0x11),
      static_cast<byte>(0x22),
      static_cast<byte>(0x33),
      static_cast<byte>(0x44),
      static_cast<byte>(0x78),
      static_cast<byte>(0x99),
      static_cast<byte>(0x7F),
      static_cast<byte>(0xE3)
  };
  span<byte const, 10> const spn(buffer);
  ASSERT_EQ(read_little_endian<std::uint64_t>(spn.subspan<0, 8>()), 0x997844332211C847ULL);
  ASSERT_EQ(read_little_endian<std::uint64_t>(spn.subspan<2, 8>()), 0xE37F997844332211ULL);
}

/// @test `read_little_endian` returns the expected value with `int8_t` type.
TEST(Endian, ReadLittleEndianInt8) {
  arene::base::array<byte, 2> const buffer = {static_cast<byte>(0x47), static_cast<byte>(0xC8)};
  span<byte const, 2> const spn(buffer);
  ASSERT_EQ(read_little_endian<std::int8_t>(spn.subspan<0, 1>()), std::int8_t(0x47));
  ASSERT_EQ(read_little_endian<std::int8_t>(spn.subspan<1, 1>()), std::int8_t(0xC8));
}

/// @test `read_little_endian` returns the expected value with `int16_t` type.
TEST(Endian, ReadLittleEndianInt16) {
  arene::base::array<byte, 4> const buffer =
      {static_cast<byte>(0x47), static_cast<byte>(0xC8), static_cast<byte>(0x11), static_cast<byte>(0x22)};
  span<byte const, 4> const spn(buffer);
  ASSERT_EQ(read_little_endian<std::int16_t>(spn.subspan<0, 2>()), std::int16_t(0xC847));
  ASSERT_EQ(read_little_endian<std::int16_t>(spn.subspan<2, 2>()), std::int16_t(0x2211));
}

/// @test `read_little_endian` returns the expected value with `int32_t` type.
TEST(Endian, ReadLittleEndianInt32) {
  arene::base::array<byte, 6> const buffer = {
      static_cast<byte>(0x47),
      static_cast<byte>(0xC8),
      static_cast<byte>(0x11),
      static_cast<byte>(0x22),
      static_cast<byte>(0x33),
      static_cast<byte>(0x44)
  };
  span<byte const, 6> const spn(buffer);
  ASSERT_EQ(read_little_endian<std::int32_t>(spn.subspan<0, 4>()), std::int32_t(0x2211C847));
  ASSERT_EQ(read_little_endian<std::int32_t>(spn.subspan<2, 4>()), std::int32_t(0x44332211));
}

/// @test `read_little_endian` returns the expected value with `int64_t` type.
TEST(Endian, ReadLittleEndianInt64) {
  arene::base::array<byte, 10> const buffer = {
      static_cast<byte>(0x47),
      static_cast<byte>(0xC8),
      static_cast<byte>(0x11),
      static_cast<byte>(0x22),
      static_cast<byte>(0x33),
      static_cast<byte>(0x44),
      static_cast<byte>(0x78),
      static_cast<byte>(0x99),
      static_cast<byte>(0x7F),
      static_cast<byte>(0xE3)
  };
  span<byte const, 10> const spn(buffer);
  ASSERT_EQ(read_little_endian<std::int64_t>(spn.subspan<0, 8>()), static_cast<std::int64_t>(0x997844332211C847LL));
  ASSERT_EQ(read_little_endian<std::int64_t>(spn.subspan<2, 8>()), static_cast<std::int64_t>(0xE37F997844332211LL));
}

/// @test `read_little_endian` returns the expected values with `constexpr` of all integer types.
TEST(Endian, ReadLittleEndianConstexpr) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  static constexpr byte buffer[8] = {
      static_cast<byte>(0x11),
      static_cast<byte>(0x22),
      static_cast<byte>(0x33),
      static_cast<byte>(0x44),
      static_cast<byte>(0x55),
      static_cast<byte>(0x66),
      static_cast<byte>(0x77),
      static_cast<byte>(0x88)
  };
  constexpr span<byte const, 8> spn(buffer);

  constexpr auto res_uint8 = read_little_endian<std::uint8_t>(spn.subspan<0, 1>());
  ASSERT_EQ(res_uint8, 0x11);

  constexpr auto res_uint16 = read_little_endian<std::uint16_t>(spn.subspan<0, 2>());
  ASSERT_EQ(res_uint16, 0x2211U);

  constexpr auto res_uint32 = read_little_endian<std::uint32_t>(spn.subspan<0, 4>());
  ASSERT_EQ(res_uint32, 0x44332211U);

  constexpr auto res_uint64 = read_little_endian<std::uint64_t>(spn.subspan<0, 8>());
  ASSERT_EQ(res_uint64, 0x8877665544332211U);

  constexpr auto res_int8 = read_little_endian<std::int8_t>(spn.subspan<0, 1>());
  ASSERT_EQ(res_int8, 0x11);

  constexpr auto res_int16 = read_little_endian<std::int16_t>(spn.subspan<0, 2>());
  ASSERT_EQ(res_int16, 0x2211);

  constexpr auto res_int32 = read_little_endian<std::int32_t>(spn.subspan<0, 4>());
  ASSERT_EQ(res_int32, 0x44332211);

  constexpr auto res_int64 = read_little_endian<std::int64_t>(spn.subspan<0, 8>());
  ASSERT_EQ(res_int64, static_cast<std::int64_t>(0x8877665544332211));
}

/// @test `read_big_endian` returns the expected value with `uint8_t` type.
TEST(Endian, ReadBigEndianUint8) {
  arene::base::array<byte, 2> const buffer = {static_cast<byte>(0x47), static_cast<byte>(0xC8)};
  span<byte const, 2> const spn(buffer);
  ASSERT_EQ(read_big_endian<std::uint8_t>(spn.subspan<0, 1>()), 0x47);
  ASSERT_EQ(read_big_endian<std::uint8_t>(spn.subspan<1, 1>()), 0xC8);
}

/// @test `read_big_endian` returns the expected value with `uint16_t` type.
TEST(Endian, ReadBigEndianUint16) {
  arene::base::array<byte, 4> const buffer =
      {static_cast<byte>(0x47), static_cast<byte>(0xC8), static_cast<byte>(0x11), static_cast<byte>(0x22)};
  span<byte const, 4> const spn(buffer);
  ASSERT_EQ(read_big_endian<std::uint16_t>(spn.subspan<0, 2>()), 0x47C8);
  ASSERT_EQ(read_big_endian<std::uint16_t>(spn.subspan<2, 2>()), 0x1122);
}

/// @test `read_big_endian` returns the expected value with `uint32_t` type.
TEST(Endian, ReadBigEndianUint32) {
  arene::base::array<byte, 6> const buffer = {
      static_cast<byte>(0x47),
      static_cast<byte>(0xC8),
      static_cast<byte>(0x11),
      static_cast<byte>(0x22),
      static_cast<byte>(0x33),
      static_cast<byte>(0x44)
  };
  span<byte const, 6> const spn(buffer);
  ASSERT_EQ(read_big_endian<std::uint32_t>(spn.subspan<0, 4>()), 0x47C81122U);
  ASSERT_EQ(read_big_endian<std::uint32_t>(spn.subspan<2, 4>()), 0x11223344U);
}

/// @test `read_big_endian` returns the expected value with `uint64_t` type.
TEST(Endian, ReadBigEndianUint64) {
  arene::base::array<byte, 10> const buffer = {
      static_cast<byte>(0x47),
      static_cast<byte>(0xC8),
      static_cast<byte>(0x11),
      static_cast<byte>(0x22),
      static_cast<byte>(0x33),
      static_cast<byte>(0x44),
      static_cast<byte>(0x78),
      static_cast<byte>(0x99),
      static_cast<byte>(0x7F),
      static_cast<byte>(0xE3)
  };
  span<byte const, 10> const spn(buffer);
  ASSERT_EQ(read_big_endian<std::uint64_t>(spn.subspan<0, 8>()), 0x47C8112233447899ULL);
  ASSERT_EQ(read_big_endian<std::uint64_t>(spn.subspan<2, 8>()), 0x1122334478997FE3ULL);
}

/// @test `read_big_endian` returns the expected value with `int8_t` type.
TEST(Endian, ReadBigEndianInt8) {
  arene::base::array<byte, 2> const buffer = {static_cast<byte>(0x47), static_cast<byte>(0xC8)};
  span<byte const, 2> const spn(buffer);
  ASSERT_EQ(read_big_endian<std::int8_t>(spn.subspan<0, 1>()), std::int8_t(0x47));
  ASSERT_EQ(read_big_endian<std::int8_t>(spn.subspan<1, 1>()), std::int8_t(0xC8));
}

/// @test `read_big_endian` returns the expected value with `int16_t` type.
TEST(Endian, ReadBigEndianInt16) {
  arene::base::array<byte, 4> const buffer =
      {static_cast<byte>(0x47), static_cast<byte>(0xC8), static_cast<byte>(0x11), static_cast<byte>(0x22)};
  span<byte const, 4> const spn(buffer);
  ASSERT_EQ(read_big_endian<std::int16_t>(spn.subspan<0, 2>()), std::int16_t(0x47C8));
  ASSERT_EQ(read_big_endian<std::int16_t>(spn.subspan<2, 2>()), std::int16_t(0x1122));
}

/// @test `read_big_endian` returns the expected value with `int32_t` type.
TEST(Endian, ReadBigEndianInt32) {
  arene::base::array<byte, 6> const buffer = {
      static_cast<byte>(0x47),
      static_cast<byte>(0xC8),
      static_cast<byte>(0x11),
      static_cast<byte>(0x22),
      static_cast<byte>(0x33),
      static_cast<byte>(0x44)
  };
  span<byte const, 6> const spn(buffer);
  ASSERT_EQ(read_big_endian<std::int32_t>(spn.subspan<0, 4>()), 0x47C81122);
  ASSERT_EQ(read_big_endian<std::int32_t>(spn.subspan<2, 4>()), 0x11223344);
}

/// @test `read_big_endian` returns the expected value with `int64_t` type.
TEST(Endian, ReadBigEndianInt64) {
  arene::base::array<byte, 10> const buffer = {
      static_cast<byte>(0x47),
      static_cast<byte>(0xC8),
      static_cast<byte>(0x11),
      static_cast<byte>(0x22),
      static_cast<byte>(0x33),
      static_cast<byte>(0x44),
      static_cast<byte>(0x78),
      static_cast<byte>(0x99),
      static_cast<byte>(0x7F),
      static_cast<byte>(0xE3)
  };
  span<byte const, 10> const spn(buffer);
  ASSERT_EQ(read_big_endian<std::int64_t>(spn.subspan<0, 8>()), 0x47C8112233447899LL);
  ASSERT_EQ(read_big_endian<std::int64_t>(spn.subspan<2, 8>()), 0x1122334478997FE3LL);
}

/// @test `read_big_endian` returns the expected values with `constexpr` of all integer types.
TEST(Endian, ReadBigEndianConstexpr) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  static constexpr byte buffer[8] = {
      static_cast<byte>(0x11),
      static_cast<byte>(0x22),
      static_cast<byte>(0x33),
      static_cast<byte>(0x44),
      static_cast<byte>(0x55),
      static_cast<byte>(0x66),
      static_cast<byte>(0x77),
      static_cast<byte>(0x88)
  };
  constexpr span<byte const, 8> spn(buffer);

  constexpr auto res_uint8 = read_big_endian<std::uint8_t>(spn.subspan<0, 1>());
  ASSERT_EQ(res_uint8, 0x11);

  constexpr auto res_uint16 = read_big_endian<std::uint16_t>(spn.subspan<0, 2>());
  ASSERT_EQ(res_uint16, 0x1122U);

  constexpr auto res_uint32 = read_big_endian<std::uint32_t>(spn.subspan<0, 4>());
  ASSERT_EQ(res_uint32, 0x11223344U);

  constexpr auto res_uint64 = read_big_endian<std::uint64_t>(spn.subspan<0, 8>());
  ASSERT_EQ(res_uint64, 0x1122334455667788ULL);

  constexpr auto res_int8 = read_big_endian<std::int8_t>(spn.subspan<0, 1>());
  ASSERT_EQ(res_int8, 0x11);

  constexpr auto res_int16 = read_big_endian<std::int16_t>(spn.subspan<0, 2>());
  ASSERT_EQ(res_int16, 0x1122);

  constexpr auto res_int32 = read_big_endian<std::int32_t>(spn.subspan<0, 4>());
  ASSERT_EQ(res_int32, 0x11223344);

  constexpr auto res_int64 = read_big_endian<std::int64_t>(spn.subspan<0, 8>());
  ASSERT_EQ(res_int64, 0x1122334455667788LL);
}

/// @test `write_little_endian` generates the expected value with `uint8_t` type.
TEST(Endian, WriteLittleEndianUint8) {
  arene::base::array<byte, 2> buffer{};
  span<byte, 2> const spn{buffer};
  write_little_endian<std::uint8_t>(0x44, spn.subspan<0, 1>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x44));
  write_little_endian<std::uint8_t>(0xCD, spn.subspan<1, 1>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[1], static_cast<byte>(0xCD));
}

/// @test `write_little_endian` generates the expected value with `uint16_t` type.
TEST(Endian, WriteLittleEndianUint16) {
  arene::base::array<byte, 4> buffer{};
  span<byte, 4> const spn{buffer};
  write_little_endian<std::uint16_t>(0x4455, spn.subspan<0, 2>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x55));
  ASSERT_EQ(buffer[1], static_cast<byte>(0x44));
  write_little_endian<std::uint16_t>(0xCDFE, spn.subspan<2, 2>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x55));
  ASSERT_EQ(buffer[1], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[2], static_cast<byte>(0xFE));
  ASSERT_EQ(buffer[3], static_cast<byte>(0xCD));
}

/// @test `write_little_endian` generates the expected value with `uint32_t` type.
TEST(Endian, WriteLittleEndianUint32) {
  arene::base::array<byte, 8> buffer{};
  span<byte, 8> const spn{buffer};
  write_little_endian<std::uint32_t>(0x11223344U, spn.subspan<0, 4>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[1], static_cast<byte>(0x33));
  ASSERT_EQ(buffer[2], static_cast<byte>(0x22));
  ASSERT_EQ(buffer[3], static_cast<byte>(0x11));
  write_little_endian<std::uint32_t>(0xAABBCCDDU, spn.subspan<4, 4>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[1], static_cast<byte>(0x33));
  ASSERT_EQ(buffer[2], static_cast<byte>(0x22));
  ASSERT_EQ(buffer[3], static_cast<byte>(0x11));
  ASSERT_EQ(buffer[4], static_cast<byte>(0xDD));
  ASSERT_EQ(buffer[5], static_cast<byte>(0xCC));
  ASSERT_EQ(buffer[6], static_cast<byte>(0xBB));
  ASSERT_EQ(buffer[7], static_cast<byte>(0xAA));
}

/// @test `write_little_endian` generates the expected value with `uint64_t` type.
TEST(Endian, WriteLittleEndianUint64) {
  arene::base::array<byte, 16> buffer{};
  span<byte, 16> const spn{buffer};
  write_little_endian<std::uint64_t>(0x1122334455667788ULL, spn.subspan<0, 8>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x88));
  ASSERT_EQ(buffer[1], static_cast<byte>(0x77));
  ASSERT_EQ(buffer[2], static_cast<byte>(0x66));
  ASSERT_EQ(buffer[3], static_cast<byte>(0x55));
  ASSERT_EQ(buffer[4], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[5], static_cast<byte>(0x33));
  ASSERT_EQ(buffer[6], static_cast<byte>(0x22));
  ASSERT_EQ(buffer[7], static_cast<byte>(0x11));
  write_little_endian<std::uint64_t>(0xAABBCCDDEEFF1122ULL, spn.subspan<8, 8>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x88));
  ASSERT_EQ(buffer[1], static_cast<byte>(0x77));
  ASSERT_EQ(buffer[2], static_cast<byte>(0x66));
  ASSERT_EQ(buffer[3], static_cast<byte>(0x55));
  ASSERT_EQ(buffer[4], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[5], static_cast<byte>(0x33));
  ASSERT_EQ(buffer[6], static_cast<byte>(0x22));
  ASSERT_EQ(buffer[7], static_cast<byte>(0x11));
  ASSERT_EQ(buffer[8], static_cast<byte>(0x22));
  ASSERT_EQ(buffer[9], static_cast<byte>(0x11));
  ASSERT_EQ(buffer[10], static_cast<byte>(0xFF));
  ASSERT_EQ(buffer[11], static_cast<byte>(0xEE));
  ASSERT_EQ(buffer[12], static_cast<byte>(0xDD));
  ASSERT_EQ(buffer[13], static_cast<byte>(0xCC));
  ASSERT_EQ(buffer[14], static_cast<byte>(0xBB));
  ASSERT_EQ(buffer[15], static_cast<byte>(0xAA));
}

/// @test `write_little_endian` generates the expected value with `int8_t` type.
TEST(Endian, WriteLittleEndianInt8) {
  arene::base::array<byte, 2> buffer{};
  span<byte, 2> const spn{buffer};
  write_little_endian<std::int8_t>(0x44, spn.subspan<0, 1>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x44));
  write_little_endian<std::int8_t>(static_cast<std::int8_t>(0xCD), spn.subspan<1, 1>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[1], static_cast<byte>(0xCD));
}

/// @test `write_little_endian` generates the expected value with `int16_t` type.
TEST(Endian, WriteLittleEndianInt16) {
  arene::base::array<byte, 4> buffer{};
  span<byte, 4> const spn{buffer};
  write_little_endian<std::int16_t>(0x4455, spn.subspan<0, 2>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x55));
  ASSERT_EQ(buffer[1], static_cast<byte>(0x44));
  write_little_endian<std::int16_t>(static_cast<std::int16_t>(0xCDFE), spn.subspan<2, 2>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x55));
  ASSERT_EQ(buffer[1], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[2], static_cast<byte>(0xFE));
  ASSERT_EQ(buffer[3], static_cast<byte>(0xCD));
}

/// @test `write_little_endian` generates the expected value with `int32_t` type.
TEST(Endian, WriteLittleEndianInt32) {
  arene::base::array<byte, 8> buffer{};
  span<byte, 8> const spn{buffer};
  write_little_endian<std::int32_t>(0x11223344, spn.subspan<0, 4>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[1], static_cast<byte>(0x33));
  ASSERT_EQ(buffer[2], static_cast<byte>(0x22));
  ASSERT_EQ(buffer[3], static_cast<byte>(0x11));
  write_little_endian<std::int32_t>(static_cast<std::int32_t>(0xAABBCCDD), spn.subspan<4, 4>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[1], static_cast<byte>(0x33));
  ASSERT_EQ(buffer[2], static_cast<byte>(0x22));
  ASSERT_EQ(buffer[3], static_cast<byte>(0x11));
  ASSERT_EQ(buffer[4], static_cast<byte>(0xDD));
  ASSERT_EQ(buffer[5], static_cast<byte>(0xCC));
  ASSERT_EQ(buffer[6], static_cast<byte>(0xBB));
  ASSERT_EQ(buffer[7], static_cast<byte>(0xAA));
}

/// @test `write_little_endian` generates the expected value with `int64_t` type.
TEST(Endian, WriteLittleEndianInt64) {
  arene::base::array<byte, 16> buffer{};
  span<byte, 16> const spn{buffer};
  write_little_endian<std::int64_t>(0x1122334455667788LL, spn.subspan<0, 8>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x88));
  ASSERT_EQ(buffer[1], static_cast<byte>(0x77));
  ASSERT_EQ(buffer[2], static_cast<byte>(0x66));
  ASSERT_EQ(buffer[3], static_cast<byte>(0x55));
  ASSERT_EQ(buffer[4], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[5], static_cast<byte>(0x33));
  ASSERT_EQ(buffer[6], static_cast<byte>(0x22));
  ASSERT_EQ(buffer[7], static_cast<byte>(0x11));
  write_little_endian<std::int64_t>(static_cast<std::int64_t>(0xAABBCCDDEEFF1122LL), spn.subspan<8, 8>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x88));
  ASSERT_EQ(buffer[1], static_cast<byte>(0x77));
  ASSERT_EQ(buffer[2], static_cast<byte>(0x66));
  ASSERT_EQ(buffer[3], static_cast<byte>(0x55));
  ASSERT_EQ(buffer[4], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[5], static_cast<byte>(0x33));
  ASSERT_EQ(buffer[6], static_cast<byte>(0x22));
  ASSERT_EQ(buffer[7], static_cast<byte>(0x11));
  ASSERT_EQ(buffer[8], static_cast<byte>(0x22));
  ASSERT_EQ(buffer[9], static_cast<byte>(0x11));
  ASSERT_EQ(buffer[10], static_cast<byte>(0xFF));
  ASSERT_EQ(buffer[11], static_cast<byte>(0xEE));
  ASSERT_EQ(buffer[12], static_cast<byte>(0xDD));
  ASSERT_EQ(buffer[13], static_cast<byte>(0xCC));
  ASSERT_EQ(buffer[14], static_cast<byte>(0xBB));
  ASSERT_EQ(buffer[15], static_cast<byte>(0xAA));
}

/// @test `write_big_endian` generates the expected value with `uint8_t` type.
TEST(Endian, WriteBigEndianUint8) {
  arene::base::array<byte, 2> buffer{};
  span<byte, 2> const spn{buffer};
  write_big_endian<std::uint8_t>(0x44, spn.subspan<0, 1>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x44));
  write_big_endian<std::uint8_t>(0xCD, spn.subspan<1, 1>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[1], static_cast<byte>(0xCD));
}

/// @test `write_big_endian` generates the expected value with `uint16_t` type.
TEST(Endian, WriteBigEndianUint16) {
  arene::base::array<byte, 4> buffer{};
  span<byte, 4> const spn{buffer};
  write_big_endian<std::uint16_t>(0x4455, spn.subspan<0, 2>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[1], static_cast<byte>(0x55));
  write_big_endian<std::uint16_t>(0xCDFE, spn.subspan<2, 2>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[1], static_cast<byte>(0x55));
  ASSERT_EQ(buffer[2], static_cast<byte>(0xCD));
  ASSERT_EQ(buffer[3], static_cast<byte>(0xFE));
}

/// @test `write_big_endian` generates the expected value with `uint32_t` type.
TEST(Endian, WriteBigEndianUint32) {
  arene::base::array<byte, 8> buffer{};
  span<byte, 8> const spn{buffer};
  write_big_endian<std::uint32_t>(0x11223344U, spn.subspan<0, 4>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x11));
  ASSERT_EQ(buffer[1], static_cast<byte>(0x22));
  ASSERT_EQ(buffer[2], static_cast<byte>(0x33));
  ASSERT_EQ(buffer[3], static_cast<byte>(0x44));
  write_big_endian<std::uint32_t>(0xAABBCCDDU, spn.subspan<4, 4>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x11));
  ASSERT_EQ(buffer[1], static_cast<byte>(0x22));
  ASSERT_EQ(buffer[2], static_cast<byte>(0x33));
  ASSERT_EQ(buffer[3], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[4], static_cast<byte>(0xAA));
  ASSERT_EQ(buffer[5], static_cast<byte>(0xBB));
  ASSERT_EQ(buffer[6], static_cast<byte>(0xCC));
  ASSERT_EQ(buffer[7], static_cast<byte>(0xDD));
}

/// @test `write_big_endian` generates the expected value with `uint64_t` type.
TEST(Endian, WriteBigEndianUint64) {
  arene::base::array<byte, 16> buffer{};
  span<byte, 16> const spn{buffer};
  write_big_endian<std::uint64_t>(0x1122334455667788ULL, spn.subspan<0, 8>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x11));
  ASSERT_EQ(buffer[1], static_cast<byte>(0x22));
  ASSERT_EQ(buffer[2], static_cast<byte>(0x33));
  ASSERT_EQ(buffer[3], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[4], static_cast<byte>(0x55));
  ASSERT_EQ(buffer[5], static_cast<byte>(0x66));
  ASSERT_EQ(buffer[6], static_cast<byte>(0x77));
  ASSERT_EQ(buffer[7], static_cast<byte>(0x88));
  write_big_endian<std::uint64_t>(0xAABBCCDDEEFF1122ULL, spn.subspan<8, 8>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x11));
  ASSERT_EQ(buffer[1], static_cast<byte>(0x22));
  ASSERT_EQ(buffer[2], static_cast<byte>(0x33));
  ASSERT_EQ(buffer[3], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[4], static_cast<byte>(0x55));
  ASSERT_EQ(buffer[5], static_cast<byte>(0x66));
  ASSERT_EQ(buffer[6], static_cast<byte>(0x77));
  ASSERT_EQ(buffer[7], static_cast<byte>(0x88));
  ASSERT_EQ(buffer[8], static_cast<byte>(0xAA));
  ASSERT_EQ(buffer[9], static_cast<byte>(0xBB));
  ASSERT_EQ(buffer[10], static_cast<byte>(0xCC));
  ASSERT_EQ(buffer[11], static_cast<byte>(0xDD));
  ASSERT_EQ(buffer[12], static_cast<byte>(0xEE));
  ASSERT_EQ(buffer[13], static_cast<byte>(0xFF));
  ASSERT_EQ(buffer[14], static_cast<byte>(0x11));
  ASSERT_EQ(buffer[15], static_cast<byte>(0x22));
}

/// @test `write_big_endian` generates the expected value with `int8_t` type.
TEST(Endian, WriteBigEndianInt8) {
  arene::base::array<byte, 2> buffer{};
  span<byte, 2> const spn{buffer};
  write_big_endian<std::int8_t>(0x44, spn.subspan<0, 1>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x44));
  write_big_endian<std::int8_t>(static_cast<std::int8_t>(0xCD), spn.subspan<1, 1>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[1], static_cast<byte>(0xCD));
}

/// @test `write_big_endian` generates the expected value with `int16_t` type.
TEST(Endian, WriteBigEndianInt16) {
  arene::base::array<byte, 4> buffer{};
  span<byte, 4> const spn{buffer};
  write_big_endian<std::int16_t>(0x4455, spn.subspan<0, 2>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[1], static_cast<byte>(0x55));
  write_big_endian<std::int16_t>(static_cast<std::int16_t>(0xCDFE), spn.subspan<2, 2>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[1], static_cast<byte>(0x55));
  ASSERT_EQ(buffer[2], static_cast<byte>(0xCD));
  ASSERT_EQ(buffer[3], static_cast<byte>(0xFE));
}

/// @test `write_big_endian` generates the expected value with `int32_t` type.
TEST(Endian, WriteBigEndianInt32) {
  arene::base::array<byte, 8> buffer{};
  span<byte, 8> const spn{buffer};
  write_big_endian<std::int32_t>(0x11223344, spn.subspan<0, 4>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x11));
  ASSERT_EQ(buffer[1], static_cast<byte>(0x22));
  ASSERT_EQ(buffer[2], static_cast<byte>(0x33));
  ASSERT_EQ(buffer[3], static_cast<byte>(0x44));
  write_big_endian<std::int32_t>(static_cast<std::int32_t>(0xAABBCCDD), spn.subspan<4, 4>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x11));
  ASSERT_EQ(buffer[1], static_cast<byte>(0x22));
  ASSERT_EQ(buffer[2], static_cast<byte>(0x33));
  ASSERT_EQ(buffer[3], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[4], static_cast<byte>(0xAA));
  ASSERT_EQ(buffer[5], static_cast<byte>(0xBB));
  ASSERT_EQ(buffer[6], static_cast<byte>(0xCC));
  ASSERT_EQ(buffer[7], static_cast<byte>(0xDD));
}

/// @test `write_big_endian` generates the expected value with `int64_t` type.
TEST(Endian, WriteBigEndianInt64) {
  arene::base::array<byte, 16> buffer{};
  span<byte, 16> const spn{buffer};
  write_big_endian<std::int64_t>(0x1122334455667788LL, spn.subspan<0, 8>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x11));
  ASSERT_EQ(buffer[1], static_cast<byte>(0x22));
  ASSERT_EQ(buffer[2], static_cast<byte>(0x33));
  ASSERT_EQ(buffer[3], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[4], static_cast<byte>(0x55));
  ASSERT_EQ(buffer[5], static_cast<byte>(0x66));
  ASSERT_EQ(buffer[6], static_cast<byte>(0x77));
  ASSERT_EQ(buffer[7], static_cast<byte>(0x88));
  write_big_endian<std::int64_t>(static_cast<std::int64_t>(0xAABBCCDDEEFF1122LL), spn.subspan<8, 8>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x11));
  ASSERT_EQ(buffer[1], static_cast<byte>(0x22));
  ASSERT_EQ(buffer[2], static_cast<byte>(0x33));
  ASSERT_EQ(buffer[3], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[4], static_cast<byte>(0x55));
  ASSERT_EQ(buffer[5], static_cast<byte>(0x66));
  ASSERT_EQ(buffer[6], static_cast<byte>(0x77));
  ASSERT_EQ(buffer[7], static_cast<byte>(0x88));
  ASSERT_EQ(buffer[8], static_cast<byte>(0xAA));
  ASSERT_EQ(buffer[9], static_cast<byte>(0xBB));
  ASSERT_EQ(buffer[10], static_cast<byte>(0xCC));
  ASSERT_EQ(buffer[11], static_cast<byte>(0xDD));
  ASSERT_EQ(buffer[12], static_cast<byte>(0xEE));
  ASSERT_EQ(buffer[13], static_cast<byte>(0xFF));
  ASSERT_EQ(buffer[14], static_cast<byte>(0x11));
  ASSERT_EQ(buffer[15], static_cast<byte>(0x22));
}

constexpr auto write_little_endian_constexpr() -> arene::base::array<byte, 15> {
  arene::base::array<byte, 15> res{};
  span<byte, 15> const spn{res};
  write_little_endian<std::uint8_t>(0x11U, spn.subspan<0, 1>());
  write_little_endian<std::uint16_t>(0x3322U, spn.subspan<1, 2>());
  write_little_endian<std::uint32_t>(0x77665544U, spn.subspan<3, 4>());
  write_little_endian<std::uint64_t>(0xFFEEDDCCBBAA9988ULL, spn.subspan<7, 8>());
  return res;
}

constexpr auto write_big_endian_constexpr() -> arene::base::array<byte, 15> {
  arene::base::array<byte, 15> res{};
  span<byte, 15> const spn{res};
  write_big_endian<std::uint8_t>(0x11U, spn.subspan<0, 1>());
  write_big_endian<std::uint16_t>(0x3322U, spn.subspan<1, 2>());
  write_big_endian<std::uint32_t>(0x77665544U, spn.subspan<3, 4>());
  write_big_endian<std::uint64_t>(0xFFEEDDCCBBAA9988ULL, spn.subspan<7, 8>());
  return res;
}

/// @test `write_little_endian` generates the expected values with `constexpr` of all unsigned integer types.
TEST(Endian, WriteLittleEndianConstexpr) {
  constexpr arene::base::array<byte, 15> res = write_little_endian_constexpr();
  ASSERT_EQ(res[0], static_cast<byte>(0x11));
  ASSERT_EQ(res[1], static_cast<byte>(0x22));
  ASSERT_EQ(res[2], static_cast<byte>(0x33));
  ASSERT_EQ(res[3], static_cast<byte>(0x44));
  ASSERT_EQ(res[4], static_cast<byte>(0x55));
  ASSERT_EQ(res[5], static_cast<byte>(0x66));
  ASSERT_EQ(res[6], static_cast<byte>(0x77));
  ASSERT_EQ(res[7], static_cast<byte>(0x88));
  ASSERT_EQ(res[8], static_cast<byte>(0x99));
  ASSERT_EQ(res[9], static_cast<byte>(0xAA));
  ASSERT_EQ(res[10], static_cast<byte>(0xBB));
  ASSERT_EQ(res[11], static_cast<byte>(0xCC));
  ASSERT_EQ(res[12], static_cast<byte>(0xDD));
  ASSERT_EQ(res[13], static_cast<byte>(0xEE));
  ASSERT_EQ(res[14], static_cast<byte>(0xFF));
}

/// @test `write_big_endian` generates the expected values with `constexpr` of all unsigned integer types.
TEST(Endian, WriteBigEndianConstexpr) {
  constexpr arene::base::array<byte, 15> res = write_big_endian_constexpr();
  ASSERT_EQ(res[0], static_cast<byte>(0x11));
  ASSERT_EQ(res[1], static_cast<byte>(0x33));
  ASSERT_EQ(res[2], static_cast<byte>(0x22));
  ASSERT_EQ(res[3], static_cast<byte>(0x77));
  ASSERT_EQ(res[4], static_cast<byte>(0x66));
  ASSERT_EQ(res[5], static_cast<byte>(0x55));
  ASSERT_EQ(res[6], static_cast<byte>(0x44));
  ASSERT_EQ(res[7], static_cast<byte>(0xFF));
  ASSERT_EQ(res[8], static_cast<byte>(0xEE));
  ASSERT_EQ(res[9], static_cast<byte>(0xDD));
  ASSERT_EQ(res[10], static_cast<byte>(0xCC));
  ASSERT_EQ(res[11], static_cast<byte>(0xBB));
  ASSERT_EQ(res[12], static_cast<byte>(0xAA));
  ASSERT_EQ(res[13], static_cast<byte>(0x99));
  ASSERT_EQ(res[14], static_cast<byte>(0x88));
}

/// @test `read_little_endian` returns the expected value with `char` type.
TEST(Endian, ReadLittleEndianChar) {
  arene::base::array<byte, 2> const buffer = {static_cast<byte>(0x47), static_cast<byte>(0xC8)};
  span<byte const, 2> const spn(buffer);
  ASSERT_EQ(read_little_endian<char>(spn.subspan<0, 1>()), static_cast<char>(0x47));
  ASSERT_EQ(read_little_endian<char>(spn.subspan<1, 1>()), static_cast<char>(0xC8));
}

/// @test `read_little_endian` returns the expected value with `signed char` type.
TEST(Endian, ReadLittleEndianSignedChar) {
  arene::base::array<byte, 2> const buffer = {static_cast<byte>(0x47), static_cast<byte>(0xC8)};
  span<byte const, 2> const spn(buffer);
  ASSERT_EQ(read_little_endian<char>(spn.subspan<0, 1>()), 0x47);
  ASSERT_EQ(read_little_endian<signed char>(spn.subspan<1, 1>()), static_cast<signed char>(0xC8));
}

/// @test `read_little_endian` returns the expected value with `unsigned char` type.
TEST(Endian, ReadLittleEndianUnsignedChar) {
  arene::base::array<byte, 2> const buffer = {static_cast<byte>(0x47), static_cast<byte>(0xC8)};
  span<byte const, 2> const spn(buffer);
  ASSERT_EQ(read_little_endian<unsigned char>(spn.subspan<0, 1>()), 0x47);
  ASSERT_EQ(read_little_endian<unsigned char>(spn.subspan<1, 1>()), 0xC8);
}

/// @test `write_little_endian` generates the expected value with `char` type.
TEST(Endian, WriteLittleEndianChar) {
  arene::base::array<byte, 2> buffer{};
  span<byte, 2> const spn{buffer};
  write_little_endian<char>(0x44, spn.subspan<0, 1>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x44));
  write_little_endian<char>(static_cast<char>(0xCD), spn.subspan<1, 1>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[1], static_cast<byte>(0xCD));
}

/// @test `write_little_endian` generates the expected value with `signed char` type.
TEST(Endian, WriteLittleEndianSignedChar) {
  arene::base::array<byte, 2> buffer{};
  span<byte, 2> const spn{buffer};
  write_little_endian<signed char>(0x44, spn.subspan<0, 1>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x44));
  write_little_endian<signed char>(static_cast<signed char>(0xCD), spn.subspan<1, 1>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[1], static_cast<byte>(0xCD));
}

/// @test `write_little_endian` generates the expected value with `unsigned char` type.
TEST(Endian, WriteLittleEndianUnsignedChar) {
  arene::base::array<byte, 2> buffer{};
  span<byte, 2> const spn{buffer};
  write_little_endian<unsigned char>(0x44, spn.subspan<0, 1>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x44));
  write_little_endian<unsigned char>(0xCD, spn.subspan<1, 1>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[1], static_cast<byte>(0xCD));
}

/// @test `read_big_endian` returns the expected value with `char` type.
TEST(Endian, ReadBigEndianChar) {
  arene::base::array<byte, 2> const buffer = {static_cast<byte>(0x47), static_cast<byte>(0xC8)};
  span<byte const, 2> const spn(buffer);
  ASSERT_EQ(read_big_endian<char>(spn.subspan<0, 1>()), static_cast<char>(0x47));
  ASSERT_EQ(read_big_endian<char>(spn.subspan<1, 1>()), static_cast<char>(0xC8));
}

/// @test `read_big_endian` returns the expected value with `signed char` type.
TEST(Endian, ReadBigEndianSignedChar) {
  arene::base::array<byte, 2> const buffer = {static_cast<byte>(0x47), static_cast<byte>(0xC8)};
  span<byte const, 2> const spn(buffer);
  ASSERT_EQ(read_big_endian<char>(spn.subspan<0, 1>()), 0x47);
  ASSERT_EQ(read_big_endian<signed char>(spn.subspan<1, 1>()), static_cast<signed char>(0xC8));
}

/// @test `read_big_endian` returns the expected value with `unsigned char` type.
TEST(Endian, ReadBigEndianUnsignedChar) {
  arene::base::array<byte, 2> const buffer = {static_cast<byte>(0x47), static_cast<byte>(0xC8)};
  span<byte const, 2> const spn(buffer);
  ASSERT_EQ(read_big_endian<unsigned char>(spn.subspan<0, 1>()), 0x47);
  ASSERT_EQ(read_big_endian<unsigned char>(spn.subspan<1, 1>()), 0xC8);
}

/// @test `write_big_endian` generates the expected value with `char` type.
TEST(Endian, WriteBigEndianChar) {
  arene::base::array<byte, 2> buffer{};
  span<byte, 2> const spn{buffer};
  write_big_endian<char>(0x44, spn.subspan<0, 1>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x44));
  write_big_endian<char>(static_cast<char>(0xCD), spn.subspan<1, 1>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[1], static_cast<byte>(0xCD));
}

/// @test `write_big_endian` generates the expected value with `signed char` type.
TEST(Endian, WriteBigEndianSignedChar) {
  arene::base::array<byte, 2> buffer{};
  span<byte, 2> const spn{buffer};
  write_big_endian<signed char>(0x44, spn.subspan<0, 1>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x44));
  write_big_endian<signed char>(static_cast<signed char>(0xCD), spn.subspan<1, 1>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[1], static_cast<byte>(0xCD));
}

/// @test `write_big_endian` generates the expected value with `unsigned char` type.
TEST(Endian, WriteBigEndianUnsignedChar) {
  arene::base::array<byte, 2> buffer{};
  span<byte, 2> const spn{buffer};
  write_big_endian<unsigned char>(0x44, spn.subspan<0, 1>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x44));
  write_big_endian<unsigned char>(0xCD, spn.subspan<1, 1>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[1], static_cast<byte>(0xCD));
}

enum class enum1 : std::uint8_t { a = 0x1a, b = 0xb2, c = 0xc3 };
enum class enum2 : std::uint16_t { a = 0x1122, b = 0x3344, c = 0x5566 };
enum class enum3 : std::uint32_t { a = 0x11223344, b = 0x55667788, c = 0x99aabbcc };
enum class enum4 : std::uint64_t { a = 0x1122334455667788, b = 0x99aabbccddeeff00, c = 0x1a2b3c4d5e6f7088 };

/// @test `write_little_endian` generates the expected value with `byte` type.
TEST(Endian, WriteLittleEndianByte) {
  arene::base::array<byte, 2> buffer{};
  span<byte, 2> const spn{buffer};
  byte const first = static_cast<byte>(0xa5);
  byte const second = static_cast<byte>(0x5a);
  write_little_endian<byte>(first, spn.subspan<0, 1>());
  ASSERT_EQ(buffer[0], first);
  write_little_endian<byte>(second, spn.subspan<1, 1>());
  ASSERT_EQ(buffer[0], first);
  ASSERT_EQ(buffer[1], second);
}

/// @test `write_little_endian` generates the expected value with `uint8_t` enum.
TEST(Endian, WriteLittleEndianEnum8) {
  arene::base::array<byte, 2> buffer{};
  span<byte, 2> const spn{buffer};
  write_little_endian<enum1>(enum1::a, spn.subspan<0, 1>());
  ASSERT_EQ(buffer[0], static_cast<byte>(enum1::a));
  write_little_endian<enum1>(enum1::b, spn.subspan<1, 1>());
  ASSERT_EQ(buffer[0], static_cast<byte>(enum1::a));
  ASSERT_EQ(buffer[1], static_cast<byte>(enum1::b));
}

/// @test `read_little_endian` returns the expected value with `uint8_t` enum.
TEST(Endian, ReadLittleEndianEnum8) {
  arene::base::array<byte, 2> const buffer = {static_cast<byte>(0x1a), static_cast<byte>(0xb2)};
  span<byte const, 2> const spn(buffer);
  ASSERT_EQ(read_little_endian<enum1>(spn.subspan<0, 1>()), enum1::a);
  ASSERT_EQ(read_little_endian<enum1>(spn.subspan<1, 1>()), enum1::b);
}

/// @test `read_little_endian` returns the expected value with `uint16_t` enum.
TEST(Endian, ReadLittleEndianEnum16) {
  arene::base::array<byte, 4> const buffer =
      {static_cast<byte>(0x22), static_cast<byte>(0x11), static_cast<byte>(0x44), static_cast<byte>(0x33)};
  span<byte const, 4> const spn(buffer);
  ASSERT_EQ(read_little_endian<enum2>(spn.subspan<0, 2>()), enum2::a);
  ASSERT_EQ(read_little_endian<enum2>(spn.subspan<2, 2>()), enum2::b);
}

/// @test `write_little_endian` generates the expected value with `uint16_t` enum.
TEST(Endian, WriteLittleEndianEnum16) {
  arene::base::array<byte, 4> buffer{};
  span<byte, 4> const spn{buffer};
  write_little_endian<enum2>(enum2::a, spn.subspan<0, 2>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x22));
  ASSERT_EQ(buffer[1], static_cast<byte>(0x11));
  write_little_endian<enum2>(enum2::b, spn.subspan<2, 2>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x22));
  ASSERT_EQ(buffer[1], static_cast<byte>(0x11));
  ASSERT_EQ(buffer[2], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[3], static_cast<byte>(0x33));
}

/// @test `read_little_endian` returns the expected value with `uint32_t` enum.
TEST(Endian, ReadLittleEndianEnum32) {
  arene::base::array<byte, 8> const buffer = {
      static_cast<byte>(0x44),
      static_cast<byte>(0x33),
      static_cast<byte>(0x22),
      static_cast<byte>(0x11),
      static_cast<byte>(0x88),
      static_cast<byte>(0x77),
      static_cast<byte>(0x66),
      static_cast<byte>(0x55)
  };
  span<byte const, 8> const spn(buffer);
  ASSERT_EQ(read_little_endian<enum3>(spn.subspan<0, 4>()), enum3::a);
  ASSERT_EQ(read_little_endian<enum3>(spn.subspan<4, 4>()), enum3::b);
}

/// @test `read_little_endian` returns the expected value with `uint64_t` enum.
TEST(Endian, ReadLittleEndianEnum64) {
  arene::base::array<byte, 16> const buffer = {
      static_cast<byte>(0x88),
      static_cast<byte>(0x77),
      static_cast<byte>(0x66),
      static_cast<byte>(0x55),
      static_cast<byte>(0x44),
      static_cast<byte>(0x33),
      static_cast<byte>(0x22),
      static_cast<byte>(0x11),
      static_cast<byte>(0x00),
      static_cast<byte>(0xff),
      static_cast<byte>(0xee),
      static_cast<byte>(0xdd),
      static_cast<byte>(0xcc),
      static_cast<byte>(0xbb),
      static_cast<byte>(0xaa),
      static_cast<byte>(0x99)
  };
  span<byte const, 16> const spn(buffer);
  ASSERT_EQ(read_little_endian<enum4>(spn.subspan<0, 8>()), enum4::a);
  ASSERT_EQ(read_little_endian<enum4>(spn.subspan<8, 8>()), enum4::b);
}

/// @test `write_little_endian` generates the expected value with `uint32_t` enum.
TEST(Endian, WriteLittleEndianEnum32) {
  arene::base::array<byte, 8> buffer{};
  span<byte, 8> const spn{buffer};
  write_little_endian<enum3>(enum3::a, spn.subspan<0, 4>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[1], static_cast<byte>(0x33));
  ASSERT_EQ(buffer[2], static_cast<byte>(0x22));
  ASSERT_EQ(buffer[3], static_cast<byte>(0x11));
  write_little_endian<enum3>(enum3::b, spn.subspan<4, 4>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[1], static_cast<byte>(0x33));
  ASSERT_EQ(buffer[2], static_cast<byte>(0x22));
  ASSERT_EQ(buffer[3], static_cast<byte>(0x11));
  ASSERT_EQ(buffer[4], static_cast<byte>(0x88));
  ASSERT_EQ(buffer[5], static_cast<byte>(0x77));
  ASSERT_EQ(buffer[6], static_cast<byte>(0x66));
  ASSERT_EQ(buffer[7], static_cast<byte>(0x55));
}

/// @test `write_little_endian` generates the expected value with `uint64_t` enum.
TEST(Endian, WriteLittleEndianEnum64) {
  arene::base::array<byte, 16> buffer{};
  span<byte, 16> const spn{buffer};
  write_little_endian<enum4>(enum4::a, spn.subspan<0, 8>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x88));
  ASSERT_EQ(buffer[1], static_cast<byte>(0x77));
  ASSERT_EQ(buffer[2], static_cast<byte>(0x66));
  ASSERT_EQ(buffer[3], static_cast<byte>(0x55));
  ASSERT_EQ(buffer[4], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[5], static_cast<byte>(0x33));
  ASSERT_EQ(buffer[6], static_cast<byte>(0x22));
  ASSERT_EQ(buffer[7], static_cast<byte>(0x11));
  write_little_endian<enum4>(enum4::b, spn.subspan<8, 8>());
  ASSERT_EQ(buffer[0], static_cast<byte>(0x88));
  ASSERT_EQ(buffer[1], static_cast<byte>(0x77));
  ASSERT_EQ(buffer[2], static_cast<byte>(0x66));
  ASSERT_EQ(buffer[3], static_cast<byte>(0x55));
  ASSERT_EQ(buffer[4], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[5], static_cast<byte>(0x33));
  ASSERT_EQ(buffer[6], static_cast<byte>(0x22));
  ASSERT_EQ(buffer[7], static_cast<byte>(0x11));
  ASSERT_EQ(buffer[8], static_cast<byte>(0x00));
  ASSERT_EQ(buffer[9], static_cast<byte>(0xff));
  ASSERT_EQ(buffer[10], static_cast<byte>(0xee));
  ASSERT_EQ(buffer[11], static_cast<byte>(0xdd));
  ASSERT_EQ(buffer[12], static_cast<byte>(0xcc));
  ASSERT_EQ(buffer[13], static_cast<byte>(0xbb));
  ASSERT_EQ(buffer[14], static_cast<byte>(0xaa));
  ASSERT_EQ(buffer[15], static_cast<byte>(0x99));
}

/// @test `write_big_endian` generates the expected value with `byte` type.
TEST(Endian, WriteBigEndianByte) {
  arene::base::array<byte, 2> buffer{};
  span<byte, 2> const spn{buffer};
  byte const first = static_cast<byte>(0xa5);
  byte const second = static_cast<byte>(0x5a);
  write_big_endian<byte>(first, spn.subspan<0, 1>());
  ASSERT_EQ(buffer[0], first);
  write_big_endian<byte>(second, spn.subspan<1, 1>());
  ASSERT_EQ(buffer[0], first);
  ASSERT_EQ(buffer[1], second);
}

/// @test `write_big_endian` generates the expected value with `uint8_t` enum.
TEST(Endian, WriteBigEndianEnum8) {
  arene::base::array<byte, 2> buffer{};
  span<byte, 2> const spn{buffer};
  write_big_endian<enum1>(enum1::a, spn.subspan<0, 1>());
  ASSERT_EQ(buffer[0], static_cast<byte>(enum1::a));
  write_big_endian<enum1>(enum1::b, spn.subspan<1, 1>());
  ASSERT_EQ(buffer[0], static_cast<byte>(enum1::a));
  ASSERT_EQ(buffer[1], static_cast<byte>(enum1::b));
}

/// @test `read_big_endian` returns the expected value with `uint8_t` enum.
TEST(Endian, ReadBigEndianEnum8) {
  arene::base::array<byte, 2> const buffer = {static_cast<byte>(0x1a), static_cast<byte>(0xb2)};
  span<byte const, 2> const spn(buffer);
  ASSERT_EQ(read_big_endian<enum1>(spn.subspan<0, 1>()), enum1::a);
  ASSERT_EQ(read_big_endian<enum1>(spn.subspan<1, 1>()), enum1::b);
}

/// @test `read_big_endian` returns the expected value with `uint16_t` enum.
TEST(Endian, ReadBigEndianEnum16) {
  arene::base::array<byte, 4> const buffer =
      {static_cast<byte>(0x11), static_cast<byte>(0x22), static_cast<byte>(0x33), static_cast<byte>(0x44)};
  span<byte const, 4> const spn(buffer);
  ASSERT_EQ(read_big_endian<enum2>(spn.subspan<0, 2>()), enum2::a);
  ASSERT_EQ(read_big_endian<enum2>(spn.subspan<2, 2>()), enum2::b);
}

/// @test `write_big_endian` generates the expected value with `uint16_t` enum.
TEST(Endian, WriteBigEndianEnum16) {
  arene::base::array<byte, 4> buffer{};
  span<byte, 4> const spn{buffer};
  write_big_endian<enum2>(enum2::a, spn.subspan<0, 2>());
  ASSERT_EQ(buffer[1], static_cast<byte>(0x22));
  ASSERT_EQ(buffer[0], static_cast<byte>(0x11));
  write_big_endian<enum2>(enum2::b, spn.subspan<2, 2>());
  ASSERT_EQ(buffer[1], static_cast<byte>(0x22));
  ASSERT_EQ(buffer[0], static_cast<byte>(0x11));
  ASSERT_EQ(buffer[3], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[2], static_cast<byte>(0x33));
}

/// @test `read_big_endian` returns the expected value with `uint32_t` enum.
TEST(Endian, ReadBigEndianEnum32) {
  arene::base::array<byte, 8> const buffer = {
      static_cast<byte>(0x11),
      static_cast<byte>(0x22),
      static_cast<byte>(0x33),
      static_cast<byte>(0x44),
      static_cast<byte>(0x55),
      static_cast<byte>(0x66),
      static_cast<byte>(0x77),
      static_cast<byte>(0x88)
  };
  span<byte const, 8> const spn(buffer);
  ASSERT_EQ(read_big_endian<enum3>(spn.subspan<0, 4>()), enum3::a);
  ASSERT_EQ(read_big_endian<enum3>(spn.subspan<4, 4>()), enum3::b);
}

/// @test `read_big_endian` returns the expected value with `uint64_t` enum.
TEST(Endian, ReadBigEndianEnum64) {
  arene::base::array<byte, 16> const buffer = {
      static_cast<byte>(0x11),
      static_cast<byte>(0x22),
      static_cast<byte>(0x33),
      static_cast<byte>(0x44),
      static_cast<byte>(0x55),
      static_cast<byte>(0x66),
      static_cast<byte>(0x77),
      static_cast<byte>(0x88),
      static_cast<byte>(0x99),
      static_cast<byte>(0xaa),
      static_cast<byte>(0xbb),
      static_cast<byte>(0xcc),
      static_cast<byte>(0xdd),
      static_cast<byte>(0xee),
      static_cast<byte>(0xff),
      static_cast<byte>(0x00)
  };
  span<byte const, 16> const spn(buffer);
  ASSERT_EQ(read_big_endian<enum4>(spn.subspan<0, 8>()), enum4::a);
  ASSERT_EQ(read_big_endian<enum4>(spn.subspan<8, 8>()), enum4::b);
}

/// @test `write_big_endian` generates the expected value with `uint32_t` enum.
TEST(Endian, WriteBigEndianEnum32) {
  arene::base::array<byte, 8> buffer{};
  span<byte, 8> const spn{buffer};
  write_big_endian<enum3>(enum3::a, spn.subspan<0, 4>());
  ASSERT_EQ(buffer[3], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[2], static_cast<byte>(0x33));
  ASSERT_EQ(buffer[1], static_cast<byte>(0x22));
  ASSERT_EQ(buffer[0], static_cast<byte>(0x11));
  write_big_endian<enum3>(enum3::b, spn.subspan<4, 4>());
  ASSERT_EQ(buffer[3], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[2], static_cast<byte>(0x33));
  ASSERT_EQ(buffer[1], static_cast<byte>(0x22));
  ASSERT_EQ(buffer[0], static_cast<byte>(0x11));
  ASSERT_EQ(buffer[7], static_cast<byte>(0x88));
  ASSERT_EQ(buffer[6], static_cast<byte>(0x77));
  ASSERT_EQ(buffer[5], static_cast<byte>(0x66));
  ASSERT_EQ(buffer[4], static_cast<byte>(0x55));
}

/// @test `write_big_endian` generates the expected value with `uint64_t` enum.
TEST(Endian, WriteBigEndianEnum64) {
  arene::base::array<byte, 16> buffer{};
  span<byte, 16> const spn{buffer};
  write_big_endian<enum4>(enum4::a, spn.subspan<0, 8>());
  ASSERT_EQ(buffer[7], static_cast<byte>(0x88));
  ASSERT_EQ(buffer[6], static_cast<byte>(0x77));
  ASSERT_EQ(buffer[5], static_cast<byte>(0x66));
  ASSERT_EQ(buffer[4], static_cast<byte>(0x55));
  ASSERT_EQ(buffer[3], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[2], static_cast<byte>(0x33));
  ASSERT_EQ(buffer[1], static_cast<byte>(0x22));
  ASSERT_EQ(buffer[0], static_cast<byte>(0x11));
  write_big_endian<enum4>(enum4::b, spn.subspan<8, 8>());
  ASSERT_EQ(buffer[7], static_cast<byte>(0x88));
  ASSERT_EQ(buffer[6], static_cast<byte>(0x77));
  ASSERT_EQ(buffer[5], static_cast<byte>(0x66));
  ASSERT_EQ(buffer[4], static_cast<byte>(0x55));
  ASSERT_EQ(buffer[3], static_cast<byte>(0x44));
  ASSERT_EQ(buffer[2], static_cast<byte>(0x33));
  ASSERT_EQ(buffer[1], static_cast<byte>(0x22));
  ASSERT_EQ(buffer[0], static_cast<byte>(0x11));
  ASSERT_EQ(buffer[15], static_cast<byte>(0x00));
  ASSERT_EQ(buffer[14], static_cast<byte>(0xff));
  ASSERT_EQ(buffer[13], static_cast<byte>(0xee));
  ASSERT_EQ(buffer[12], static_cast<byte>(0xdd));
  ASSERT_EQ(buffer[11], static_cast<byte>(0xcc));
  ASSERT_EQ(buffer[10], static_cast<byte>(0xbb));
  ASSERT_EQ(buffer[9], static_cast<byte>(0xaa));
  ASSERT_EQ(buffer[8], static_cast<byte>(0x99));
}

template <typename FloatingPoint>
constexpr FloatingPoint test_value = static_cast<FloatingPoint>(1.2345e32L);

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "Not performing arithmetic, just reading and writing bytes");

/// @test `write_little_endian` generates the expected value with `float` type,
/// and `read_little_endian` returns the expected value with `float` type.
TEST(Endian, LittleEndianFloat) {
  arene::base::array<byte, sizeof(float)> buffer{};
  span<byte, sizeof(float)> const spn(buffer);

  float const value = test_value<float>;
  write_little_endian<float>(value, spn);

  auto const bytes = as_bytes(span<float const, 1>(&value, 1));

  if (endian::native == endian::little) {
    for (unsigned idx = 0; idx < sizeof(float); ++idx) {
      ASSERT_EQ(bytes[idx], buffer[idx]);
    }
  } else if (endian::native == endian::big) {
    for (unsigned idx = 0; idx < sizeof(float); ++idx) {
      ASSERT_EQ(bytes[sizeof(float) - (idx + 1)], buffer[idx]);
    }
  } else {
    FAIL();
  }

  ASSERT_EQ(value, read_little_endian<float>(spn));
}

/// @test `write_big_endian` generates the expected value with `float` type,
/// and `read_big_endian` returns the expected value with `float` type.
TEST(Endian, BigEndianFloat) {
  arene::base::array<byte, sizeof(float)> buffer{};
  span<byte, sizeof(float)> const spn(buffer);

  auto const value = test_value<float>;
  write_big_endian<float>(value, spn);

  auto const bytes = as_bytes(span<float const, 1>(&value, 1));

  if (endian::native == endian::little) {
    for (unsigned idx = 0; idx < sizeof(float); ++idx) {
      ASSERT_EQ(bytes[sizeof(float) - (idx + 1)], buffer[idx]);
    }
  } else if (endian::native == endian::big) {
    for (unsigned idx = 0; idx < sizeof(float); ++idx) {
      ASSERT_EQ(bytes[idx], buffer[idx]);
    }
  } else {
    FAIL();
  }
  ASSERT_EQ(value, read_big_endian<float>(spn));
}

/// @test `write_little_endian` generates the expected value with `double` type,
/// and `read_little_endian` returns the expected value with `double` type.
TEST(Endian, LittleEndianDouble) {
  arene::base::array<byte, sizeof(double)> buffer{};
  span<byte, sizeof(double)> const spn(buffer);

  double const value = test_value<double>;
  write_little_endian<double>(value, spn);

  auto const bytes = as_bytes(span<double const, 1>(&value, 1));

  if (endian::native == endian::little) {
    for (unsigned idx = 0; idx < sizeof(double); ++idx) {
      ASSERT_EQ(bytes[idx], buffer[idx]);
    }
  } else if (endian::native == endian::big) {
    for (unsigned idx = 0; idx < sizeof(double); ++idx) {
      ASSERT_EQ(bytes[sizeof(double) - (idx + 1)], buffer[idx]);
    }
  } else {
    FAIL();
  }

  ASSERT_EQ(value, read_little_endian<double>(spn));
}

/// @test `write_big_endian` generates the expected value with `double` type,
/// and `read_big_endian` returns the expected value with `double` type.
TEST(Endian, BigEndianDouble) {
  arene::base::array<byte, sizeof(double)> buffer{};
  span<byte, sizeof(double)> const spn(buffer);

  double const value = test_value<double>;
  write_big_endian<double>(value, spn);

  auto const bytes = as_bytes(span<double const, 1>(&value, 1));

  if (endian::native == endian::little) {
    for (unsigned idx = 0; idx < sizeof(double); ++idx) {
      ASSERT_EQ(bytes[sizeof(double) - (idx + 1)], buffer[idx]);
    }
  } else if (endian::native == endian::big) {
    for (unsigned idx = 0; idx < sizeof(double); ++idx) {
      ASSERT_EQ(bytes[idx], buffer[idx]);
    }
  } else {
    FAIL();
  }
  ASSERT_EQ(value, read_big_endian<double>(spn));
}

ARENE_IGNORE_END();

}  // namespace

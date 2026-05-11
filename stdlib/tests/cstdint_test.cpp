// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/cstdint"

#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"

namespace {
static_assert(sizeof(std::uint8_t) == 1, "Type should be 8-bit");
static_assert(sizeof(std::uint16_t) == 2, "Type should be 16-bit");
static_assert(sizeof(std::uint32_t) == 4, "Type should be 32-bit");
// NOLINTNEXTLINE(readability-magic-numbers)
static_assert(sizeof(std::uint64_t) == 8, "Type should be 64-bit");
static_assert(
    sizeof(std::uintmax_t) == sizeof(std::uint64_t),
    "Type should be equivalent to uint64_t on our target platforms"
);

static_assert(sizeof(std::int8_t) == 1, "Type should be 8-bit");
static_assert(sizeof(std::int16_t) == 2, "Type should be 16-bit");
static_assert(sizeof(std::int32_t) == 4, "Type should be 32-bit");
// NOLINTNEXTLINE(readability-magic-numbers)
static_assert(sizeof(std::int64_t) == 8, "Type should be 64-bit");
static_assert(
    sizeof(std::intmax_t) == sizeof(std::int64_t),
    "Type should be equivalent to int64_t on our target platforms"
);

/// @test Ensure that the @c uintptr_t and @c intptr_t are integral types the same size as a pointer
TEST(IntegerTypes, IntPtr) {
  static_assert(sizeof(std::intptr_t) == sizeof(void*), "Type should be same size as a pointer");
  static_assert(sizeof(std::uintptr_t) == sizeof(void*), "Type should be same size as a pointer");
  static_assert(std::is_integral_v<std::intptr_t>, "Type should be integral");
  static_assert(std::is_integral_v<std::uintptr_t>, "Type should be integral");
  static_assert(std::is_signed_v<std::intptr_t>, "Type should be signed");
  static_assert(std::is_unsigned_v<std::uintptr_t>, "Type should be unsigned");
}

/// @test Ensure that the @c ::uint32_t is the same as @c std::uint32_t with the correct range
TEST(IntegerTypes, Uint32T) {
  ::testing::StaticAssertTypeEq<::uint32_t, std::uint32_t>();
  static_assert(static_cast<std::uint32_t>(~static_cast<std::uint32_t>(0)) == 4'294'967'295U, "Correct max");
  static_assert(
      static_cast<std::uint32_t>(static_cast<std::uint32_t>(0) - static_cast<std::uint32_t>(1)) > 0,
      "Must be unsigned"
  );
}

/// @test Ensure that the @c ::uint8_t is the same as @c std::uint8_t with the correct range
TEST(IntegerTypes, Uint8T) {
  ::testing::StaticAssertTypeEq<::uint8_t, std::uint8_t>();
  static_assert(static_cast<std::uint8_t>(~static_cast<std::uint8_t>(0)) == 255U, "Correct max");
  static_assert(
      static_cast<std::uint8_t>(static_cast<std::uint8_t>(0) - static_cast<std::uint8_t>(1)) > 0,
      "Must be unsigned"
  );
}

/// @test Ensure that the @c ::uint16_t is the same as @c std::uint16_t with the correct range
TEST(IntegerTypes, Uint16T) {
  ::testing::StaticAssertTypeEq<::uint16_t, std::uint16_t>();
  static_assert(static_cast<std::uint16_t>(~static_cast<std::uint16_t>(0)) == 65535U, "Correct max");
  static_assert(
      static_cast<std::uint16_t>(static_cast<std::uint16_t>(0) - static_cast<std::uint16_t>(1)) > 0,
      "Must be unsigned"
  );
}

/// @test Ensure that the @c ::uint64_t is the same as @c std::uint64_t with the correct range
TEST(IntegerTypes, Uint64T) {
  ::testing::StaticAssertTypeEq<::uint64_t, std::uint64_t>();
  static_assert(
      static_cast<std::uint64_t>(~static_cast<std::uint64_t>(0)) == 18'446'744'073'709'551'615ULL,
      "Correct max"
  );
  static_assert(
      static_cast<std::uint64_t>(static_cast<std::uint64_t>(0) - static_cast<std::uint64_t>(1)) > 0,
      "Must be unsigned"
  );
}

/// @test Ensure that the @c ::int32_t is the same as @c std::int32_t with the correct range
TEST(IntegerTypes, Int32T) {
  ::testing::StaticAssertTypeEq<::int32_t, std::int32_t>();
  // Assume 2's complement: max negative value has same representation as unsigned 2^31
  static_assert(
      static_cast<std::int32_t>(static_cast<std::uint32_t>(static_cast<std::uint32_t>(1) << 31U)) == -2'147'483'648,
      "Correct min"
  );
  // Assume 2's complement: max positive signed value is half the max positive (all bits set) unsigned value
  static_assert(
      static_cast<std::int32_t>(static_cast<std::uint32_t>(~static_cast<std::uint32_t>(0)) / 2) == 2'147'483'647,
      "Correct max"
  );
  static_assert(
      static_cast<std::int32_t>(static_cast<std::int32_t>(0) - static_cast<std::int32_t>(1)) < 0,
      "Must be signed"
  );
}

/// @test Ensure that the @c ::int8_t is the same as @c std::int8_t with the correct range
TEST(IntegerTypes, Int8T) {
  ::testing::StaticAssertTypeEq<::int8_t, std::int8_t>();
  // Assume 2's complement: max negative value has same representation as unsigned 2^7
  static_assert(
      static_cast<std::int8_t>(static_cast<std::uint8_t>(static_cast<std::uint8_t>(1) << 7U)) == -128,
      "Correct min"
  );
  // Assume 2's complement: max positive signed value is half the max positive (all bits set) unsigned value
  static_assert(
      static_cast<std::int8_t>(static_cast<std::uint8_t>(~static_cast<std::uint8_t>(0)) / 2) == 127,
      "Correct max"
  );
  static_assert(
      static_cast<std::int8_t>(static_cast<std::int8_t>(0) - static_cast<std::int8_t>(1)) < 0,
      "Must be signed"
  );
}

/// @test Ensure that the @c ::int16_t is the same as @c std::int16_t with the correct range
TEST(IntegerTypes, Int16T) {
  ::testing::StaticAssertTypeEq<::int16_t, std::int16_t>();
  // Assume 2's complement: max negative value has same representation as unsigned 2^15
  static_assert(
      static_cast<std::int16_t>(static_cast<std::uint16_t>(static_cast<std::uint16_t>(1) << 15U)) == -32768,
      "Correct min"
  );
  // Assume 2's complement: max positive signed value is half the max positive (all bits set) unsigned value
  static_assert(
      static_cast<std::int16_t>(static_cast<std::uint16_t>(~static_cast<std::uint16_t>(0)) / 2) == 32767,
      "Correct max"
  );
  static_assert(
      static_cast<std::int16_t>(static_cast<std::int16_t>(0) - static_cast<std::int16_t>(1)) < 0,
      "Must be signed"
  );
}

/// @test Ensure that the @c ::int64_t is the same as @c std::int64_t with the correct range
TEST(IntegerTypes, Int64T) {
  ::testing::StaticAssertTypeEq<::int64_t, std::int64_t>();
  // Assume 2's complement: max negative value has same representation as unsigned 2^63
  // You cannot actually represent that as a numeric literal since numeric literals are always positive, and so we have
  // to take "-(max positive literal) -1" for long long, which is the only type guaranteed to have 64 bits.
  static_assert(
      // NOLINTNEXTLINE(google-runtime-int)
      static_cast<long long>(static_cast<std::int64_t>(static_cast<std::uint64_t>(static_cast<std::uint64_t>(1) << 63U))
      ) == (-9'223'372'036'854'775'807LL - 1LL),
      "Correct min"
  );
  // Assume 2's complement: max positive signed value is half the max positive (all bits set) unsigned value
  static_assert(
      // NOLINTNEXTLINE(google-runtime-int)
      static_cast<long long>(static_cast<std::int64_t>(static_cast<std::uint64_t>(~static_cast<std::uint64_t>(0)) / 2)
      ) == 9'223'372'036'854'775'807LL,
      "Correct max"
  );
  static_assert(
      static_cast<std::int64_t>(static_cast<std::int64_t>(0) - static_cast<std::int64_t>(1)) < 0,
      "Must be signed"
  );
}

}  // namespace

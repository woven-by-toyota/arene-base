// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_manipulation/smallest_integer_for.hpp"

#include <gtest/gtest.h>

namespace {

using ::arene::base::smallest_signed_integer_for;
using ::arene::base::smallest_unsigned_integer_for;

/// @test `smallest_unsigned_integer_for` is the smallest unsigned integral type that can hold the specified value.
TEST(SmallestUnsignedIntegerFor, ReturnsSmallestBitwidthUnsignedIntegerTypeForValue) {
  ::testing::StaticAssertTypeEq<smallest_unsigned_integer_for<0>, std::uint8_t>();
  ::testing::StaticAssertTypeEq<smallest_unsigned_integer_for<1>, std::uint8_t>();
  ::testing::StaticAssertTypeEq<smallest_unsigned_integer_for<255>, std::uint8_t>();
  ::testing::StaticAssertTypeEq<smallest_unsigned_integer_for<256>, std::uint16_t>();
  ::testing::StaticAssertTypeEq<smallest_unsigned_integer_for<16384>, std::uint16_t>();
  ::testing::StaticAssertTypeEq<smallest_unsigned_integer_for<65535>, std::uint16_t>();
  ::testing::StaticAssertTypeEq<smallest_unsigned_integer_for<65536>, std::uint32_t>();
  ::testing::StaticAssertTypeEq<
      arene::base::smallest_unsigned_integer_for<std::numeric_limits<std::uint32_t>::max() / 2>,
      std::uint32_t>();
  ::testing::StaticAssertTypeEq<
      arene::base::smallest_unsigned_integer_for<std::numeric_limits<std::uint32_t>::max() - 1>,
      std::uint32_t>();
  ::testing::
      StaticAssertTypeEq<smallest_unsigned_integer_for<std::numeric_limits<std::uint32_t>::max()>, std::uint32_t>();
  ::testing::StaticAssertTypeEq<
      smallest_unsigned_integer_for<static_cast<std::uint64_t>(std::numeric_limits<std::uint32_t>::max()) + 1>,
      std::uint64_t>();
  ::testing::StaticAssertTypeEq<
      arene::base::smallest_unsigned_integer_for<std::numeric_limits<std::uint64_t>::max() / 2>,
      std::uint64_t>();
  ::testing::
      StaticAssertTypeEq<smallest_unsigned_integer_for<std::numeric_limits<std::uint64_t>::max()>, std::uint64_t>();
}

/// @test `smallest_signed_integer_for` is the smallest signed integral type that can hold the specified range of
/// values.
TEST(SmallestSignedIntegerFor, ReturnsSmallestBitwidthSignedIntegerForValue) {
  ::testing::StaticAssertTypeEq<smallest_signed_integer_for<0, 0>, std::int8_t>();
  ::testing::StaticAssertTypeEq<smallest_signed_integer_for<0, 1>, std::int8_t>();
  ::testing::StaticAssertTypeEq<smallest_signed_integer_for<0, 127>, std::int8_t>();
  ::testing::StaticAssertTypeEq<smallest_signed_integer_for<-100, 103>, std::int8_t>();
  ::testing::StaticAssertTypeEq<smallest_signed_integer_for<-128, 0>, std::int8_t>();
  ::testing::StaticAssertTypeEq<smallest_signed_integer_for<-129, 0>, std::int16_t>();
  ::testing::StaticAssertTypeEq<smallest_signed_integer_for<-130, 100>, std::int16_t>();
  ::testing::StaticAssertTypeEq<smallest_signed_integer_for<0, 255>, std::int16_t>();
  ::testing::StaticAssertTypeEq<smallest_signed_integer_for<0, 256>, std::int16_t>();
  ::testing::StaticAssertTypeEq<smallest_signed_integer_for<-45, 2000>, std::int16_t>();
  ::testing::StaticAssertTypeEq<smallest_signed_integer_for<-300, 1234>, std::int16_t>();
  ::testing::StaticAssertTypeEq<smallest_signed_integer_for<0, 16384>, std::int16_t>();
  ::testing::StaticAssertTypeEq<smallest_signed_integer_for<0, 32767>, std::int16_t>();
  ::testing::StaticAssertTypeEq<smallest_signed_integer_for<0, 32768>, std::int32_t>();
  ::testing::StaticAssertTypeEq<smallest_signed_integer_for<-32767, 0>, std::int16_t>();
  ::testing::StaticAssertTypeEq<smallest_signed_integer_for<-32768, 0>, std::int16_t>();
  ::testing::StaticAssertTypeEq<smallest_signed_integer_for<-32769, 0>, std::int32_t>();
  ::testing::StaticAssertTypeEq<
      arene::base::smallest_signed_integer_for<0, std::numeric_limits<std::int32_t>::max() / 2>,
      std::int32_t>();
  ::testing::StaticAssertTypeEq<
      arene::base::smallest_signed_integer_for<std::numeric_limits<std::int32_t>::min() / 2, 0>,
      std::int32_t>();
  ::testing::StaticAssertTypeEq<smallest_signed_integer_for<std::numeric_limits<std::int32_t>::min(), 0>, std::int32_t>(
  );
  ::testing::StaticAssertTypeEq<
      arene::base::smallest_signed_integer_for<0, std::numeric_limits<std::int32_t>::max() - 1>,
      std::int32_t>();
  ::testing::StaticAssertTypeEq<smallest_signed_integer_for<0, std::numeric_limits<std::int32_t>::max()>, std::int32_t>(
  );
  ::testing::StaticAssertTypeEq<
      smallest_signed_integer_for<0, static_cast<std::int64_t>(std::numeric_limits<std::int32_t>::max()) + 1>,
      std::int64_t>();
  ::testing::StaticAssertTypeEq<
      smallest_signed_integer_for<static_cast<std::int64_t>(std::numeric_limits<std::int32_t>::min()) - 1, 0>,
      std::int64_t>();
  ::testing::StaticAssertTypeEq<
      arene::base::smallest_signed_integer_for<0, std::numeric_limits<std::int64_t>::max() / 2>,
      std::int64_t>();
  ::testing::StaticAssertTypeEq<smallest_signed_integer_for<0, std::numeric_limits<std::int64_t>::max()>, std::int64_t>(
  );
  ::testing::StaticAssertTypeEq<
      smallest_signed_integer_for<
          std::numeric_limits<std::int64_t>::min(),
          std::numeric_limits<std::int64_t>::max() / 2>,
      std::int64_t>();
  ::testing::StaticAssertTypeEq<
      smallest_signed_integer_for<std::numeric_limits<std::int64_t>::min(), std::numeric_limits<std::int64_t>::max()>,
      std::int64_t>();
}

}  // namespace

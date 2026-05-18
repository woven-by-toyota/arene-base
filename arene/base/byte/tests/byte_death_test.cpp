// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/byte/byte.hpp"

namespace {

using ::arene::base::literals::operator""_byte;

constexpr auto starting_bytes = ::arene::base::to_array({0_byte, 1_byte, 0b0000'1111_byte, 0b1111'1111_byte});

/// @test The `byte` left shift operator crashes with a precondition violation if the shift is 8 or more bits.
TEST(ByteDeathTest, LeftShiftOf8OrMoreCrashes) {
  for (arene::base::byte const& starting_byte : starting_bytes) {
    ASSERT_DEATH(starting_byte << 8U, "Precondition");
    ASSERT_DEATH(starting_byte << 128U, "Precondition");
    ASSERT_DEATH(starting_byte << 5'000U, "Precondition");
  }
}

/// @test The `byte` right shift operator crashes with a precondition violation if the shift is 8 or more bits.
TEST(ByteDeathTest, RightShiftOf8OrMoreCrashes) {
  for (arene::base::byte const& starting_byte : starting_bytes) {
    ASSERT_DEATH(starting_byte >> 8U, "Precondition");
    ASSERT_DEATH(starting_byte >> 128U, "Precondition");
    ASSERT_DEATH(starting_byte >> 5'000U, "Precondition");
  }
}

}  // namespace

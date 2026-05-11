// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/endian/detail/byte_swap32_raw.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {
using ::arene::base::byte_swap_detail::byte_swap_raw;

using uint_t = std::uint32_t;

/// @test Validate that bytes are reversed
TEST(ByteSwap, Uint32Constexpr) {
  STATIC_ASSERT_EQ(byte_swap_raw(static_cast<uint_t>(0x01'02'03'04U)), static_cast<uint_t>(0x04'03'02'01U));
  STATIC_ASSERT_EQ(byte_swap_raw(static_cast<uint_t>(0x23'45'67'89U)), static_cast<uint_t>(0x89'67'45'23U));
}

}  // namespace

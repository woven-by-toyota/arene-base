// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/endian/detail/byte_swap16.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {
using ::arene::base::byte_swap_detail::byte_swap;

using uint_t = std::uint16_t;

TEST(ByteSwapRaw, Uint16Constexpr) {
  STATIC_ASSERT_EQ(byte_swap(static_cast<uint_t>(0x0102)), static_cast<uint_t>(0x0201));
  STATIC_ASSERT_EQ(byte_swap(static_cast<uint_t>(0x5678)), static_cast<uint_t>(0x7856));
}

}  // namespace

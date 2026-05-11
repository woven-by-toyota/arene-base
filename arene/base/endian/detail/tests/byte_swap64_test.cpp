// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/endian/detail/byte_swap64.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {
using ::arene::base::byte_swap_detail::byte_swap;

using uint_t = std::uint64_t;

TEST(ByteSwap, Uint64Constexpr) {
  STATIC_ASSERT_EQ(byte_swap(static_cast<uint_t>(0x0102030405060708ULL)), static_cast<uint_t>(0x0807060504030201ULL));
  STATIC_ASSERT_EQ(byte_swap(static_cast<uint_t>(0x23456789'12345678ULL)), static_cast<uint_t>(0x78563412'89674523ULL));
}

}  // namespace

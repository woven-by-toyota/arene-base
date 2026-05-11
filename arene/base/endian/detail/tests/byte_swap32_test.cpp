// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/endian/detail/byte_swap32.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {
using ::arene::base::byte_swap_detail::byte_swap;

using uint_t = std::uint32_t;

TEST(ByteSwap, Uint32Constexpr) {
  STATIC_ASSERT_EQ(byte_swap(static_cast<uint_t>(0x01020304U)), static_cast<uint_t>(0x04030201U));
  STATIC_ASSERT_EQ(byte_swap(static_cast<uint_t>(0x23456789U)), static_cast<uint_t>(0x89674523U));
}

}  // namespace

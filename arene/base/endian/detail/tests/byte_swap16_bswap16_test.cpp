// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/endian/detail/byte_swap16_bswap16.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/endian/detail/tests/indirect_byte_swap.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {
using ::arene::base::byte_swap_detail::byte_swap;

using uint_t = std::uint16_t;

/// @test If the compiler provides `__builtin_bswap16`, `byte_swap` produces the correct answer for 16-bit integers
TEST(ByteSwapRaw, Uint16Constexpr) {
#if ARENE_HAS_BUILTIN(__builtin_bswap16)
  STATIC_ASSERT_EQ(byte_swap(static_cast<uint_t>(0x0102)), static_cast<uint_t>(0x0201));
  STATIC_ASSERT_EQ(byte_swap(static_cast<uint_t>(0x5678)), static_cast<uint_t>(0x7856));
#else
  GTEST_SKIP() << "Not relevant if __builtin_bswap16 is not defined";
#endif
}

/// @test If the compiler provides `__builtin_bswap16`, `byte_swap` produces the correct answer for 16-bit integers in
/// non-`constexpr` contexts
TEST(ByteSwapRaw, Uint16NonConstexpr) {
#if ARENE_HAS_BUILTIN(__builtin_bswap16)
  // Indirection to force non-constexpr evaluation
  EXPECT_EQ(arene::base::testing::indirect_byte_swap(static_cast<uint_t>(0x0102)), static_cast<uint_t>(0x0201));
  EXPECT_EQ(arene::base::testing::indirect_byte_swap(static_cast<uint_t>(0x5678)), static_cast<uint_t>(0x7856));
#else
  GTEST_SKIP() << "Not relevant if __builtin_bswap16 is not defined";
#endif
}

}  // namespace

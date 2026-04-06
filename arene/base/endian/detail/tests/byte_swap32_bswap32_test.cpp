// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/endian/detail/byte_swap32_bswap32.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/endian/detail/tests/indirect_byte_swap.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {
using ::arene::base::byte_swap_detail::byte_swap;

using uint_t = std::uint32_t;

/// @test If the compiler provides `__builtin_bswap32`, `byte_swap` produces the correct answer for 32-bit integers
TEST(ByteSwap, Uint32Constexpr) {
#if ARENE_HAS_BUILTIN(__builtin_bswap32)
  STATIC_ASSERT_EQ(byte_swap(static_cast<uint_t>(0x01020304U)), static_cast<uint_t>(0x04030201U));
  STATIC_ASSERT_EQ(byte_swap(static_cast<uint_t>(0x23456789U)), static_cast<uint_t>(0x89674523U));
#else
  GTEST_SKIP() << "Not relevant if __builtin_bswap32 is not defined";
#endif
}

/// @test If the compiler provides `__builtin_bswap32`, `byte_swap` produces the correct answer for 32-bit integers in
/// non-`constexpr` contexts
TEST(ByteSwap, Uint32NonConstexpr) {
#if ARENE_HAS_BUILTIN(__builtin_bswap32)
  // Indirection to force non-constexpr evaluation
  EXPECT_EQ(
      arene::base::testing::indirect_byte_swap(static_cast<uint_t>(0x01020304U)),
      static_cast<uint_t>(0x04030201U)
  );
  EXPECT_EQ(
      arene::base::testing::indirect_byte_swap(static_cast<uint_t>(0x23456789U)),
      static_cast<uint_t>(0x89674523U)
  );
#else
  GTEST_SKIP() << "Not relevant if __builtin_bswap32 is not defined";
#endif
}

}  // namespace

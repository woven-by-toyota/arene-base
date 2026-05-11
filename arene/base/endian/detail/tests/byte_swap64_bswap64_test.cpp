// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/endian/detail/byte_swap64_bswap64.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/endian/detail/tests/indirect_byte_swap.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {
using ::arene::base::byte_swap_detail::byte_swap;

using uint_t = std::uint64_t;

/// @test If the compiler provides `__builtin_bswap64`, `byte_swap` produces the correct answer for 64-bit integers
TEST(ByteSwap, Uint64Constexpr) {
#if ARENE_HAS_BUILTIN(__builtin_bswap64)
  STATIC_ASSERT_EQ(byte_swap(static_cast<uint_t>(0x0102030405060708ULL)), static_cast<uint_t>(0x0807060504030201ULL));
  STATIC_ASSERT_EQ(byte_swap(static_cast<uint_t>(0x23456789'12345678ULL)), static_cast<uint_t>(0x78563412'89674523ULL));
#else
  GTEST_SKIP() << "Not relevant if __builtin_bswap64 is not defined";
#endif
}

/// @test If the compiler provides `__builtin_bswap64`, `byte_swap` produces the correct answer for 64-bit integers in
/// non-`constexpr` contexts
TEST(ByteSwap, Uint64NonConstexpr) {
#if ARENE_HAS_BUILTIN(__builtin_bswap64)
  // Indirection to force non-constexpr evaluation
  EXPECT_EQ(
      arene::base::testing::indirect_byte_swap(static_cast<uint_t>(0x0102030405060708ULL)),
      static_cast<uint_t>(0x0807060504030201ULL)
  );
  EXPECT_EQ(
      arene::base::testing::indirect_byte_swap(static_cast<uint_t>(0x23456789'12345678ULL)),
      static_cast<uint_t>(0x78563412'89674523ULL)
  );
#else
  GTEST_SKIP() << "Not relevant if __builtin_bswap64 is not defined";
#endif
}

}  // namespace

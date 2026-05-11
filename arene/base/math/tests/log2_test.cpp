// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/math/log2.hpp"

#include <climits>

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {
/// @test `log2` returns the expected value for arguments of 2^n.
TEST(Log2, LogOfPowersOf2) {
  for (unsigned i = 0; i < (sizeof(std::size_t) * CHAR_BIT); ++i) {
    ASSERT_EQ(arene::base::log2(static_cast<std::size_t>(1) << i), i);
  }
}

/// @test `log2` returns the expected value for arguments of 2^n + 1.
TEST(Log2, LogOfPowersOf2Plus1) {
  for (unsigned i = 1; i < (sizeof(std::size_t) * CHAR_BIT); ++i) {
    ASSERT_EQ(arene::base::log2(1 + (static_cast<std::size_t>(1) << i)), i);
  }
}

/// @test `log2` returns the expected value, with `constexpr`.
TEST(Log2, ConstexprLog2) {
  STATIC_ASSERT_EQ(arene::base::log2(1024), 10);
  STATIC_ASSERT_EQ(arene::base::log2(std::numeric_limits<std::size_t>::max()), (sizeof(std::size_t) * CHAR_BIT - 1));
}

}  // namespace

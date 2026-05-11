// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

namespace {

/// @test Simple test that uses the C++17 language feature "if constexpr"
/// This won't compile if C++17 not enabled, so verifies that C++17
/// selection is working
TEST(Cpp17, Cpp17Usable) {
  // NOLINTNEXTLINE(readability-simplify-boolean-expr)
  if constexpr (true) {
    ASSERT_TRUE(true);
  }
}

}  // namespace

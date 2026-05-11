// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file hardware_queries_test.cpp
/// @brief Unit tests for hardware_queries.hpp
///
/// These unit tests simply validate the expected macros are exported from the expected location. It's not possible to
/// do better without the ability to test if code compiles, which is non-trivial in pre C++20 contexts.
///

#include "arene/base/compiler_support/hardware_queries.hpp"

#include <gtest/gtest.h>

namespace {
/// @test Ensure that the `hardware_destructive_interference_size` is set to a value greater than zero
TEST(HardwareQueries, HardwareDestructiveInterferenceSizeIsExported) {
  EXPECT_GT(arene::base::hardware_destructive_interference_size, 0);
}

}  // namespace

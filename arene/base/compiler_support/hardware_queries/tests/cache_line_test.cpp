// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file cache_line_test.cpp
/// @brief Unit tests for cache_line.hpp
///

#include "arene/base/compiler_support/hardware_queries/cache_line.hpp"

#include <gtest/gtest.h>

#include "arene/base/testing/gtest.hpp"

namespace {
/// @test Ensure that the `hardware_destructive_interference_size` is set to a value greater than zero
TEST(CacheLine, IsDefined) {
  // Since we can't practically test across the implementations in a unit test, we simply validate it is defined and a
  // non-zero number.
  STATIC_ASSERT_GT(arene::base::hardware_destructive_interference_size, 0U);
}

}  // namespace

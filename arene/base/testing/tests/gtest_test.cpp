// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file gtest_test.cpp
/// @brief Validates that expected symbols are exported from gtest.hpp
///
///

#include "arene/base/testing/gtest.hpp"

#include <gtest/gtest.h>

namespace {

/// @test Static assertions are correctly imported from the GoogleTest framework.
TEST(Gtest, StaticAssertionsExported) {
  STATIC_ASSERT_EQ(10, 10);
  STATIC_ASSERT_NE(10, 11);
  STATIC_ASSERT_LT(10, 11);
  STATIC_ASSERT_LE(10, 11);
  STATIC_ASSERT_GT(11, 10);
  STATIC_ASSERT_GE(11, 10);
  STATIC_ASSERT_TRUE(true);
  STATIC_ASSERT_FALSE(false);
  STATIC_ASSERT_NEAR(0.0, 0.0, 1.0);
  STATIC_ASSERT_STREQ("a", "a");
  STATIC_ASSERT_STRNE("a", "b");
}

}  // namespace

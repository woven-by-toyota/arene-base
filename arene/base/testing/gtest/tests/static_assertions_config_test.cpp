// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file static_assertions_config_test.cpp
/// @brief Test for when the config has been not explicitly altered.
///
#include "arene/base/testing/gtest/static_assertions_config.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/preprocessor.hpp"

namespace {

/// @test Ensure that `ARENE_GTEST_STATIC_ASSERTIONS` is a valid configuration query
TEST(AreneGtestStaticAssertions, IsValidConfig) {
  static_assert(
      ARENE_GUARANTEE_INTERNAL_DEFINITION(ARENE_GTEST_STATIC_ASSERTIONS),
      "ARENE_GTEST_STATIC_ASSERTIONS should be a config still."
  );
}

/// @test Ensure that `ARENE_GTEST_STATIC_ASSERTIONS` is "on"
TEST(AreneGtestStaticAssertions, IsOnByDefault) {
  static_assert(ARENE_IS_ON(ARENE_GTEST_STATIC_ASSERTIONS), "Should be ARENE_IS_ON in normal building");
}

}  // namespace

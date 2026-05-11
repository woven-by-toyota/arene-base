// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/platform_queries/has_thread_safety_analysis.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/platform_queries/compiler_info.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

/// @test Ensure `ARENE_HAS_THREAD_SAFETY_ANALYSIS` property check is defined
TEST(AreneHasThreadSafetyAnalysis, IsAValidConfigFlag) {
  STATIC_ASSERT_TRUE(ARENE_GUARANTEE_INTERNAL_DEFINITION(ARENE_HAS_THREAD_SAFETY_ANALYSIS));
}

/// @test Thread Safety Analysis is reported as available when the compiler is clang
TEST(AreneHasThreadSafetyAnalysis, IsOnForClang) {
  if (ARENE_IS_ON(ARENE_COMPILER_CLANG)) {
    ASSERT_TRUE(ARENE_IS_ON(ARENE_HAS_THREAD_SAFETY_ANALYSIS));
  }
}

}  // namespace

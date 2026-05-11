// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/platform_queries/coverage_info.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

/// @test Ensure `ARENE_IS_GCOV` property check is defined
TEST(AreneIsGcov, IsProperlyDefinedConfig) { STATIC_ASSERT_TRUE(ARENE_GUARANTEE_INTERNAL_DEFINITION(ARENE_IS_GCOV)); }

/// @test Ensure `ARENE_IS_LLVMCOV` property check is defined
TEST(AreneIsLLVMCov, IsProperlyDefinedConfig) {
  STATIC_ASSERT_TRUE(ARENE_GUARANTEE_INTERNAL_DEFINITION(ARENE_IS_LLVMCOV));
}

/// @test Ensure `ARENE_IS_COVERAGE_COMPILATION` property check is defined
TEST(AreneIsCoverageCompilation, IsProperlyDefinedConfig) {
  STATIC_ASSERT_TRUE(ARENE_GUARANTEE_INTERNAL_DEFINITION(ARENE_IS_COVERAGE_COMPILATION));
}

}  // namespace

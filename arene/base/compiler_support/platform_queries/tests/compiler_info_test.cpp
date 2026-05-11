// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/platform_queries/compiler_info.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

/// @test Ensure `ARENE_COMPILER_CLANG` property check is defined
TEST(AreneCompilerClang, IsProperlyDefinedConfig) {
  STATIC_ASSERT_TRUE(ARENE_GUARANTEE_INTERNAL_DEFINITION(ARENE_COMPILER_CLANG));
}

/// @test Ensure `ARENE_COMPILER_GCC` property check is defined
TEST(AreneCompilerGcc, IsProperlyDefinedConfig) {
  STATIC_ASSERT_TRUE(ARENE_GUARANTEE_INTERNAL_DEFINITION(ARENE_COMPILER_GCC));
}

}  // namespace

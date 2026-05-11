// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/platform_queries/stdlib_info.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

/// @test Ensure `ARENE_STDLIB_LIBCXX` property check is defined
TEST(AreneStdlibCxx, IsProperlyDefinedConfig) {
  STATIC_ASSERT_TRUE(ARENE_GUARANTEE_INTERNAL_DEFINITION(ARENE_STDLIB_LIBCXX));
}

/// @test Ensure `ARENE_STDLIB_LIBSTDCXX` property check is defined
TEST(AreneStdlibStdCxx, IsProperlyDefinedConfig) {
  STATIC_ASSERT_TRUE(ARENE_GUARANTEE_INTERNAL_DEFINITION(ARENE_STDLIB_LIBSTDCXX));
}

/// @test Ensure `ARENE_STDLIB_LIBARENECXX` property check is defined
TEST(AreneStdlibAreneCxx, IsProperlyDefinedConfig) {
  STATIC_ASSERT_TRUE(ARENE_GUARANTEE_INTERNAL_DEFINITION(ARENE_STDLIB_LIBARENECXX));
}

}  // namespace

// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/platform_queries/has_uncaught_exceptions.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

/// @test Ensure `ARENE_STD_LIBRARY_UNCAUGHT_EXCEPTIONS` property check is defined
TEST(AreneStdLibraryUncaughtExceptions, IsProperlyDefinedConfig) {
  STATIC_ASSERT_TRUE(ARENE_GUARANTEE_INTERNAL_DEFINITION(ARENE_STD_LIBRARY_UNCAUGHT_EXCEPTIONS));
}

}  // namespace

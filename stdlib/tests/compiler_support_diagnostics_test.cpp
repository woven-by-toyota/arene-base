// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/diagnostics.hpp"
#include "testlibs/minitest/minitest.hpp"

namespace {

/// @test Ensure @c ARENE_IGNORE_* macros can be used in stdlib and stdlib tests
TEST(CompilerSupportDiagnostics, Ignore) {
  ARENE_IGNORE_START();

  ARENE_IGNORE_ARMCLANG("-Wc++11-narrowing", "This is an explicit test for warning suppression.");
  ARENE_IGNORE_CLANG("-Wc++11-narrowing", "This is an explicit test for warning suppression.");
  ARENE_IGNORE_GCC("-Wnarrowing", "This is an explicit test for warning suppression.");
  ASSERT_TRUE(bool{2});

  ARENE_IGNORE_END();
}

}  // namespace

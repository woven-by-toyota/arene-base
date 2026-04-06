// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/compiler_support/preprocessor/require_semicolon.hpp"

namespace {

#define USE_ARENE_REQUIRE_SEMICOLON \
  struct udt {};                    \
  ARENE_REQUIRE_SEMICOLON

/// @test `ARENE_REQUIRE_SEMICOLON` does not compile if a semicolon is not present.
TEST(AreneRequireSemicolonTest, DoesNotCompileIfSemicolonIsNotPresent) { USE_ARENE_REQUIRE_SEMICOLON }

}  // namespace

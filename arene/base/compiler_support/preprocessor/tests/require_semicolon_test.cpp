// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/preprocessor/require_semicolon.hpp"

#include <gtest/gtest.h>

namespace {

/// @test `ARENE_REQUIRE_SEMICOLON` compiles successfully if a semicolon is present.
TEST(AreneRequireSemicolonTest, CompilesIfSemicolonIsPresent) { ARENE_REQUIRE_SEMICOLON; }

#define USE_ARENE_REQUIRE_SEMICOLON \
  struct udt {};                    \
  ARENE_REQUIRE_SEMICOLON

/// @test Given a macro which ends in `ARENE_REQUIRE_SEMICOLON`, invoking the macro compiles successfully if the
/// semicolon is present.
TEST(AreneRequireSemicolonTest, CanBeUsedInsideAnotherMacroToRequireASemicolon) { USE_ARENE_REQUIRE_SEMICOLON; }

}  // namespace

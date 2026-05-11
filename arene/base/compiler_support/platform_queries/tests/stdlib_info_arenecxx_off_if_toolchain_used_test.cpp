// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/compiler_support/platform_queries/stdlib_info.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

/// @test Given a toolchain provided stdlib is in use, then `ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)` is `true`.
TEST(AreneStdlibAreneCxx, IsProperlyDefinedConfig) { STATIC_ASSERT_TRUE(ARENE_TEST_MACRO(ARENE_STDLIB_LIBARENECXX)); }

}  // namespace

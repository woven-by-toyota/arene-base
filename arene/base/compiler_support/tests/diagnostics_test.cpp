// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file diagnostics_test.cpp
/// @brief Unit tests for diagnostics.hpp
///
/// These unit tests simply validate the expected macros are exported from the expected location. It's not possible to
/// do better without the ability to test if code compiles, which is non-trivial in pre C++20 contexts.
///

#include "arene/base/compiler_support/diagnostics.hpp"

#include <gtest/gtest.h>

namespace {
/// @test Ensure that `ARENE_IGNORE` is defined
TEST(Diagnostics, AreneIgnore) {
#ifndef ARENE_IGNORE
  FAIL() << "Expected ARENE_IGNORE to be defined";
#endif
}

/// @test Ensure that `ARENE_IGNORE_CLANG` is defined
TEST(Diagnostics, AreneIgnoreClang) {
#ifndef ARENE_IGNORE_CLANG
  FAIL() << "Expected ARENE_IGNORE_CLANG to be defined";
#endif
}

/// @test Ensure that `ARENE_IGNORE_ALL` is defined
TEST(Diagnostics, AreneIgnoreAll) {
#ifndef ARENE_IGNORE_ALL
  FAIL() << "Expected ARENE_IGNORE_ALL to be defined";
#endif
}

/// @test Ensure that `ARENE_IGNORE_GCC` is defined
TEST(Diagnostics, AreneIgnoreGcc) {
#ifndef ARENE_IGNORE_GCC
  FAIL() << "Expected ARENE_IGNORE_GCC to be defined";
#endif
}

/// @test Ensure that `ARENE_IGNORE_START` is defined
TEST(Diagnostics, AreneIgnoreStart) {
#ifndef ARENE_IGNORE_START
  FAIL() << "Expected ARENE_IGNORE_START to be defined";
#endif
}

/// @test Ensure that `ARENE_IGNORE_END` is defined
TEST(Diagnostics, AreneIgnoreEnd) {
#ifndef ARENE_IGNORE_END
  FAIL() << "Expected ARENE_IGNORE_END to be defined";
#endif
}

}  // namespace

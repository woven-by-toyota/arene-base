// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file attributes_test.cpp
/// @brief Unit tests for attributes.hpp
///
/// These unit tests simply validate the expected macros are exported from the expected location. It's not possible to
/// do better without the ability to test if code compiles, which is non-trivial in pre C++20 contexts.
///

#include "arene/base/compiler_support/attributes.hpp"

#include <gtest/gtest.h>

namespace {
/// @test Ensure that `ARENE_NODISCARD` is defined
TEST(Attributes, AreneNodiscard) {
#ifndef ARENE_NODISCARD
  FAIL() << "Expected ARENE_NODISCARD to be defined";
#endif
}

/// @test Ensure that `ARENE_NODISCARD_WITH` is defined
TEST(Attributes, AreneNodiscardWith) {
#ifndef ARENE_NODISCARD_WITH
  FAIL() << "Expected ARENE_NODISCARD_WITH to be defined";
#endif
}

/// @test Ensure that `ARENE_MAYBE_UNUSED` is defined
TEST(Attributes, AreneMaybeUnused) {
#ifndef ARENE_MAYBE_UNUSED
  FAIL() << "Expected ARENE_MAYBE_UNUSED to be defined";
#endif
}

/// @test Ensure that `ARENE_NORETURN` is defined
TEST(Attributes, AreneNoReturn) {
#ifndef ARENE_NORETURN
  FAIL() << "Expected ARENE_NORETURN to be defined";
#endif
}

/// @test Ensure that `ARENE_DEPRECATED` is defined
TEST(Attributes, AreneDeprecated) {
#ifndef ARENE_DEPRECATED
  FAIL() << "Expected ARENE_DEPRECATED to be defined";
#endif
}

/// @test Ensure that `ARENE_DEPRECATED_WITH` is defined
TEST(Attributes, AreneDeprecatedWith) {
#ifndef ARENE_DEPRECATED_WITH
  FAIL() << "Expected ARENE_DEPRECATED_WITH to be defined";
#endif
}

}  // namespace

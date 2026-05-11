// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file stringize_test.cpp
/// @brief Unit tests for stringize.hpp
///
/// These unit tests simply validate the expected macros are exported from the expected location. It's not possible to
/// do better without the ability to test if code compiles, which is non-trivial in pre C++20 contexts.
///

#include <gtest/gtest.h>

#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

// NOLINTBEGIN(cppcoreguidelines-macro-usage) testing usage of macro intended for making macros

#define STRINGIZE_TEST_CAT(a, b) a##b

/// @test The result of applying `ARENE_STRINGIZE` to a token-pasting expression is the string form of the pasted token.
TEST(Stringize, ReturnsStringizedVersionOfArgument) {
  constexpr auto value = ARENE_STRINGIZE(STRINGIZE_TEST_CAT(foo, bar));
  STATIC_ASSERT_STREQ(value, "foobar");
}

/// @test The result of applying `ARENE_STRINGIZE` to a list of comma-separated values is a single string holding the
/// comma separated list of values
TEST(Stringize, ReturnsStringizedVersionOfVariadicArguments) {
  constexpr auto value = ARENE_STRINGIZE(foo, bar, baz);
  STATIC_ASSERT_STREQ(value, "foo, bar, baz");
}

// NOLINTEND(cppcoreguidelines-macro-usage)

}  // namespace

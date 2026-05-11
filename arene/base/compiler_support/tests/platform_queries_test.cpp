// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file platform_queries_test.cpp
/// @brief Unit tests for platform_queries.hpp
///
/// These unit tests simply validate the expected macros are exported from the expected location. It's not possible to
/// do better without the ability to test if code compiles, which is non-trivial in pre C++20 contexts.
///

#include "arene/base/compiler_support/platform_queries.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

/// @test Ensure that the query for GCC is defined
TEST(PlatformQueries, AreneGcc) { STATIC_ASSERT_TRUE(ARENE_GUARANTEE_INTERNAL_DEFINITION(ARENE_GCC)); }

/// @test Ensure that the query for clang is defined
TEST(PlatformQueries, AreneClang) { STATIC_ASSERT_TRUE(ARENE_GUARANTEE_INTERNAL_DEFINITION(ARENE_CLANG)); }

/// @test Ensure that the query for libc++ is defined
TEST(PlatformQueries, AreneStdLibLibCxx) {
  STATIC_ASSERT_TRUE(ARENE_GUARANTEE_INTERNAL_DEFINITION(ARENE_STDLIB_LIBCXX));
}

/// @test Ensure that the query for libstdc++ is defined
TEST(PlatformQueries, AreneStdLibLibStdCxx) {
  STATIC_ASSERT_TRUE(ARENE_GUARANTEE_INTERNAL_DEFINITION(ARENE_STDLIB_LIBSTDCXX));
}

/// @test Ensure that the query for Address Sanitizer is defined
TEST(PlatformQueries, AreneAsanEnabled) { STATIC_ASSERT_TRUE(ARENE_GUARANTEE_INTERNAL_DEFINITION(ARENE_ASAN_ENABLED)); }

/// @test Ensure `ARENE_HAS_BUILTIN` is defined
TEST(PlatformQueries, AreneHasBuiltin) {
#ifndef ARENE_HAS_BUILTIN
  FAIL() << "Expected ARENE_HAS_BUILTIN to be defined";
#endif
}

/// @test Ensure `ARENE_HAS_FEATURE` is defined
TEST(PlatformQueries, AreneHasFeature) {
#ifndef ARENE_HAS_FEATURE
  FAIL() << "Expected ARENE_HAS_BUILTIN to be defined";
#endif
}

/// @test Ensure `ARENE_HAS_INCLUDE` is defined
TEST(PlatformQueries, AreneHasInclude) {
#ifndef ARENE_HAS_INCLUDE
  FAIL() << "Expected ARENE_HAS_INCLUDE to be defined";
#endif
}

/// @test Ensure that the query for a built-in `fabs` function is defined
TEST(PlatformQueries, AreneHasBuiltinFabs) {
  STATIC_ASSERT_TRUE(ARENE_GUARANTEE_INTERNAL_DEFINITION(ARENE_HAS_BUILTIN_FABS));
}

/// @test Ensure that the query for a built-in `fabsf` function is defined
TEST(PlatformQueries, AreneHasBuiltinFabsf) {
  STATIC_ASSERT_TRUE(ARENE_GUARANTEE_INTERNAL_DEFINITION(ARENE_HAS_BUILTIN_FABSF));
}

/// @test Ensure that the query for inline variable support is defined
TEST(PlatformQueries, AreneStdInlineVariables) {
  STATIC_ASSERT_TRUE(ARENE_GUARANTEE_INTERNAL_DEFINITION(ARENE_STD_INLINE_VARIABLES));
}

/// @test Ensure that the query for `std::string_view` is defined
TEST(PlatformQueries, AreneStdLibraryStringView) {
  STATIC_ASSERT_TRUE(ARENE_GUARANTEE_INTERNAL_DEFINITION(ARENE_STD_LIBRARY_STRING_VIEW));
}

/// @test Ensure that the query for `std::starts_with` and `std::ends_with` is defined
TEST(PlatformQueries, AreneStdLibraryStartsEndsWith) {
  STATIC_ASSERT_TRUE(ARENE_GUARANTEE_INTERNAL_DEFINITION(ARENE_STD_LIBRARY_STARTS_ENDS_WITH));
}

/// @test Ensure that the query for `std::string::contains` is defined
TEST(PlatformQueries, AreneStdLibraryStringContains) {
  STATIC_ASSERT_TRUE(ARENE_GUARANTEE_INTERNAL_DEFINITION(ARENE_STD_LIBRARY_STRING_CONTAINS));
}

/// @test Ensure that the query for `std::uncaught_exceptions` is defined
TEST(PlatformQueries, AreneStdLibraryUncaughtExceptions) {
  STATIC_ASSERT_TRUE(ARENE_GUARANTEE_INTERNAL_DEFINITION(ARENE_STD_LIBRARY_UNCAUGHT_EXCEPTIONS));
}

/// @test Ensure that the query for noexcept function type is defined
TEST(PlatformQueries, AreneStdNoexceptFunctionType) {
  STATIC_ASSERT_TRUE(ARENE_GUARANTEE_INTERNAL_DEFINITION(ARENE_STD_NOEXCEPT_FUNCTION_TYPE));
}

/// @test Ensure that the query for constexpr bit_cast is defined
TEST(PlatformQueries, AreneStdNoexceptFunctionType) {
  STATIC_ASSERT_TRUE(ARENE_GUARANTEE_INTERNAL_DEFINITION(ARENE_HAS_CONSTEXPR_BIT_CAST));
}

}  // namespace

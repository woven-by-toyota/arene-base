// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file conditional_assertions.hpp
/// @brief Macros to help define assertions that are conditionally static depending on the test's type parameter
///

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_GTEST_CONDITIONAL_ASSERTIONS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_GTEST_CONDITIONAL_ASSERTIONS_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/testing/gtest/.*"

#include <gtest/gtest.h>

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/testing/gtest/static_assertions_config.hpp"

#if ARENE_IS_ON(ARENE_GTEST_STATIC_ASSERTIONS)
#include "arene/base/testing/gtest/detail/conditional_assertions_with_static.hpp"  // IWYU pragma: export
#else
#include "arene/base/testing/gtest/detail/conditional_assertions_without_static.hpp"  // IWYU pragma: export
#endif
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// NOLINTBEGIN(cppcoreguidelines-macro-usage) This code interacts with Google Test's macros so it needs to be too.
// parasoft-begin-suppress AUTOSAR-A16_0_1-d "Defining these macros is necessary because Google Test uses macros"
// parasoft-begin-suppress CERT_C-PRE30-a "Token concatenation is necessary because Google Test does it too"
// parasoft-begin-suppress AUTOSAR-M16_0_6-a "Some args can not be parenthesized to preserve decltype(auto)"

/// @brief Call a Google Test unary assertion macro at runtime, and statically if @c TypeParam is compatible
/// @param condition The condition to assert; should not contain reference to non-constexpr variables
/// @param assertion_type The @c ::arene::base::testing::gtest_operation value specifying the assertion to use
#define COND_STATIC_ASSERT_UNARY(assertion_type, ...)                      \
  struct ARENE_CONCATENATE(condition_impl_, __LINE__) {                    \
    constexpr static auto body() -> decltype(auto) { return __VA_ARGS__; } \
  };                                                                       \
  static_cast<void>((ARENE_CONDITIONAL_ASSERTER((assertion_type))::do_assert()))

/// @brief Call Google Test @c ASSERT_TRUE at runtime, and also statically if @c TypeParam is compatible
/// @param condition The condition to assert as true; should not contain references to non-constexpr variables
#define COND_STATIC_ASSERT_TRUE(...) \
  COND_STATIC_ASSERT_UNARY(::arene::base::testing::gtest_operation::assert_true, __VA_ARGS__)

/// @brief Call Google Test @c ASSERT_FALSE at runtime, and also statically if @c TypeParam is compatible
/// @param condition The condition to assert as false; should not contain references to non-constexpr variables
#define COND_STATIC_ASSERT_FALSE(...) \
  COND_STATIC_ASSERT_UNARY(::arene::base::testing::gtest_operation::assert_false, __VA_ARGS__)

/// @brief Call a Google Test binary assertion macro at runtime, and statically if @c TypeParam is compatible
/// @param left_arg Left half of the assertion; should not contain references to non-constexpr variables
/// @param right_arg Right half of the assertion; should not contain references to non-constexpr variables
/// @param assertion_type The @c ::arene::base::testing::gtest_operation value specifying the assertion to use
#define COND_STATIC_ASSERT_BINARY(left_arg, right_arg, assertion_type)    \
  struct ARENE_CONCATENATE(condition_impl_, __LINE__) {                   \
    constexpr static auto left() -> decltype(auto) { return left_arg; }   \
    constexpr static auto right() -> decltype(auto) { return right_arg; } \
  };                                                                      \
  static_cast<void>((ARENE_CONDITIONAL_ASSERTER((assertion_type))::do_assert()))

/// @brief Call Google Test @c ASSERT_EQ at runtime, and also statically if @c TypeParam is compatible
/// @param left_arg Left half of the assertion; should not contain references to non-constexpr variables
/// @param right_arg Right half of the assertion; should not contain references to non-constexpr variables
#define COND_STATIC_ASSERT_EQ(left_arg, right_arg) \
  COND_STATIC_ASSERT_BINARY(left_arg, right_arg, ::arene::base::testing::gtest_operation::assert_eq)

/// @brief Call Google Test @c ASSERT_NE at runtime, and also statically if @c TypeParam is compatible
/// @param left_arg Left half of the assertion; should not contain references to non-constexpr variables
/// @param right_arg Right half of the assertion; should not contain references to non-constexpr variables
#define COND_STATIC_ASSERT_NE(left_arg, right_arg) \
  COND_STATIC_ASSERT_BINARY(left_arg, right_arg, ::arene::base::testing::gtest_operation::assert_ne)

/// @brief Call Google Test @c ASSERT_LT at runtime, and also statically if @c TypeParam is compatible
/// @param left_arg Left half of the assertion; should not contain references to non-constexpr variables
/// @param right_arg Right half of the assertion; should not contain references to non-constexpr variables
#define COND_STATIC_ASSERT_LT(left_arg, right_arg) \
  COND_STATIC_ASSERT_BINARY(left_arg, right_arg, ::arene::base::testing::gtest_operation::assert_lt)

/// @brief Call Google Test @c ASSERT_LE at runtime, and also statically if @c TypeParam is compatible
/// @param left_arg Left half of the assertion; should not contain references to non-constexpr variables
/// @param right_arg Right half of the assertion; should not contain references to non-constexpr variables
#define COND_STATIC_ASSERT_LE(left_arg, right_arg) \
  COND_STATIC_ASSERT_BINARY(left_arg, right_arg, ::arene::base::testing::gtest_operation::assert_le)

/// @brief Call Google Test @c ASSERT_GT at runtime, and also statically if @c TypeParam is compatible
/// @param left_arg Left half of the assertion; should not contain references to non-constexpr variables
/// @param right_arg Right half of the assertion; should not contain references to non-constexpr variables
#define COND_STATIC_ASSERT_GT(left_arg, right_arg) \
  COND_STATIC_ASSERT_BINARY(left_arg, right_arg, ::arene::base::testing::gtest_operation::assert_gt)

/// @brief Call Google Test @c ASSERT_GE at runtime, and also statically if @c TypeParam is compatible
/// @param left_arg Left half of the assertion; should not contain references to non-constexpr variables
/// @param right_arg Right half of the assertion; should not contain references to non-constexpr variables
#define COND_STATIC_ASSERT_GE(left_arg, right_arg) \
  COND_STATIC_ASSERT_BINARY(left_arg, right_arg, ::arene::base::testing::gtest_operation::assert_ge)

/// @brief Call the Google Test suite instantiation macro with the given arguments
/// This exists to avoid having more than one @c ## in a single macro, since concatenation is needed for Google Test
/// The extra comma in the Google macro is part of its undocumented name generator API, which we don't use.
/// @param Prefix The prefix to use for this instantiation of the tests; passed directly to Google Test
/// @param SuiteBaseName The name of the suite to instantiate *without* the @c Test , @c DeathTest , etc. at the end
/// @param SuiteSuffix The suffix which is combined with @c SuiteBaseName to create the Google Test suite name
/// @param Types The name of a Google Test type list containing the types for which @c SuiteName should be instantiated
#define ARENE_INSTANTIATE_TYPED_TEST_SUITE_P(Prefix, SuiteBaseName, SuiteSuffix, Types) \
  INSTANTIATE_TYPED_TEST_SUITE_P(Prefix, SuiteBaseName##SuiteSuffix, Types, )

/// @brief Instantiate the Arene Base parameterized tests for the given suite with the given types.
/// Always instantiates the general and death tests, and also the constexpr-only tests for compatible types.
/// @param Prefix The prefix to use for this instantiation of the tests; passed directly to Google Test
/// @param SuiteBaseName The name of the suite to instantiate *without* the @c Test , @c DeathTest , etc. at the end
/// @param Types The name of a Google Test type list containing the types for which @c SuiteName should be instantiated
#define ARENE_INSTANTIATE_TESTS(Prefix, SuiteBaseName, Types)                                          \
  namespace {                                                                                          \
  /* NOLINTNEXTLINE(google-build-using-namespace) Google Test puts a lot of names in this namespace */ \
  using namespace ::arene::base::testing;                                                              \
  ARENE_INSTANTIATE_TYPED_TEST_SUITE_P(Prefix, SuiteBaseName, Test, Types);                            \
  ARENE_INSTANTIATE_TYPED_TEST_SUITE_P(Prefix, SuiteBaseName, DeathTest, Types);                       \
  } /* namespace */                                                                                    \
  static_assert(true, "Dummy static assert to absorb the semicolon after invoking the macro")

// parasoft-end-suppress AUTOSAR-M16_0_6-a
// parasoft-end-suppress CERT_C-PRE30-a
// parasoft-end-suppress AUTOSAR-A16_0_1-d
// NOLINTEND(cppcoreguidelines-macro-usage)

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_GTEST_CONDITIONAL_ASSERTIONS_HPP_

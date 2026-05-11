// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file static_assertions_enabled.hpp
/// @brief Provides the implementation of the static assertion macros when they are enabled.
///
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_GTEST_DETAIL_STATIC_ASSERTIONS_ENABLED_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_GTEST_DETAIL_STATIC_ASSERTIONS_ENABLED_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

// IWYU pragma: private
// IWYU pragma: friend "arene/base/testing/gtest/.*"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/math/abs.hpp"                                        // IWYU pragma: keep
#include "arene/base/testing/gtest/detail/static_assertions_helpers.hpp"  // IWYU pragma: keep

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
// parasoft-begin-suppress AUTOSAR-A16_0_1-d-2 "Defines function-like macros permitted by A16-0-1 Permit #1"
// parasoft-begin-suppress AUTOSAR-A16_0_1-a-2 "Conditional defines permitted by A16-0-1 Permit #2"
// parasoft-begin-suppress CERT_C-PRE31-c-3 "False Positive: static_assert is not a macro"

// parasoft-begin-suppress AUTOSAR-M16_0_6-a-2 "cannot be parenthesized without breaking macro"
/// @brief Implementation helper for a relational comparison assert
///
/// @param lhs The left hand operand
/// @param opr The comparison operator to use. Ex: @c ==
/// @param rhs The right hand operand
/// @param gtest_assertion The gtest assertion macro to use. Ex: @c ASSERT_EQ
#define ARENE_BASE_GTEST_STATIC_ASSERTION_RELATIONAL_I_(lhs, opr, rhs, gtest_assertion)                             \
  static_assert((lhs)opr(rhs), "Expected " ARENE_STRINGIZE(lhs) " " ARENE_STRINGIZE(opr) " " ARENE_STRINGIZE(rhs)); \
  gtest_assertion((lhs), (rhs))
// parasoft-end-suppress AUTOSAR-M16_0_6-a-2

/// @brief Asserts that <c>lhs == rhs</c> at compile time, equivalent to <c>ASSERT_EQ(lhs, rhs)</c>
///
/// @param lhs the left hand operand
/// @param rhs the right hand operand
#define STATIC_ASSERT_EQ(lhs, rhs) ARENE_BASE_GTEST_STATIC_ASSERTION_RELATIONAL_I_((lhs), ==, (rhs), ASSERT_EQ)

/// @brief Asserts that <c>lhs != rhs</c> at compile time, equivalent to <c>ASSERT_NE(lhs, rhs)</c>
///
/// @param lhs the left hand operand
/// @param rhs the right hand operand
#define STATIC_ASSERT_NE(lhs, rhs) ARENE_BASE_GTEST_STATIC_ASSERTION_RELATIONAL_I_((lhs), !=, (rhs), ASSERT_NE)

/// @brief Asserts that <c>lhs < rhs</c> at compile time, equivalent to <c>ASSERT_LT(lhs, rhs)</c>
///
/// @param lhs the left hand operand
/// @param rhs the right hand operand
#define STATIC_ASSERT_LT(lhs, rhs) ARENE_BASE_GTEST_STATIC_ASSERTION_RELATIONAL_I_((lhs), <, (rhs), ASSERT_LT)

/// @brief Asserts that <c>lhs <= rhs</c> at compile time, equivalent to <c>ASSERT_LE(lhs, rhs)</c>
///
/// @param lhs the left hand operand
/// @param rhs the right hand operand
#define STATIC_ASSERT_LE(lhs, rhs) ARENE_BASE_GTEST_STATIC_ASSERTION_RELATIONAL_I_((lhs), <=, (rhs), ASSERT_LE)

/// @brief Asserts that <c>lhs > rhs</c> at compile time, equivalent to <c>ASSERT_GT(lhs, rhs)</c>
///
/// @param lhs the left hand operand
/// @param rhs the right hand operand
#define STATIC_ASSERT_GT(lhs, rhs) ARENE_BASE_GTEST_STATIC_ASSERTION_RELATIONAL_I_((lhs), >, (rhs), ASSERT_GT)

/// @brief Asserts that <c>lhs >= rhs</c> at compile time, equivalent to <c>ASSERT_GE(lhs, rhs)</c>
///
/// @param lhs the left hand operand
/// @param rhs the right hand operand
#define STATIC_ASSERT_GE(lhs, rhs) ARENE_BASE_GTEST_STATIC_ASSERTION_RELATIONAL_I_((lhs), >=, (rhs), ASSERT_GE)

/// @brief Asserts that <c>value == true</c> at compile time, equivalent to <c>ASSERT_TRUE(value)</c>.
///
/// @param value The value to assert truth-ness of
#ifndef ARENE_DOC_GENERATION_RUNNING
#define STATIC_ASSERT_TRUE(...)                                                 \
  static_assert(                                                                \
      arene::base::testing::gtest::detail::static_assertion_check(__VA_ARGS__), \
      "Expected " ARENE_STRINGIZE(__VA_ARGS__) " to be true."                   \
  );                                                                            \
  ASSERT_TRUE(arene::base::testing::gtest::detail::static_assertion_check(__VA_ARGS__))
#else
#define STATIC_ASSERT_TRUE(value)
#endif

/// @brief Asserts that <c>value == false</c> at compile time, equivalent to <c>ASSERT_FALSE(value)</c>.
///
/// @param value The value to assert truth-ness of
#ifndef ARENE_DOC_GENERATION_RUNNING
#define STATIC_ASSERT_FALSE(...)                                                 \
  static_assert(                                                                 \
      !arene::base::testing::gtest::detail::static_assertion_check(__VA_ARGS__), \
      "Expected " ARENE_STRINGIZE(__VA_ARGS__) " to be false."                   \
  );                                                                             \
  ASSERT_FALSE(arene::base::testing::gtest::detail::static_assertion_check(__VA_ARGS__))
#else
#define STATIC_ASSERT_FALSE(value)
#endif

/// @brief Asserts that <c>abs(lhs - rhs) <= abs_error </c> at compile time, equivalent to <c>ASSERT_NEAR(lhs, rhs,
/// abs(abs_error))</c>
///
/// @param lhs the left hand operand
/// @param rhs the right hand operand
/// @param abs_error the maximum difference between the two values
/// @pre abs_error must not be a negative value. gtest's @c ASSERT_NEAR does not validate this, but it is a requirement,
/// and so it is guarded against here with a @c static_assert.
#define STATIC_ASSERT_NEAR(lhs, rhs, abs_error)                                                            \
  static_assert((abs_error) >= 0, "abs_error must not be a negative value");                               \
  {                                                                                                        \
    constexpr auto abs_delta = ::arene::base::abs((lhs) - (rhs));                                          \
    static_assert(                                                                                         \
        abs_delta <= (abs_error),                                                                          \
        "Expected abs(" ARENE_STRINGIZE(lhs) " - " ARENE_STRINGIZE(rhs) ") <= " ARENE_STRINGIZE(abs_error) \
    );                                                                                                     \
  }                                                                                                        \
  ASSERT_NEAR((lhs), (rhs), (abs_error))

/// @brief Asserts that <c>lhs == rhs</c> at compile time, equivalent to <c>ASSERT_STREQ(lhs, rhs)</c>
///
/// @param lhs the left hand operand
/// @param rhs the right hand operand
#define STATIC_ASSERT_STREQ(lhs, rhs)                                           \
  static_assert(                                                                \
      ::arene::base::testing::gtest::detail::c_strings_are_equal((lhs), (rhs)), \
      "Expected " ARENE_STRINGIZE(lhs) " == " ARENE_STRINGIZE(rhs)              \
  );                                                                            \
  ASSERT_STREQ((lhs), (rhs))

/// @brief Asserts that <c>lhs != rhs</c> at compile time, equivalent to <c>ASSERT_STRNE(lhs, rhs)</c>
///
/// @param lhs the left hand operand
/// @param rhs the right hand operand
#define STATIC_ASSERT_STRNE(lhs, rhs)                                            \
  static_assert(                                                                 \
      !::arene::base::testing::gtest::detail::c_strings_are_equal((lhs), (rhs)), \
      "Expected " ARENE_STRINGIZE(lhs) " != " ARENE_STRINGIZE(rhs)               \
  );                                                                             \
  ASSERT_STRNE((lhs), (rhs))

// NOLINTEND(cppcoreguidelines-macro-usage)

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_GTEST_DETAIL_STATIC_ASSERTIONS_ENABLED_HPP_

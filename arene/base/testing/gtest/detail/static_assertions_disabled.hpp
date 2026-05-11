// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file static_assertions_disabled.hpp
/// @brief Provides the implementation of the static assertion macros when they are disabled.
///
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_GTEST_DETAIL_STATIC_ASSERTIONS_DISABLED_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_GTEST_DETAIL_STATIC_ASSERTIONS_DISABLED_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/testing/gtest/.*"

#include <gtest/gtest.h>

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/testing/gtest/detail/static_assertions_helpers.hpp"  // IWYU pragma: keep
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// NOLINTBEGIN(cppcoreguidelines-macro-usage)

// parasoft-begin-suppress AUTOSAR-A16_0_1-d-2 "Defines function-like macros permitted by A16-0-1 Permit #1"
// GTest assertion macros indirectly use '#' pre-processor operators to manipulate tokens.

// parasoft-begin-suppress AUTOSAR-A16_0_1-a-2 "Conditional defines permitted by A16-0-1 Permit #2"

// parasoft-begin-suppress AUTOSAR-M16_0_6-a-2 "'gtest_assertion' cannot be parenthesized without breaking macro"
/// @brief Implementation helper for a relational comparison assert when static assertions are disabled.
///
/// @param lhs The left hand operand
/// @param opr The comparison operator to use. Ex: @c ==
/// @param rhs The right hand operand
/// @param gtest_assertion The gtest assertion macro to use. Ex: @c ASSERT_EQ
///
#define ARENE_BASE_GTEST_STATIC_ASSERTION_RELATIONAL_I_(lhs, opr, rhs, gtest_assertion) gtest_assertion((lhs), (rhs))
// parasoft-end-suppress AUTOSAR-M16_0_6-a-2

/// @brief Equivalent to the gtest macro of the same name minus @c STATIC_ .
/// @param lhs the left hand operand
/// @param rhs the right hand operand
/// @{
#define STATIC_ASSERT_EQ(lhs, rhs) ARENE_BASE_GTEST_STATIC_ASSERTION_RELATIONAL_I_((lhs), ==, (rhs), ASSERT_EQ)
#define STATIC_ASSERT_NE(lhs, rhs) ARENE_BASE_GTEST_STATIC_ASSERTION_RELATIONAL_I_((lhs), !=, (rhs), ASSERT_NE)
#define STATIC_ASSERT_LT(lhs, rhs) ARENE_BASE_GTEST_STATIC_ASSERTION_RELATIONAL_I_((lhs), <, (rhs), ASSERT_LT)
#define STATIC_ASSERT_LE(lhs, rhs) ARENE_BASE_GTEST_STATIC_ASSERTION_RELATIONAL_I_((lhs), <=, (rhs), ASSERT_LE)
#define STATIC_ASSERT_GT(lhs, rhs) ARENE_BASE_GTEST_STATIC_ASSERTION_RELATIONAL_I_((lhs), >, (rhs), ASSERT_GT)
#define STATIC_ASSERT_GE(lhs, rhs) ARENE_BASE_GTEST_STATIC_ASSERTION_RELATIONAL_I_((lhs), >=, (rhs), ASSERT_GE)
/// @}

/// @brief Equivalent to the gtest macro of the same name minus @c STATIC_ .
/// @param value The value to assert truth-ness of
#ifndef ARENE_DOC_GENERATION_RUNNING
#define STATIC_ASSERT_TRUE(...) ASSERT_TRUE(arene::base::testing::gtest::detail::static_assertion_check(__VA_ARGS__))
#else
#define STATIC_ASSERT_TRUE(value)
#endif

/// @brief Equivalent to the gtest macro of the same name minus @c STATIC_ .
/// @param value The value to assert false-ness of
#ifndef ARENE_DOC_GENERATION_RUNNING
#define STATIC_ASSERT_FALSE(...) ASSERT_FALSE(arene::base::testing::gtest::detail::static_assertion_check(__VA_ARGS__))
#else
#define STATIC_ASSERT_FALSE(value)
#endif

/// @brief Equivalent to the gtest macro of the same name minus @c STATIC_ .
/// @param lhs the left hand operand
/// @param rhs the right hand operand
/// @param abs_error the maximum difference between the two values
/// @pre abs_error must be a positive value. gtest's @c ASSERT_NEAR does not
///      validate this, but it is a requirement, and so it is guarded against
///      here with a @c static_assert.
#define STATIC_ASSERT_NEAR(lhs, rhs, abs_error)                          \
  static_assert((abs_error) >= 0, "abs_error must be a positive value"); \
  ASSERT_NEAR((lhs), (rhs), (abs_error))

/// @brief Equivalent to the gtest macro of the same name minus @c STATIC_ .
/// @param lhs the left hand operand
/// @param rhs the right hand operand
/// @{
#define STATIC_ASSERT_STREQ(lhs, rhs) ASSERT_STREQ((lhs), (rhs))
#define STATIC_ASSERT_STRNE(lhs, rhs) ASSERT_STRNE((lhs), (rhs))
/// @}

// NOLINTEND(cppcoreguidelines-macro-usage)

// parasoft-end-suppress AUTOSAR-A16_0_1-a-2 "Conditional defines permitted by A16-0-1 Permit #2"
// parasoft-end-suppress AUTOSAR-A16_0_1-d-2 "Defines function-like macros permitted by A16-0-1 Permit #1"

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_GTEST_DETAIL_STATIC_ASSERTIONS_DISABLED_HPP_

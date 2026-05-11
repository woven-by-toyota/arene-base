// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file conditional_assertions_with_static.hpp
/// @brief Provides the implementation of the conditional assertion macros when static checks are enabled.
///
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_GTEST_DETAIL_CONDITIONAL_ASSERTIONS_WITH_STATIC_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_GTEST_DETAIL_CONDITIONAL_ASSERTIONS_WITH_STATIC_HPP_

// IWYU pragma: private, include "arene/base/testing/gtest/conditional_assertions.hpp"
// IWYU pragma: friend "arene/base/testing/gtest/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/testing/gtest/detail/conditional_assertions_helpers.hpp"  // IWYU pragma: keep
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
// parasoft-begin-suppress AUTOSAR-A16_0_1-d-2 "Defines function-like macros permitted by A16-0-1 Permit #1"

/// @brief Return the name of a @c conditional_asserter for the current line using the given Google Test assertion type
/// @param assertion_type The @c testing_detail::gtest_operation enum value specifying the assertion to use
#define ARENE_CONDITIONAL_ASSERTER(assertion_type)              \
  ::arene::base::testing::testing_detail::conditional_asserter< \
      ARENE_CONCATENATE(condition_impl_, __LINE__),             \
      (assertion_type),                                         \
      TestFixture::constexpr_compatible>

// NOLINTEND(cppcoreguidelines-macro-usage)

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_GTEST_DETAIL_CONDITIONAL_ASSERTIONS_WITH_STATIC_HPP_

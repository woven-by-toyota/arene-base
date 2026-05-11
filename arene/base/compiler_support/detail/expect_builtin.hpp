// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_DETAIL_EXPECT_BUILTIN_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_DETAIL_EXPECT_BUILTIN_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/compiler_support/expect.hpp"

// parasoft-begin-suppress AUTOSAR-A16_0_1-d-2 "This header defines compiler support macros"

/// @brief An internal macro for @c ARENE_EXPECT when @c __builtin_expect is supported.
///
/// @param expression The expression to evaluate
/// @pre expression must be convertible to @c bool
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ARENE_EXPECT_INTERNAL(expression) __builtin_expect(::arene::base::expect_detail::check(expression), 1)

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_DETAIL_EXPECT_BUILTIN_HPP_

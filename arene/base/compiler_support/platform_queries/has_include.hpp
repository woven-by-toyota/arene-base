// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PLATFORM_QUERIES_HAS_INCLUDE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PLATFORM_QUERIES_HAS_INCLUDE_HPP_

// IWYU pragma: private, include "arene/base/compiler_support/platform_queries.hpp"
// IWYU pragma: friend "arene/base/compiler_support/platform_queries/.*"

// NOLINTBEGIN(cppcoreguidelines-macro-usage) Explicitly providing compiler support macro functions
// parasoft-begin-suppress AUTOSAR-A16_0_1-d-2 "Conditional defines permitted by A16-0-1 Permit #2"
// parasoft-begin-suppress AUTOSAR-A16_0_1-a-2 "Conditional defines permitted by A16-0-1 Permit #2"

/// @def ARENE_HAS_INCLUDE
/// @brief A platform support query for testing if a header exists.
/// @param ... The include to test availability of, including the @c <> or @c "".
/// @return if @c __has_include is defined, the result of @c __has_include(__VA_ARGS__), otherwise @c false .
///
/// Example usage testing for @c <version> :
/// @code{c++}
/// #if ARENE_HAS_INCLUDE(<version>)
/// @endcode

#if defined(__has_include)
#define ARENE_HAS_INCLUDE(...) __has_include(__VA_ARGS__)
#else
#define ARENE_HAS_INCLUDE(...) 0
#endif  // checking for __has_include

// NOLINTEND(cppcoreguidelines-macro-usage) Explicitly providing compiler support macro functions
// parasoft-end-suppress AUTOSAR-A16_0_1-d-2
// parasoft-end-suppress AUTOSAR-A16_0_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PLATFORM_QUERIES_HAS_INCLUDE_HPP_

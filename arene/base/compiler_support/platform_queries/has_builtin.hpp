// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PLATFORM_QUERIES_HAS_BUILTIN_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PLATFORM_QUERIES_HAS_BUILTIN_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/compiler_support/platform_queries/.*"

// NOLINTBEGIN(cppcoreguidelines-macro-usage) Explicitly providing compiler support macro functions
// parasoft-begin-suppress AUTOSAR-A16_0_1-d-2 "Conditional defines permitted by A16-0-1 Permit #2"
// parasoft-begin-suppress AUTOSAR-A16_0_1-a-2 "Conditional defines permitted by A16-0-1 Permit #2"

/// @def ARENE_HAS_BUILTIN
/// @brief A platform support query for testing if a supplied builtin is defined.
/// @param ... The name of the builtin to query for.
/// @return if @c __has_builtin is defined, the result of @c __has_builtin(__VA_ARGS__), otherwise @c 0 .
///
/// Example usage testing for @c __builtin_is_constant_evaluated :
/// @code{c++}
/// #if ARENE_HAS_BUILTIN(__builtin_is_constant_evaluated)
/// @endcode
///

#if !defined(__QNX__) && defined(__has_builtin)
#define ARENE_HAS_BUILTIN(...) __has_builtin(__VA_ARGS__)
#else
#define ARENE_HAS_BUILTIN(...) 0
#endif  // checking for __has_builtin

// NOLINTEND(cppcoreguidelines-macro-usage) Explicitly providing compiler support macro functions
// parasoft-end-suppress AUTOSAR-A16_0_1-d-2
// parasoft-end-suppress AUTOSAR-A16_0_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PLATFORM_QUERIES_HAS_BUILTIN_HPP_

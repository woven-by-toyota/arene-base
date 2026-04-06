// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PLATFORM_QUERIES_HAS_FEATURE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PLATFORM_QUERIES_HAS_FEATURE_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/compiler_support/platform_queries/.*"

// NOLINTBEGIN(cppcoreguidelines-macro-usage) Explicitly providing compiler support macro functions
// parasoft-begin-suppress AUTOSAR-A16_0_1-d-2 "Conditional defines permitted by A16-0-1 Permit #2"
// parasoft-begin-suppress AUTOSAR-A16_0_1-a-2 "Conditional defines permitted by A16-0-1 Permit #2"

/// @def ARENE_HAS_FEATURE
/// @brief A platform support query for testing if a supplied feature is defined.
/// @param ... The name of the feature to query for.
/// @return if @c __has_feature is defined, the result of @c __has_feature(__VA_ARGS__), otherwise @c 0 .
///
/// Example usage testing for the @c address_sanitizer feature:
/// @code{c++}
/// #if ARENE_HAS_FEATURE(address_sanitizer)
/// @endcode
///
/// @note @c __has_feature is a clang-specific facility. It generally shouldn't be used in user code, and instead should
///       be used only as a building block to other platform support queries.
///

#if defined(__has_feature)
#define ARENE_HAS_FEATURE(...) __has_feature(__VA_ARGS__)
#else
#define ARENE_HAS_FEATURE(...) 0
#endif  // checking for __has_feature

// NOLINTEND(cppcoreguidelines-macro-usage) Explicitly providing compiler support macro functions
// parasoft-end-suppress AUTOSAR-A16_0_1-d-2
// parasoft-end-suppress AUTOSAR-A16_0_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PLATFORM_QUERIES_HAS_FEATURE_HPP_

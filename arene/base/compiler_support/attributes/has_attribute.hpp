// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_ATTRIBUTES_HAS_ATTRIBUTE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_ATTRIBUTES_HAS_ATTRIBUTE_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/compiler_support/attributes/.*"

// NOLINTBEGIN(cppcoreguidelines-macro-usage) Explicitly providing compiler support macro functions
// parasoft-begin-suppress AUTOSAR-A16_0_1-d-2 "Conditional defines permitted by A16-0-1 Permit #2"
// parasoft-begin-suppress AUTOSAR-A16_0_1-a-2 "Conditional defines permitted by A16-0-1 Permit #2"

/// @def ARENE_HAS_STD_ATTRIBUTE
/// @brief A wrapper around the "standard" @c __has_cpp_attribute or @c __has_c_attribute query.
/// @param ... The attribute to query for.
/// @return if @c __has_c[pp]_attribute is defined, the result of @c __has_c[pp]_attribute(__VA_ARGS__) , otherwise @c
///         false .

// We only ever compile a single file in C or C++ mode; never both
// therefore, we want has attribute to map to the current standard
// "has attribute"
#if defined(__has_cpp_attribute)
#define ARENE_HAS_STD_ATTRIBUTE(...) __has_cpp_attribute(__VA_ARGS__)
#elif defined(__has_c_attribute)
#define ARENE_HAS_STD_ATTRIBUTE(...) __has_c_attribute(__VA_ARGS__)
#else
#define ARENE_HAS_STD_ATTRIBUTE(...) 0
#endif  // checking for standard __has_(c|cpp)_attribute

/// @def ARENE_HAS_OLD_ATTRIBUTE
/// @brief A wrapper around the "old style" @c __has_attribute query.
/// @param ... The attribute to query for.
/// @return if @c __has_attribute is defined, the result of @c __has_attribute(__VA_ARGS__) , otherwise @c false .
/// @note Users should generally prefer @c ARENE_HAS_STD_ATTRIBUTE or @c ARENE_HAS_ATTRIBUTE .

#if defined(__has_attribute)
#define ARENE_HAS_OLD_ATTRIBUTE(...) __has_attribute(__VA_ARGS__)
#else
#define ARENE_HAS_OLD_ATTRIBUTE(...) 0
#endif  // checking for old-style __has_attribute

/// @def ARENE_HAS_ATTRIBUTE
/// @brief A wrapper for testing for an attribute which will fall back to the "old" style query if the "standard" one is
///        not available.
/// @param ... The attribute to query for.
/// @return The result of invoking @c ARENE_HAS_STD_ATTRIBUTE if possible, else @c ARENE_HAS_OLD_ATTRIBUTE .
/// @note Users should generally prefer @c ARENE_HAS_STD_ATTRIBUTE . Only use this if you know you need to.
#if defined(__has_cpp_attribute)
#define ARENE_HAS_ATTRIBUTE(...) ARENE_HAS_STD_ATTRIBUTE(__VA_ARGS__)
#elif defined(__has_c_attribute)
#define ARENE_HAS_ATTRIBUTE(...) ARENE_HAS_STD_ATTRIBUTE(__VA_ARGS__)
#else
#define ARENE_HAS_ATTRIBUTE(...) ARENE_HAS_OLD_ATTRIBUTE(__VA_ARGS__)
#endif  // checking for any attribute in any form

// NOLINTEND(cppcoreguidelines-macro-usage) Explicitly providing compiler support macro functions
// parasoft-end-suppress AUTOSAR-A16_0_1-d-2
// parasoft-end-suppress AUTOSAR-A16_0_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_ATTRIBUTES_HAS_ATTRIBUTE_HPP_

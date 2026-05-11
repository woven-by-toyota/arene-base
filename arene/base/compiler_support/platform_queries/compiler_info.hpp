// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PLATFORM_QUERIES_COMPILER_INFO_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PLATFORM_QUERIES_COMPILER_INFO_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/compiler_support/platform_queries/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/preprocessor.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// NOLINTBEGIN(cppcoreguidelines-macro-usage) Explicitly providing compiler support macro functions
// parasoft-begin-suppress AUTOSAR-A16_0_1-d-2 "Conditional defines permitted by A16-0-1 Permit #2"
// parasoft-begin-suppress AUTOSAR-A16_0_1-a-2 "Conditional defines permitted by A16-0-1 Permit #2"

/// @def ARENE_COMPILER_CLANG
/// @brief A compile-time configuration flag for testing if the current compiler is @c clang
///
/// Usage:
/// @code{c++}
/// #if ARENE_IS_ON(ARENE_COMPILER_CLANG)
/// @endcode
///
/// The value of this query is determined as follows, in order of precedence:
/// 1. If @c ARENE_COMPILER_CLANG has been explicitly defined via a @c -D flag to the compiler or similar:
///    * @c ARENE_ON if it is truthy
///    * @c ARENE_OFF if it is not truthy.
/// 1. If @c __clang__ is defined, it is @c ARENE_ON_BY_DEFAULT
/// 1. Otherwise, it is @c ARENE_OFF_BY_DEFAULT

#if defined(ARENE_COMPILER_ARMCLANG)
#if (ARENE_COMPILER_ARMCLANG != 0)
#define ARENE_COMPILER_ARMCLANG_I_ ARENE_ON
#else
#define ARENE_COMPILER_ARMCLANG_I_ ARENE_OFF
#endif
#elif defined(__ARMCOMPILER_VERSION)
#define ARENE_COMPILER_ARMCLANG_I_ ARENE_ON_BY_DEFAULT
#else
#define ARENE_COMPILER_ARMCLANG_I_ ARENE_OFF_BY_DEFAULT
#endif

#ifdef ARENE_DOC_GENERATION_RUNNING  // make this appear when running doxygen, as it won't otherwise.
#ifndef ARENE_COMPILER_ARMCLANG
#define ARENE_COMPILER_ARMCLANG
#endif
#endif

#if defined(ARENE_COMPILER_CLANG)
#if (ARENE_COMPILER_CLANG != 0)
#define ARENE_COMPILER_CLANG_I_ ARENE_ON
#else
#define ARENE_COMPILER_CLANG_I_ ARENE_OFF
#endif
// parasoft-begin-suppress AUTOSAR-M16_0_7-a-2 "False positive: not used as a macro"
#elif ARENE_IS_OFF(ARENE_COMPILER_ARMCLANG) && defined(__clang__)
// parasoft-end-suppress AUTOSAR-M16_0_7-a-2
#define ARENE_COMPILER_CLANG_I_ ARENE_ON_BY_DEFAULT
#else
#define ARENE_COMPILER_CLANG_I_ ARENE_OFF_BY_DEFAULT
#endif

#ifdef ARENE_DOC_GENERATION_RUNNING  // make this appear when running doxygen, as it won't otherwise.
#ifndef ARENE_COMPILER_CLANG
#define ARENE_COMPILER_CLANG
#endif
#endif

/// @def ARENE_COMPILER_GCC
/// @brief A compile-time configuration flag for testing if the current compiler is @c gcc
///
/// Usage:
/// @code{c++}
/// #if ARENE_IS_ON(ARENE_COMPILER_GCC)
/// @endcode
///
/// The value of this query is determined as follows, in order of precedence:
/// 1. If @c ARENE_COMPILER_GCC has been explicitly defined via a @c -D flag to the compiler or similar:
///    * @c ARENE_ON if it is truthy
///    * @c ARENE_OFF if it is not truthy.
/// 1. If @c ARENE_IS_OFF(ARENE_COMPILER_CLANG) , and all of @c __GNUC__ , @c __GNUC_MINOR__ and @c __GNUC_PATCH_LEVEL
///    are defined, it is @c ARENE_ON_BY_DEFAULT
/// 1. Otherwise, it is @c ARENE_OFF_BY_DEFAULT

#if defined(ARENE_COMPILER_GCC)
#if (ARENE_COMPILER_GCC != 0)
#define ARENE_COMPILER_GCC_I_ ARENE_ON
#else
#define ARENE_COMPILER_GCC_I_ ARENE_OFF
#endif
// parasoft-begin-suppress AUTOSAR-M16_0_7-a-2 "False positive: not used as a macro"
#elif ARENE_IS_OFF(ARENE_COMPILER_ARMCLANG) && ARENE_IS_OFF(ARENE_COMPILER_CLANG) && \
    (defined(__GNUC__) && defined(__GNUC_MINOR__) && defined(__GNUC_PATCHLEVEL__))
// parasoft-end-suppress AUTOSAR-M16_0_7-a-2
#define ARENE_COMPILER_GCC_I_ ARENE_ON_BY_DEFAULT
#else
#define ARENE_COMPILER_GCC_I_ ARENE_OFF_BY_DEFAULT
#endif

#ifdef ARENE_DOC_GENERATION_RUNNING  // make this appear when running doxygen, as it won't otherwise.
#ifndef ARENE_COMPILER_GCC
#define ARENE_COMPILER_GCC
#endif
#endif

// NOLINTEND(cppcoreguidelines-macro-usage) Explicitly providing compiler support macro functions
// parasoft-end-suppress AUTOSAR-A16_0_1-d-2
// parasoft-end-suppress AUTOSAR-A16_0_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PLATFORM_QUERIES_COMPILER_INFO_HPP_

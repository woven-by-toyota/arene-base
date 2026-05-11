// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PLATFORM_QUERIES_HAS_BUILTIN_ABS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PLATFORM_QUERIES_HAS_BUILTIN_ABS_HPP_

// IWYU pragma: private, include "arene/base/compiler_support/platform_queries.hpp"
// IWYU pragma: friend "arene/base/compiler_support/platform_queries/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/platform_queries/compiler_info.hpp"  // IWYU pragma: keep
#include "arene/base/compiler_support/platform_queries/has_builtin.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
// parasoft-begin-suppress AUTOSAR-A16_0_1-d-2 "Conditional defines permitted by A16-0-1 Permit #2"
// parasoft-begin-suppress AUTOSAR-A16_0_1-a-2 "Conditional defines permitted by A16-0-1 Permit #2"

/// @def ARENE_HAS_BUILTIN_FABS
/// @brief A platform support query for testing if @c __builtin_fabs is defined.
///
/// Usage:
/// @code{c++}
/// #if ARENE_IS_ON(ARENE_HAS_BUILTIN_FABS)
/// @endcode
///
/// The value of this query is determined as follows, in order of precedence:
/// 1. If @c __builtin_fabs is defined, it is @c ARENE_ON_BY_DEFAULT
/// 1. Otherwise, it is @c ARENE_OFF_BY_DEFAULT

// If the macro is defined earlier, do not redefine it.
// This allows testing the difference implementation of the builtin abs.
#ifndef ARENE_HAS_BUILTIN_FABS_I_
// parasoft-begin-suppress AUTOSAR-M16_0_7-a-2 "False positive: not used as a macro"
#if ARENE_HAS_BUILTIN(__builtin_fabs)
// parasoft-end-suppress AUTOSAR-M16_0_7-a-2
#define ARENE_HAS_BUILTIN_FABS_I_ ARENE_ON_BY_DEFAULT
#elif !defined(__QNX__) && ARENE_IS_ON(ARENE_COMPILER_GCC) && __GNUC__ >= 8
// The __builtin_fabs() exists in GCC (at least) 8 and later,
// but the __has_builtin query was not added until GCC 10 and so we
// may have returned a false negative when probing for the builtin above.
#define ARENE_HAS_BUILTIN_FABS_I_ ARENE_ON_BY_DEFAULT
#else
#define ARENE_HAS_BUILTIN_FABS_I_ ARENE_OFF_BY_DEFAULT
#endif
#endif

#ifdef ARENE_DOC_GENERATION_RUNNING  // make this appear when running doxygen, as it won't otherwise.
#ifndef ARENE_HAS_BUILTIN_FABS
#define ARENE_HAS_BUILTIN_FABS
#endif
#endif

/// @def ARENE_HAS_BUILTIN_FABSF
/// @brief A platform support query for testing if @c __builtin_fabsf is defined.
///
/// Usage:
/// @code{c++}
/// #if ARENE_IS_ON(ARENE_HAS_BUILTIN_FABSF)
/// @endcode
///
/// The value of this query is determined as follows, in order of precedence:
/// 1. If @c __builtin_fabsf is defined, it is @c ARENE_ON_BY_DEFAULT
/// 1. Otherwise, it is @c ARENE_OFF_BY_DEFAULT

#ifndef ARENE_HAS_BUILTIN_FABSF_I_
// parasoft-begin-suppress AUTOSAR-M16_0_7-a-2 "False positive: not used as a macro"
#if ARENE_HAS_BUILTIN(__builtin_fabsf)
// parasoft-end-suppress AUTOSAR-M16_0_7-a-2
#define ARENE_HAS_BUILTIN_FABSF_I_ ARENE_ON_BY_DEFAULT
#elif !defined(__QNX__) && ARENE_IS_ON(ARENE_COMPILER_GCC) && __GNUC__ >= 8
// The __builtin_fabsf() exists in GCC (at least) 8 and later,
// but the __builtin_fabsf query was not added until GCC 10 and so we
// may have returned a false negative when probing for the builtin above.
#define ARENE_HAS_BUILTIN_FABSF_I_ ARENE_ON_BY_DEFAULT
#else
#define ARENE_HAS_BUILTIN_FABSF_I_ ARENE_OFF_BY_DEFAULT
#endif
#endif

#ifdef ARENE_DOC_GENERATION_RUNNING  // make this appear when running doxygen, as it won't otherwise.
#ifndef ARENE_HAS_BUILTIN_FABSF
#define ARENE_HAS_BUILTIN_FABSF
#endif
#endif

// NOLINTEND(cppcoreguidelines-macro-usage)
// parasoft-end-suppress AUTOSAR-A16_0_1-d-2
// parasoft-end-suppress AUTOSAR-A16_0_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PLATFORM_QUERIES_HAS_BUILTIN_ABS_HPP_

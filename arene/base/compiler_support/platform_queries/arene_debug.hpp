// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PLATFORM_QUERIES_ARENE_DEBUG_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PLATFORM_QUERIES_ARENE_DEBUG_HPP_

// IWYU pragma: private, include "arene/base/compiler_support/platform_queries.hpp"
// IWYU pragma: friend "arene/base/compiler_support/platform_queries/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/platform_queries/compiler_info.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// NOLINTBEGIN(cppcoreguidelines-macro-usage) Explicitly providing compiler support macro functions
// parasoft-begin-suppress AUTOSAR-A16_0_1-d-2 "Conditional defines permitted by A16-0-1 Permit #2"
// parasoft-begin-suppress AUTOSAR-A16_0_1-a-2 "Conditional defines permitted by A16-0-1 Permit #2"

/// @def ARENE_DEBUG
/// @brief A compile-time configuration flag for testing if "debug" has been configured.
///
/// Usage:
/// @code{c++}
/// #if ARENE_IS_ON(ARENE_DEBUG)
/// @endcode
///
/// The value of this query is determined as follows, in order of precedence:
/// 1. If @c ARENE_DEBUG has been explicitly defined via a @c -D flag to the compiler or similar:
///    * @c ARENE_ON if it is truthy
///    * @c ARENE_OFF if it is not truthy.
/// 1. If @c NDEBUG is defined, it is @c ARENE_OFF_BY_DEFAULT
/// 1. If @c ARENE_COMPILER_CLANG or @c ARENE_COMPILER_GCC are on, and @c __OPTIMIZE__ is defined, it is
///    @c ARENE_OFF_BY_DEFAULT
/// 1. Otherwise, it is @c ARENE_ON_BY_DEFAULT

#if defined(ARENE_DEBUG)
#if (ARENE_DEBUG != 0)
#define ARENE_DEBUG_I_ ARENE_ON
#else
#define ARENE_DEBUG_I_ ARENE_OFF
#endif
#elif defined(NDEBUG)
#define ARENE_DEBUG_I_ ARENE_OFF_BY_DEFAULT
#elif (ARENE_IS_ON(ARENE_COMPILER_CLANG) || ARENE_IS_ON(ARENE_COMPILER_GCC)) && defined(__OPTIMIZE__)
#define ARENE_DEBUG_I_ ARENE_OFF_BY_DEFAULT
#else
#define ARENE_DEBUG_I_ ARENE_ON_BY_DEFAULT
#endif

#ifdef ARENE_DOC_GENERATION_RUNNING  // make this appear when running doxygen, as it won't otherwise.
#ifndef ARENE_DEBUG
#define ARENE_DEBUG
#endif
#endif

// NOLINTEND(cppcoreguidelines-macro-usage) Explicitly providing compiler support macro functions
// parasoft-end-suppress AUTOSAR-A16_0_1-d-2
// parasoft-end-suppress AUTOSAR-A16_0_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PLATFORM_QUERIES_ARENE_DEBUG_HPP_

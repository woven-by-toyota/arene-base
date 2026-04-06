// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PLATFORM_QUERIES_EXCEPTIONS_ENABLED_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PLATFORM_QUERIES_EXCEPTIONS_ENABLED_HPP_

// IWYU pragma: private, include "arene/base/compiler_support/platform_queries.hpp"
// IWYU pragma: friend "arene/base/compiler_support/platform_queries/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/platform_queries/has_feature.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// NOLINTBEGIN(cppcoreguidelines-macro-usage) Explicitly providing compiler support macro functions
// parasoft-begin-suppress AUTOSAR-A16_0_1-d-2 "Conditional defines permitted by A16-0-1 Permit #2"
// parasoft-begin-suppress AUTOSAR-A16_0_1-a-2 "Conditional defines permitted by A16-0-1 Permit #2"

/// @def ARENE_EXCEPTIONS_ENABLED
/// @brief A compile-time configuration flag for testing if exceptions are enabled
///
/// Usage:
/// @code{c++}
/// #if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)
/// @endcode
///
/// The value of this query is determined as follows, in order of precedence:
/// 1. If @c ARENE_EXCEPTIONS_ENABLED has been explicitly defined via a @c -D flag to the compiler or similar:
///    * @c ARENE_ON if it is truthy
///    * @c ARENE_OFF if it is not truthy.
/// 1. If @c __cpp_exceptions is defined, it is @c ARENE_ON_BY_DEFAULT
/// 1. If @c __EXCEPTIONS is defined, it is @c ARENE_ON_BY_DEFAULT
/// 1. Otherwise, it is @c ARENE_OFF_BY_DEFAULT

#if defined(ARENE_EXCEPTIONS_ENABLED)
#if (ARENE_EXCEPTIONS_ENABLED != 0)
#define ARENE_EXCEPTIONS_ENABLED_I_ ARENE_ON
#else
#define ARENE_EXCEPTIONS_ENABLED_I_ ARENE_OFF
#endif
// parasoft-begin-suppress AUTOSAR-M16_0_7-a-2 "False positive: not used as a macro"
#elif ARENE_HAS_FEATURE(cxx_exceptions)
// parasoft-end-suppress AUTOSAR-M16_0_7-a-2
#define ARENE_EXCEPTIONS_ENABLED_I_ ARENE_ON_BY_DEFAULT
#elif defined(__cpp_exceptions)
#define ARENE_EXCEPTIONS_ENABLED_I_ ARENE_ON_BY_DEFAULT
#elif defined(__EXCEPTIONS)
#define ARENE_EXCEPTIONS_ENABLED_I_ ARENE_ON_BY_DEFAULT
#else
#define ARENE_EXCEPTIONS_ENABLED_I_ ARENE_OFF_BY_DEFAULT
#endif

#ifdef ARENE_DOC_GENERATION_RUNNING  // make this appear when running doxygen, as it won't otherwise.
#ifndef ARENE_EXCEPTIONS_ENABLED
#define ARENE_EXCEPTIONS_ENABLED
#endif
#endif

// NOLINTEND(cppcoreguidelines-macro-usage) Explicitly providing compiler support macro functions
// parasoft-end-suppress AUTOSAR-A16_0_1-d-2
// parasoft-end-suppress AUTOSAR-A16_0_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PLATFORM_QUERIES_EXCEPTIONS_ENABLED_HPP_

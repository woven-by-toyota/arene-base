// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PLATFORM_QUERIES_HAS_STRING_CONTAINS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PLATFORM_QUERIES_HAS_STRING_CONTAINS_HPP_

// IWYU pragma: private, include "arene/base/compiler_support/platform_queries.hpp"
// IWYU pragma: friend "arene/base/compiler_support/platform_queries/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/preprocessor.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// NOLINTBEGIN(cppcoreguidelines-macro-usage) Explicitly providing compiler support macro functions
// parasoft-begin-suppress AUTOSAR-A16_0_1-d-2 "Conditional defines permitted by A16-0-1 Permit #2"
// parasoft-begin-suppress AUTOSAR-A16_0_1-a-2 "Conditional defines permitted by A16-0-1 Permit #2"

/// @def ARENE_STD_LIBRARY_STRING_CONTAINS
/// @brief A platform support query for testing if @c __cpp_lib_string_contains is defined.
///
/// Usage:
/// @code{c++}
/// #if ARENE_IS_ON(ARENE_STD_LIBRARY_STRING_CONTAINS)
/// @endcode
///
/// The value of this query is determined as follows, in order of precedence:
/// 1. If @c ARENE_STD_LIBRARY_STRING_CONTAINS has been explicitly defined via a @c -D flag to the compiler or similar:
///    * @c ARENE_ON if it is truthy
///    * @c ARENE_OFF if it is not truthy
/// 1. If @c __cpp_lib_string_contains is defined it is @c ARENE_ON_BY_DEFAULT
/// 1. Otherwise, it is @c ARENE_OFF_BY_DEFAULT

#if defined(ARENE_STD_LIBRARY_STRING_CONTAINS)
#if (ARENE_STD_LIBRARY_STRING_CONTAINS != 0)
#define ARENE_STD_LIBRARY_STRING_CONTAINS_I_ ARENE_ON
#else
#define ARENE_STD_LIBRARY_STRING_CONTAINS_I_ ARENE_ON
#endif
#elif defined(__cpp_lib_string_contains)
#define ARENE_STD_LIBRARY_STRING_CONTAINS_I_ ARENE_ON_BY_DEFAULT
#else
#define ARENE_STD_LIBRARY_STRING_CONTAINS_I_ ARENE_OFF_BY_DEFAULT
#endif

#ifdef ARENE_DOC_GENERATION_RUNNING  // make this appear when running doxygen, as it won't otherwise.
#ifndef ARENE_STD_LIBRARY_STRING_CONTAINS
#define ARENE_STD_LIBRARY_STRING_CONTAINS
#endif
#endif

// NOLINTEND(cppcoreguidelines-macro-usage) Explicitly providing compiler support macro functions
// parasoft-end-suppress AUTOSAR-A16_0_1-d-2
// parasoft-end-suppress AUTOSAR-A16_0_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PLATFORM_QUERIES_HAS_STRING_CONTAINS_HPP_

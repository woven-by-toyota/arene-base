// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PLATFORM_QUERIES_STDLIB_INFO_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PLATFORM_QUERIES_STDLIB_INFO_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/compiler_support/platform_queries/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

#include "arene/base/compiler_support/preprocessor.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

#include "arene/base/stdlib_choice/cstddef.hpp"
// IWYU pragma: no_include <__configuration/language.h>

// NOLINTBEGIN(cppcoreguidelines-macro-usage) Explicitly providing compiler support macro functions
// parasoft-begin-suppress AUTOSAR-A16_0_1-d-2 "Conditional defines permitted by A16-0-1 Permit #2"
// parasoft-begin-suppress AUTOSAR-A16_0_1-a-2 "Conditional defines permitted by A16-0-1 Permit #2"

/// @def ARENE_STDLIB_LIBCXX
/// @brief A platform support query for testing if the current stdlib is @c libc++ .
///
/// Usage:
/// @code{c++}
/// #if ARENE_IS_ON(ARENE_STDLIB_LIBCXX)
/// @endcode
///
/// The value of this query is determined as follows, in order of precedence:
/// 1. If @c ARENE_STDLIB_LIBCXX has been explicitly defined via a @c -D flag to the compiler or similar:
///    * @c ARENE_ON if it is truthy
///    * @c ARENE_OFF if it is not truthy
/// 1. If @c _LIBCPP_STD_VER is defined it is @c ARENE_ON_BY_DEFAULT
/// 1. Otherwise, it is @c ARENE_OFF_BY_DEFAULT

#if defined(ARENE_STDLIB_LIBCXX)
#if (ARENE_STDLIB_LIBCXX != 0)
#define ARENE_STDLIB_LIBCXX_I_ ARENE_ON
#else
#define ARENE_STDLIB_LIBCXX_I_ ARENE_OFF
#endif
#elif defined(_LIBCPP_STD_VER)
#define ARENE_STDLIB_LIBCXX_I_ ARENE_ON_BY_DEFAULT
#else
#define ARENE_STDLIB_LIBCXX_I_ ARENE_OFF_BY_DEFAULT
#endif

#ifdef ARENE_DOC_GENERATION_RUNNING  // make this appear when running doxygen, as it won't otherwise.
#ifndef ARENE_STDLIB_LIBCXX
#define ARENE_STDLIB_LIBCXX
#endif
#endif

/// @def ARENE_STDLIB_LIBSTDCXX
/// @brief A platform support query for testing if the current stdlib is @c libstdc++ .
///
/// Usage:
/// @code{c++}
/// #if ARENE_IS_ON(ARENE_STDLIB_LIBSTDCXX)
/// @endcode
///
/// The value of this query is determined as follows, in order of precedence:
/// 1. If @c ARENE_STDLIB_LIBSTDCXX has been explicitly defined via a @c -D flag to the compiler or similar:
///    * @c ARENE_ON if it is truthy
///    * @c ARENE_OFF if it is not truthy
/// 1. If @c __GLIBCXX__ or @c __GLIBCPP__ are defined it is @c ARENE_ON_BY_DEFAULT
/// 1. Otherwise, it is @c ARENE_OFF_BY_DEFAULT

#if defined(ARENE_STDLIB_LIBSTDCXX)
#if (ARENE_STDLIB_LIBSTDCXX != 0)
#define ARENE_STDLIB_LIBSTDCXX_I_ ARENE_ON
#else
#define ARENE_STDLIB_LIBSTDCXX_I_ ARENE_OFF
#endif
#elif defined(__GLIBCXX__) || defined(__GLIBCPP__)
#define ARENE_STDLIB_LIBSTDCXX_I_ ARENE_ON_BY_DEFAULT
#else
#define ARENE_STDLIB_LIBSTDCXX_I_ ARENE_OFF_BY_DEFAULT
#endif

#ifdef ARENE_DOC_GENERATION_RUNNING  // make this appear when running doxygen, as it won't otherwise.
#ifndef ARENE_STDLIB_LIBSTDCXX
#define ARENE_STDLIB_LIBSTDCXX
#endif
#endif

/// @def ARENE_STDLIB_LIBARENECXX
/// @brief A platform support query for testing if the current stdlib is the internal arene_base C++ standard library.
///
/// Usage:
/// @code{c++}
/// #if ARENE_IS_ON(ARENE_STDLIB_LIBARENECXX)
/// @endcode
///
/// The value of this query is determined as follows, in order of precedence:
/// 1. If @c ARENE_STDLIB_LIBARENECXX has been explicitly defined via a @c -D flag to the compiler or similar:
///    * @c ARENE_ON if it is truthy
///    * @c ARENE_OFF if it is not truthy
/// 1. If the inclusion of @c <cstddef> results in detection of internal stdlib defines known to be in that header, it
/// is @c ARENE_ON_BY_DEFAULT
/// 1. Otherwise, it is @c ARENE_OFF_BY_DEFAULT

#if defined(ARENE_STDLIB_LIBARENECXX)
#if (ARENE_STDLIB_LIBARENECXX != 0)
#define ARENE_STDLIB_LIBARENECXX_I_ ARENE_ON
#else
#define ARENE_STDLIB_LIBARENECXX_I_ ARENE_OFF
#endif
#elif defined(INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_CSTDDEF_HPP_)
#define ARENE_STDLIB_LIBARENECXX_I_ ARENE_ON_BY_DEFAULT
#else
#define ARENE_STDLIB_LIBARENECXX_I_ ARENE_OFF_BY_DEFAULT
#endif

#ifdef ARENE_DOC_GENERATION_RUNNING  // make this appear when running doxygen, as it won't otherwise.
#ifndef ARENE_STDLIB_LIBARENECXX
#define ARENE_STDLIB_LIBARENECXX
#endif
#endif

// NOLINTEND(cppcoreguidelines-macro-usage) Explicitly providing compiler support macro functions
// parasoft-end-suppress AUTOSAR-A16_0_1-d-2
// parasoft-end-suppress AUTOSAR-A16_0_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PLATFORM_QUERIES_STDLIB_INFO_HPP_

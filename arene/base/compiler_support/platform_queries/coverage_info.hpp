// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PLATFORM_QUERIES_COVERAGE_INFO_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PLATFORM_QUERIES_COVERAGE_INFO_HPP_

// IWYU pragma: private, include "arene/base/compiler_support/platform_queries.hpp"
// IWYU pragma: friend "arene/base/compiler_support/platform_queries/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/preprocessor.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-A16_0_1-d-2 "Conditional defines permitted by A16-0-1 Permit #2"
// parasoft-begin-suppress AUTOSAR-A16_0_1-a-2 "Conditional defines permitted by A16-0-1 Permit #2"

/// @def ARENE_IS_GCOV
/// @brief A platform support query for testing if the current compilation is happening under gcov.
///
/// Usage:
/// @code{c++}
/// #if ARENE_IS_ON(ARENE_IS_GCOV)
/// @endcode
///
/// The value of this query is determined as follows, in order of precedence:
/// 1. If @c ARENE_IS_GCOV has been explicitly defined via a @c -D flag to the compiler or similar:
///    * @c ARENE_ON if it is truthy
///    * @c ARENE_OFF if it is not truthy
/// 1. If @c __GCOV__ is defined it is @c ARENE_ON_BY_DEFAULT
/// 1. Otherwise, it is @c ARENE_OFF_BY_DEFAULT
#if defined(ARENE_IS_GCOV)
#if (ARENE_IS_GCOV != 0)
#define ARENE_IS_GCOV_I_ ARENE_ON
#else
#define ARENE_IS_GCOV_I_ ARENE_OFF
#endif
#elif defined(__GCOV__)
#define ARENE_IS_GCOV_I_ ARENE_ON_BY_DEFAULT
#else
#define ARENE_IS_GCOV_I_ ARENE_OFF_BY_DEFAULT
#endif

#ifdef ARENE_DOC_GENERATION_RUNNING  // make this appear when running doxygen, as it won't otherwise.
#ifndef ARENE_IS_GCOV
#define ARENE_IS_GCOV
#endif
#endif

/// @def ARENE_IS_LLVMCOV
/// @brief A platform support query for testing if the current compilation is happening under LLVMCov.
///
/// Usage:
/// @code{c++}
/// #if ARENE_IS_ON(ARENE_IS_LLVMCOV)
/// @endcode
///
/// The value of this query is determined as follows, in order of precedence:
/// 1. If @c ARENE_IS_LLVMCOV has been explicitly defined via a @c -D flag to the compiler or similar:
///    * @c ARENE_ON if it is truthy
///    * @c ARENE_OFF if it is not truthy
/// 1. If @c __LLVM_INSTR_PROFILE_GENERATE is defined it is @c ARENE_ON_BY_DEFAULT
#if defined(ARENE_IS_LLVMCOV)
#if (ARENE_IS_LLVMCOV != 0)
#define ARENE_IS_LLVMCOV_I_ ARENE_ON
#else
#define ARENE_IS_LLVMCOV_I_ ARENE_OFF
#endif
#elif defined(__LLVM_INSTR_PROFILE_GENERATE)
#define ARENE_IS_LLVMCOV_I_ ARENE_ON_BY_DEFAULT
#else
#define ARENE_IS_LLVMCOV_I_ ARENE_OFF_BY_DEFAULT
#endif

#ifdef ARENE_DOC_GENERATION_RUNNING  // make this appear when running doxygen, as it won't otherwise.
#ifndef ARENE_IS_LLVMCOV
#define ARENE_IS_LLVMCOV
#endif
#endif

/// @def ARENE_IS_COVERAGE_COMPILATION
/// @brief A platform support query for testing if the current compilation is happening under a coverage tool such as
/// gcov or LLVMCov.
///
/// Usage:
/// @code{c++}
/// #if ARENE_IS_ON(ARENE_IS_COVERAGE_COMPILATION)
/// @endcode
///
/// The value of this query is determined as follows, in order of precedence:
/// 1. If @c ARENE_IS_COVERAGE_COMPILATION has been explicitly defined via a @c -D flag to the compiler or similar:
///    * @c ARENE_ON if it is truthy
///    * @c ARENE_OFF if it is not truthy
/// 1. If either @c ARENE_IS_ON(ARENE_IS_GCOV) or @c ARENE_IS_ON(ARENE_IS_LLVMCOV) returns @c true it is
///    @c ARENE_ON_BY_DEFAULT
#if defined(ARENE_IS_COVERAGE_COMPILATION)
#if (ARENE_IS_COVERAGE_COMPILATION != 0)
#define ARENE_IS_COVERAGE_COMPILATION_I_ ARENE_ON
#else
#define ARENE_IS_COVERAGE_COMPILATION_I_ ARENE_OFF
#endif
#elif defined(COVERAGE_ENABLED)
#if (COVERAGE_ENABLED != 0)
#define ARENE_IS_COVERAGE_COMPILATION_I_ ARENE_ON
#else
#define ARENE_IS_COVERAGE_COMPILATION_I_ ARENE_OFF
#endif
// parasoft-begin-suppress AUTOSAR-M16_0_7-a-2 "False positive: not used as a macro"
#elif ARENE_IS_ON(ARENE_IS_GCOV) || ARENE_IS_ON(ARENE_IS_LLVMCOV)
// parasoft-end-suppress AUTOSAR-M16_0_7-a-2
#define ARENE_IS_COVERAGE_COMPILATION_I_ ARENE_ON_BY_DEFAULT
#else
#define ARENE_IS_COVERAGE_COMPILATION_I_ ARENE_OFF_BY_DEFAULT
#endif

#ifdef ARENE_DOC_GENERATION_RUNNING  // make this appear when running doxygen, as it won't otherwise.
#ifndef ARENE_IS_COVERAGE_COMPILATION
#define ARENE_IS_COVERAGE_COMPILATION
#endif
#endif

// parasoft-end-suppress AUTOSAR-A16_0_1-d-2
// parasoft-end-suppress AUTOSAR-A16_0_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PLATFORM_QUERIES_COVERAGE_INFO_HPP_

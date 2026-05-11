// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file static_assertions_config.hpp
/// @brief Compile time configuration for the static assertions macros
///

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_GTEST_STATIC_ASSERTIONS_CONFIG_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_GTEST_STATIC_ASSERTIONS_CONFIG_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/testing/gtest/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/preprocessor.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-A16_0_1-d-2 "Defines macros for configuration"
// parasoft-begin-suppress AUTOSAR-A16_0_1-a-2 "Conditional defines for tool support and configuration detection"

/// @def ARENE_GTEST_STATIC_ASSERTIONS
/// @brief A compile time configuration query for testing if static assertion macros should actually static assert
///
/// Usage:
/// @code{c++}
/// #if ARENE_IS_ON(ARENE_GTEST_STATIC_ASSERTIONS)
/// @endcode
///
/// The value of this query is determined as follows, in order of precedence:
/// 1. If @c ARENE_GTEST_STATIC_ASSERTIONS has been explicitly defined via a @c -D flag to the compiler or similar:
///    * @c ARENE_ON if it is truthy
///    * @c ARENE_OFF if it is not truthy
/// 1. Otherwise, it is @c ARENE_ON_BY_DEFAULT
#if defined(ARENE_GTEST_STATIC_ASSERTIONS)
#if (ARENE_GTEST_STATIC_ASSERTIONS != 0)
#define ARENE_GTEST_STATIC_ASSERTIONS_I_ ARENE_ON
#else
#define ARENE_GTEST_STATIC_ASSERTIONS_I_ ARENE_OFF
#endif
#else
#define ARENE_GTEST_STATIC_ASSERTIONS_I_ ARENE_ON_BY_DEFAULT
#endif

#ifdef ARENE_DOC_GENERATION_RUNNING  // make this appear when running doxygen, as it won't otherwise.
#ifndef ARENE_GTEST_STATIC_ASSERTIONS
#define ARENE_GTEST_STATIC_ASSERTIONS
#endif
#endif

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_GTEST_STATIC_ASSERTIONS_CONFIG_HPP_

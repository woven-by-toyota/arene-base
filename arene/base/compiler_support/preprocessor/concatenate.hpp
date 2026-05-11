// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PREPROCESSOR_CONCATENATE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PREPROCESSOR_CONCATENATE_HPP_

// IWYU pragma: private, include "arene/base/compiler_support/preprocessor.hpp"

// AUTOSAR exceptions:
// A16-0-1 The pre-processor shall only be used for unconditional and
// conditional file inclusion and include guards, and using the following
// directives: (1) #ifndef, (2) #ifdef, (3) #if, (4) #if defined, (5) #elif, (6)
// #else, (7) #define, (8) #endif, (9) #include.
//
// Exception Rationale: The code below is used to concatenate __FILE__ and __LINE__ with other tokens, which is needed
// to generate unique identifiers for macro-based libraries like Google Test.

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
// parasoft-begin-suppress AUTOSAR-A16_0_1-d-2 "Function-like macro permitted by A16-0-1 Permit #1"
// parasoft-begin-suppress CERT_C-PRE30-a "Token concatenation is necessary because Google Test does it too"
// parasoft-begin-suppress AUTOSAR-M16_0_6-a "The purpose of these macros is to concatenate tokens, not generate code"

/// @brief Concatenate two preprocessor tokens together.
/// This has to be nested inside another macro in order to avoid the ## taking precedence over evaluating __LINE__.
/// @param left The left token to concatenate
/// @param right The right token to concatenate
#define ARENE_CONCATENATE_IMPL(left, right) left##right

/// @brief Concatenate two tokens together, similar to ## except that arguments are replaced before concatenation.
/// @param left The left token to concatenate
/// @param right The right token to concatenate
#define ARENE_CONCATENATE(left, right) ARENE_CONCATENATE_IMPL(left, right)

// parasoft-end-suppress AUTOSAR-M16_0_6-a
// parasoft-end-suppress CERT_C-PRE30-a
// parasoft-end-suppress AUTOSAR-A16_0_1-d-2
// NOLINTEND(cppcoreguidelines-macro-usage)

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PREPROCESSOR_CONCATENATE_HPP_

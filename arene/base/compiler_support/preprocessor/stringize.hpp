// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PREPROCESSOR_STRINGIZE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PREPROCESSOR_STRINGIZE_HPP_

// IWYU pragma: private, include "arene/base/compiler_support/preprocessor.hpp"

// AUTOSAR exceptions:
// A16-0-1 The pre-processor shall only be used for unconditional and
// conditional file inclusion and include guards, and using the following
// directives: (1) #ifndef, (2) #ifdef, (3) #if, (4) #if defined, (5) #elif, (6)
// #else, (7) #define, (8) #endif, (9) #include.
//
// Exception Rationale: The code below is used to safely convert a macro
// argument to a string literal, for use in supporting other basic facilities
// such as the contract checking.

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
// parasoft-begin-suppress AUTOSAR-A16_0_1-d-2 "Function-like macro permitted by A16-0-1 Permit #1"

/// Internal macro for actually converting a macro argument to a string literal.
/// DO NOT CALL DIRECTLY.
/// @param ... the macro argument to convert.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ARENE_STRINGIZE_INTERNAL(...) #__VA_ARGS__
/// Macro for actually converting a macro argument to a string literal.
/// @param ... the macro argument to convert.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ARENE_STRINGIZE(...) ARENE_STRINGIZE_INTERNAL(__VA_ARGS__)

// NOLINTEND(cppcoreguidelines-macro-usage)
// parasoft-end-suppress AUTOSAR-A16_0_1-d-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PREPROCESSOR_STRINGIZE_HPP_

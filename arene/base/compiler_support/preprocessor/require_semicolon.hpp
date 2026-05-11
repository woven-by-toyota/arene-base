// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PREPROCESSOR_REQUIRE_SEMICOLON_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PREPROCESSOR_REQUIRE_SEMICOLON_HPP_

// IWYU pragma: private, include "arene/base/compiler_support/preprocessor.hpp"
// IWYU pragma: friend "arene/base/compiler_support/preprocessor/.*"

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
// parasoft-begin-suppress AUTOSAR-A16_0_1-d-2 "Function-like macro permitted by A16-0-1 Permit #1"

// Require use of a semicolon at the end of the macro "function".
#define ARENE_REQUIRE_SEMICOLON static_assert(true, "")

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PREPROCESSOR_REQUIRE_SEMICOLON_HPP_

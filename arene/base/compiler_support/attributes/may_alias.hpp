// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_ATTRIBUTES_MAY_ALIAS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_ATTRIBUTES_MAY_ALIAS_HPP_

// IWYU pragma: private, include "arene/base/compiler_support/attributes.hpp"

// NOLINTBEGIN(cppcoreguidelines-macro-usage) Explicitly providing compiler support macro functions
// parasoft-begin-suppress AUTOSAR-A16_0_1-d-2 "Conditional defines permitted by A16-0-1 Permit #2"
// parasoft-begin-suppress AUTOSAR-A16_0_1-a-2 "Conditional defines permitted by A16-0-1 Permit #2"

/// @def ARENE_MAY_ALIAS
/// @brief An alias for the @c __may_alias__ attribute, or a no-op if the attribute is not supported

#if defined(__clang__) || defined(__GNUC__)
#define ARENE_MAY_ALIAS __attribute__((__may_alias__))
#else
#define ARENE_MAY_ALIAS
#endif

// NOLINTEND(cppcoreguidelines-macro-usage) Explicitly providing compiler support macro functions
// parasoft-end-suppress AUTOSAR-A16_0_1-d-2
// parasoft-end-suppress AUTOSAR-A16_0_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_ATTRIBUTES_MAY_ALIAS_HPP_

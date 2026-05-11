// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_ATTRIBUTES_NORETURN_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_ATTRIBUTES_NORETURN_HPP_

// IWYU pragma: private, include "arene/base/compiler_support/attributes.hpp"

#include "arene/base/compiler_support/attributes/has_attribute.hpp"

// NOLINTBEGIN(cppcoreguidelines-macro-usage) Explicitly providing compiler support macro functions
// parasoft-begin-suppress AUTOSAR-A16_0_1-d-2 "Conditional defines permitted by A16-0-1 Permit #2"
// parasoft-begin-suppress AUTOSAR-A16_0_1-a-2 "Conditional defines permitted by A16-0-1 Permit #2"

/// @def ARENE_NORETURN
/// @brief An alias for @c [[noreturn]] , or a no-op if the attribute is not supported

// parasoft-begin-suppress AUTOSAR-M16_0_7-a-2 "False positive: not used as a macro"
#if ARENE_HAS_STD_ATTRIBUTE(noreturn)
#define ARENE_NORETURN [[noreturn]]
#else
#define ARENE_NORETURN
#endif
// parasoft-end-suppress AUTOSAR-M16_0_7-a-2

// NOLINTEND(cppcoreguidelines-macro-usage) Explicitly providing compiler support macro functions
// parasoft-end-suppress AUTOSAR-A16_0_1-d-2
// parasoft-end-suppress AUTOSAR-A16_0_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_ATTRIBUTES_NORETURN_HPP_

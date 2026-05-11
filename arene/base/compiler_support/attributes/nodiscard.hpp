// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_ATTRIBUTES_NODISCARD_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_ATTRIBUTES_NODISCARD_HPP_

// IWYU pragma: private, include "arene/base/compiler_support/attributes.hpp"

#include "arene/base/compiler_support/attributes/has_attribute.hpp"

// NOLINTBEGIN(cppcoreguidelines-macro-usage) Explicitly providing compiler support macro functions
// parasoft-begin-suppress AUTOSAR-A16_0_1-d-2 "Conditional defines permitted by A16-0-1 Permit #2"
// parasoft-begin-suppress AUTOSAR-A16_0_1-a-2 "Conditional defines permitted by A16-0-1 Permit #2"

/// @def ARENE_NODISCARD
/// @brief An alias for @c [[nodiscard]] , or a no-op if the attribute is not supported

// parasoft-begin-suppress AUTOSAR-M16_0_7-a-2 "False positive: not used as a macro"
#if defined(__clang__) && ARENE_HAS_STD_ATTRIBUTE(gnu::warn_unused_result)
#define ARENE_NODISCARD [[gnu::warn_unused_result]]
#elif ARENE_HAS_STD_ATTRIBUTE(nodiscard) >= 201603L
#define ARENE_NODISCARD [[nodiscard]]
#else
#define ARENE_NODISCARD
#endif
// parasoft-end-suppress AUTOSAR-M16_0_7-a-2

/// @def ARENE_NODISCARD_WITH
/// @brief An alias for @c [[nodiscard(...)]] , or a no-op if the attribute is not supported

// parasoft-begin-suppress AUTOSAR-M16_0_7-a-2 "False positive: not used as a macro"
#if defined(__clang__) && ARENE_HAS_STD_ATTRIBUTE(gnu::warn_unused_result) >= 201907L
#define ARENE_NODISCARD_WITH(...) [[gnu::warn_unused_result(__VA_ARGS__)]]
#elif ARENE_HAS_STD_ATTRIBUTE(nodiscard) >= 201907L
#define ARENE_NODISCARD_WITH(...) [[nodiscard(__VA_ARGS__)]]
#else
#define ARENE_NODISCARD_WITH(...) ARENE_NODISCARD
#endif
// parasoft-end-suppress AUTOSAR-M16_0_7-a-2

// NOLINTEND(cppcoreguidelines-macro-usage) Explicitly providing compiler support macro functions
// parasoft-end-suppress AUTOSAR-A16_0_1-d-2
// parasoft-end-suppress AUTOSAR-A16_0_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_ATTRIBUTES_NODISCARD_HPP_

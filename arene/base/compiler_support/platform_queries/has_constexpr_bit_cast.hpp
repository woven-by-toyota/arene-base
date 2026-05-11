// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PLATFORM_QUERIES_HAS_CONSTEXPR_BIT_CAST_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PLATFORM_QUERIES_HAS_CONSTEXPR_BIT_CAST_HPP_

// IWYU pragma: private, include "arene/base/compiler_support/platform_queries.hpp"
// IWYU pragma: friend "arene/base/compiler_support/platform_queries/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/platform_queries/compiler_info.hpp"  // IWYU pragma: keep
#include "arene/base/compiler_support/platform_queries/has_builtin.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// NOLINTBEGIN(cppcoreguidelines-macro-usage) Explicitly providing compiler support macro functions
// parasoft-begin-suppress AUTOSAR-A16_0_1-d-2 "Conditional defines permitted by A16-0-1 Permit #2"
// parasoft-begin-suppress AUTOSAR-A16_0_1-a-2 "Conditional defines permitted by A16-0-1 Permit #2"

/// @def ARENE_HAS_CONSTEXPR_BIT_CAST
/// @brief A platform support query indicating whether @c arene::base::bit_cast
/// can be evaluated in a constant expression.
///
/// @c arene::base::bit_cast is @c constexpr only when the compiler provides
/// @c __builtin_bit_cast . On toolchains without it (e.g. GCC 8), the
/// implementation falls back to @c std::memcpy , which is not usable in a
/// constant expression in C++14.
///
/// Usage:
/// @code{c++}
/// #if ARENE_IS_ON(ARENE_HAS_CONSTEXPR_BIT_CAST)
///     constexpr auto value = arene::base::bit_cast<float>(pattern);
/// #endif
/// @endcode
///
/// The value of this query is determined as follows, in order of precedence:
/// 1. If @c __builtin_bit_cast is defined, it is @c ARENE_ON_BY_DEFAULT
/// 1. Otherwise, it is @c ARENE_OFF_BY_DEFAULT

// parasoft-begin-suppress AUTOSAR-M16_0_7-a-2 "False positive: not used as a macro"
#if ARENE_HAS_BUILTIN(__builtin_bit_cast)
// parasoft-end-suppress AUTOSAR-M16_0_7-a-2
#define ARENE_HAS_CONSTEXPR_BIT_CAST_I_ ARENE_ON_BY_DEFAULT
#else
#define ARENE_HAS_CONSTEXPR_BIT_CAST_I_ ARENE_OFF_BY_DEFAULT
#endif

#ifdef ARENE_DOC_GENERATION_RUNNING  // make this appear when running doxygen, as it won't otherwise.
#ifndef ARENE_HAS_CONSTEXPR_BIT_CAST
#define ARENE_HAS_CONSTEXPR_BIT_CAST
#endif
#endif

// NOLINTEND(cppcoreguidelines-macro-usage)
// parasoft-end-suppress AUTOSAR-A16_0_1-d-2
// parasoft-end-suppress AUTOSAR-A16_0_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PLATFORM_QUERIES_HAS_CONSTEXPR_BIT_CAST_HPP_

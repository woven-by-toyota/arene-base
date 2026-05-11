// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_IS_POSSIBLY_CONSTANT_EVALUATED_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_IS_POSSIBLY_CONSTANT_EVALUATED_HPP_

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
// parasoft-begin-suppress AUTOSAR-A16_0_1-a-2 "Conditional define permitted by A16-0-1 Permit #2"
// parasoft-begin-suppress AUTOSAR-A16_0_1-d-2 "Conditional define permitted by A16-0-1 Permit #2"

#if ARENE_IS_ON(ARENE_HAS_BUILTIN_IS_CONSTANT_EVALUATED)
// Call into the compiler intrinsic to detect this.
#define ARENE_IS_POSSIBLY_CONSTANT_EVALUATED_RETURN_EXPRESSION __builtin_is_constant_evaluated()
#else
// We can't detect if we are in a constant evaluation context so
// conservatively return yes (since it's "possible" we are).
#define ARENE_IS_POSSIBLY_CONSTANT_EVALUATED_RETURN_EXPRESSION true
#endif

namespace arene {
namespace base {

/// @brief Query to determine if the caller may be in a constant-evaluated-context.
/// @return true if the call _may_ be in a constant-evaluated context.
/// @return false If the call is manifestly not in a constant-evaluated context.
///
/// This trait can be used to provide both constexpr and non-constexpr specializations of an implementation when there
/// is a performance optimization that can only be performed in non-contexpr contexts.
///
/// @note This API is similar, but not identical to,
///   C++20's @c [std::is_constant_evaluated](https://en.cppreference.com/w/cpp/types/is_constant_evaluated) .
///   Determining if the current execution context is constant-evaluated requires compiler support through an intrinsic
///   added specifically to allow the implementation of @c std::is_constant_evaluated . Therefor on compilers lacking
///   these intrinsics, it is not possible to determine if the current context is constant-evaluated. As such, the name
///   has been changed to reflex the loosened semantics; while @c is_constant_evaluated returns @c true IFF the current
///   context is a constant evaluated context, @c is_possible_constant_evaluated returns @c fase IFF the current context
///   is **not** a constant evaluated context. Relaxing this constraint allows code to be written in pre-C++20 contexts
///   to perform this query, and if the compiler version supports it, benefit, while always safely falling back to the
///   @c constexpr compatible implementation when compiled with compilers which do not support the intrinsic.
constexpr auto is_possibly_constant_evaluated() noexcept -> bool {
  return ARENE_IS_POSSIBLY_CONSTANT_EVALUATED_RETURN_EXPRESSION;
}
}  // namespace base
}  // namespace arene

// NOLINTEND(cppcoreguidelines-macro-usage)
// parasoft-end-suppress AUTOSAR-A16_0_1-a-2
// parasoft-end-suppress AUTOSAR-A16_0_1-d-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_IS_POSSIBLY_CONSTANT_EVALUATED_HPP_

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file diagnostics.hpp
/// @brief Provides macros used to disable and enable compiler specific diagnostics using the _Pragma operator
///

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_DIAGNOSTICS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_DIAGNOSTICS_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
// parasoft-begin-suppress AUTOSAR-A16_0_1-d-2 "Indirect call of _Pragma permitted by A16-0-1 Permit #1"
// parasoft-begin-suppress AUTOSAR-A16_0_1-a-2 "Conditional defines permitted by A16-0-1 Permit #2"

/// @def ARENE_IGNORE_CLANG(s, comment)
/// @brief Pushes an ignore for a diagnostic into the compiler for @c clang only.
/// @param s The diagnostic to ignore
/// @param comment The justification for the diagnostic suppression. Is discarded in the compiled output.
/// @pre @c ARENE_IGNORE_START() has been called.
/// @pre The given diagnostic is a valid diagnostic for @c clang , else it will be a compiler error when evaluated by
///      @c clang.
/// @post The given diagnostic is suppressed when the active compiler is @c clang.

/// @def ARENE_IGNORE_ARMCLANG(s, comment)
/// @brief Pushes an ignore for a diagnostic into the compiler for @c armclang only.
/// @param s The diagnostic to ignore
/// @param comment The justification for the diagnostic suppression. Is discarded in the compiled output.
/// @pre @c ARENE_IGNORE_START() has been called.
/// @pre The given diagnostic is a valid diagnostic for @c armclang , else it will be a compiler error when evaluated by
///      @c armclang.
/// @post The given diagnostic is suppressed when the active compiler is @c armclang.

/// @def ARENE_IGNORE_GCC(s, comment)
/// @brief Pushes an ignore for a diagnostic into the compiler for @c gcc only (which includes @c qcc).
/// @param s The diagnostic to ignore
/// @param comment The justification for the diagnostic suppression. Is discarded in the compiled output.
/// @pre @c ARENE_IGNORE_START() has been called.
/// @pre The given diagnostic is a valid diagnostic for @c gcc, else it will be a compiler error when evaluated by
///      @c gcc.
/// @post The given diagnostic is suppressed when the active compiler is @c gcc.

/// @def ARENE_IGNORE_ALL(s, comment)
/// @brief Pushes an ignore for a diagnostic into the compiler for any compiler.
/// @param s The diagnostic to ignore
/// @param comment The justification for the diagnostic suppression. Is discarded in the compiled output.
/// @pre @c ARENE_IGNORE_START() has been called.
/// @pre The given diagnostic is a valid diagnostic for all compilers, else it may be a compiler error when evaluated by
///      an unsupported compiler.
/// @post The given diagnostic is suppressed.

/// @def ARENE_IGNORE_WSELFMOVE(comment)
/// @brief Pushes an ignore for the self-move diagnostic for applicable compilers.
/// @param comment The justification for suppressing the self-move diagnostic.
/// @pre @c ARENE_IGNORE_START() has been called.
/// @post The self-move diagnostic will not be issued.

// To extend support to a new compiler, one must:
// 1. Define ARENE_IGNORE_IMPL(s) for the compiler
// 2. Define ARENE_IGNORE_[COMPILER](s, comment) as ARENE_IGNORE_IMPL(s)
// 3. Define ARENE_IGNORE_ALL(s, comment) as ARENE_IGNORE_[COMPILER](s, (comment))
// 4. Define ARENE_IGNORE_W[DIAGNOSTIC](comment) as ARENE_IGNORE_[COMPILER]("-W[diagnostic]", (comment)) if applicable
// or a no-op otherwise.
//    Supported diagnostics:
//      - self-move
// 5. Define ARENE_IGNORE_[COMPILER](s, comment) as a no-op for all other compilers
// These defines must be placed within a block that is only evaluated for the respective compiler (i.e., the new
// compiler for steps 1-4, the existing compiler(s) for step 5).

#if ARENE_IS_ON(ARENE_COMPILER_ARMCLANG)
#define ARENE_IGNORE_IMPL(s) _Pragma(ARENE_STRINGIZE(clang diagnostic s)) ARENE_REQUIRE_SEMICOLON
#define ARENE_IGNORE_ARMCLANG(s, comment) ARENE_IGNORE_IMPL(ignored s)
// parasoft-begin-suppress AUTOSAR-M16_0_6-a-2 "'s' is a parameter to ## operator"
#define ARENE_IGNORE_ALL(s, comment) ARENE_IGNORE_ARMCLANG(s, (comment))
// parasoft-end-suppress AUTOSAR-M16_0_6-a-2
#define ARENE_IGNORE_WSELFMOVE(comment) ARENE_IGNORE_ARMCLANG("-Wself-move", (comment))
#elif ARENE_IS_ON(ARENE_COMPILER_CLANG)
#define ARENE_IGNORE_IMPL(s) _Pragma(ARENE_STRINGIZE(clang diagnostic s)) ARENE_REQUIRE_SEMICOLON
#define ARENE_IGNORE_CLANG(s, comment) ARENE_IGNORE_IMPL(ignored s)
// parasoft-begin-suppress AUTOSAR-M16_0_6-a-2 "'s' is a parameter to ## operator"
#define ARENE_IGNORE_ALL(s, comment) ARENE_IGNORE_CLANG(s, (comment))
// parasoft-end-suppress AUTOSAR-M16_0_6-a-2
#define ARENE_IGNORE_WSELFMOVE(comment) ARENE_IGNORE_CLANG("-Wself-move", (comment))

#elif ARENE_IS_ON(ARENE_COMPILER_GCC)
#define ARENE_IGNORE_IMPL(s) _Pragma(ARENE_STRINGIZE(GCC diagnostic s)) ARENE_REQUIRE_SEMICOLON
#define ARENE_IGNORE_GCC(s, comment) ARENE_IGNORE_IMPL(ignored s)
// parasoft-begin-suppress AUTOSAR-M16_0_6-a-2 "'s' is a parameter to ## operator"
#define ARENE_IGNORE_ALL(s, comment) ARENE_IGNORE_GCC(s, (comment))
// parasoft-end-suppress AUTOSAR-M16_0_6-a-2
#if __GNUC__ >= 13
#define ARENE_IGNORE_WSELFMOVE(comment) ARENE_IGNORE_GCC("-Wself-move", (comment))
#else  // __GNUC__ < 13
#define ARENE_IGNORE_WSELFMOVE(comment) ARENE_REQUIRE_SEMICOLON
#endif

#else
/* to be extended for other compilers */
static_assert(false, "Unsupported compiler");
#endif  // Active Compiler Macro Selection

// we must still define all the compiler-specific extension points for the not-selected compiler as no-ops so they are
// still valid macro calls
#ifndef ARENE_IGNORE_GCC
#define ARENE_IGNORE_GCC(s, comment) ARENE_REQUIRE_SEMICOLON
#endif  // ARENE_IGNORE_GCC
#ifndef ARENE_IGNORE_CLANG
#define ARENE_IGNORE_CLANG(s, comment) ARENE_REQUIRE_SEMICOLON
#endif  // ARENE_IGNORE_CLANG
#ifndef ARENE_IGNORE_ARMCLANG
#define ARENE_IGNORE_ARMCLANG(s, comment) ARENE_REQUIRE_SEMICOLON
#endif  // ARENE_IGNORE_ARMCLANG
#ifndef ARENE_IGNORE_ALL
#define ARENE_IGNORE_ALL(s, comment) ARENE_REQUIRE_SEMICOLON
#endif  // ARENE_IGNORE_ALL

/// @brief Begins a region of a suppressed diagnostic
/// @note There must be a call to @c ARENE_IGNORE_END() for each call to @c ARENE_IGNORE_START() .
#define ARENE_IGNORE_START() ARENE_IGNORE_IMPL(push)

/// @brief Ends a region of a suppressed diagnostic
/// @pre There was a call to @c ARENE_IGNORE_START() and one of @c ARENE_IGNORE_CLANG , @c ARENE_IGNORE_GCC ,
///      or @c ARENE_IGNORE_ALL
#define ARENE_IGNORE_END() ARENE_IGNORE_IMPL(pop)

// NOLINTEND(cppcoreguidelines-macro-usage)
// parasoft-end-suppress AUTOSAR-A16_0_1-d-2
// parasoft-end-suppress AUTOSAR-A16_0_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_DIAGNOSTICS_HPP_

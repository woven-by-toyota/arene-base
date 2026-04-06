// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PREPROCESSOR_DEFINE_CONFIGURATION_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PREPROCESSOR_DEFINE_CONFIGURATION_HPP_

// IWYU pragma: private, include "arene/base/compiler_support/preprocessor.hpp"

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
// parasoft-begin-suppress AUTOSAR-A16_0_1-d-2 "Function-like macros permitted by A16-0-1 Permit #1"
// parasoft-begin-suppress AUTOSAR-A16_0_1-a-2 "Function-like macros permitted by A16-0-1 Permit #1"
// parasoft-begin-suppress AUTOSAR-M16_0_6-a-2 "OP_SYMBOL needs to be used without parentheses in order to work
// correctly"
// parasoft-begin-suppress AUTOSAR-M16_3_1-a-2 "Token pasting ## is used to provide a configuration macro
// system"

/// @brief Evaluates if the given compile time configuration symbol is "on"
/// @param OP_SYMBOL The symbol to evaluate, unmodified.
/// @return true if the input symbol was defined as either @c ARENE_ON or @c ARENE_ON_BY_DEFAULT
/// @return false otherwise
/// @pre The symbol must be defined, and it must only be defined as one of @c ARENE_ON , @c ARENE_OFF ,
///      @c ARENE_ON_BY_DEFAULT , or @c ARENE_OFF_BY_DEFAULT  else behavior is undefined.
/// @note This is generally not used directly in user code, prefer @c ARENE_IS_ON instead.
#define ARENE_IS_ON_RAW(OP_SYMBOL) ((3U OP_SYMBOL 3U) != 0U)  // NOLINT(bugprone-macro-parentheses)

/// @brief Evaluates if the given compile time configuration symbol is "on"
/// @param OP_SYMBOL The "query name" of symbol to evaluate. It will have @c _I_ concatenated to it before
///            evaluation.
/// @return true if the modified input symbol was defined as either @c ARENE_ON or @c ARENE_ON_BY_DEFAULT
/// @return false otherwise
/// @pre The modified symbol must be defined, and it must only be defined as one of @c ARENE_ON , @c ARENE_OFF ,
///      @c ARENE_ON_BY_DEFAULT , or @c ARENE_OFF_BY_DEFAULT  else behavior is undefined.
#define ARENE_IS_ON(OP_SYMBOL) ARENE_IS_ON_RAW(OP_SYMBOL##_I_)  // NOLINT(bugprone-macro-parentheses)

/// @brief Evaluates if the given compile time configuration symbol is "off"
/// @param OP_SYMBOL The symbol to evaluate, unmodified.
/// @return true if the input symbol was defined as either @c ARENE_OFF or @c ARENE_OFF_BY_DEFAULT
/// @return false otherwise
/// @pre The symbol must be defined, and it must only be defined as one of @c ARENE_ON , @c ARENE_OFF ,
///      @c ARENE_ON_BY_DEFAULT , or @c ARENE_OFF_BY_DEFAULT  else behavior is undefined.
/// @note This is generally not used directly in user code, prefer @c ARENE_IS_OFF instead.
#define ARENE_IS_OFF_RAW(OP_SYMBOL) ((3U OP_SYMBOL 3U) == 0U)  // NOLINT(bugprone-macro-parentheses)

/// @brief Evaluates if the given compile time configuration symbol is "off"
/// @param OP_SYMBOL The "query name" of symbol to evaluate. It will have @c _I_ concatenated to it before
///            evaluation.
/// @return true if the modified input symbol was defined as either @c ARENE_OFF or @c ARENE_OFF_BY_DEFAULT
/// @return false otherwise
/// @pre The modified symbol must be defined, and it must only be defined as one of @c ARENE_ON , @c ARENE_OFF ,
///      @c ARENE_ON_BY_DEFAULT , or @c ARENE_OFF_BY_DEFAULT  else behavior is undefined.
#define ARENE_IS_OFF(OP_SYMBOL) ARENE_IS_OFF_RAW(OP_SYMBOL##_I_)  // NOLINT(bugprone-macro-parentheses)

/// @brief Evaluates if the given compile time configuration symbol is "on by default."
/// @param OP_SYMBOL The symbol to evaluate, unmodified.
/// @return true if the input symbol was defined as @c ARENE_ON_BY_DEFAULT.
/// @return false otherwise.
/// @pre The symbol must be defined, and it must only be defined as one of @c ARENE_ON , @c ARENE_OFF ,
///      @c ARENE_ON_BY_DEFAULT , or @c ARENE_OFF_BY_DEFAULT  else behavior is undefined.
/// @note This is generally not used directly in user code, prefer @c ARENE_IS_ON_BY_DEFAULT instead.
#define ARENE_IS_ON_BY_DEFAULT_RAW(OP_SYMBOL) ((3U OP_SYMBOL 3U) > 3U)  // NOLINT(bugprone-macro-parentheses)

/// @brief Evaluates if the given compile time configuration symbol is "on by default."
/// @param OP_SYMBOL The "query name" of symbol to evaluate. It will have @c _I_ concatenated to it before
///            evaluation.
/// @return true if the modified input symbol was defined as @c ARENE_ON_BY_DEFAULT
/// @return false otherwise
/// @pre The modified symbol must be defined, and it must only be defined as one of @c ARENE_ON , @c ARENE_OFF ,
///      @c ARENE_ON_BY_DEFAULT , or @c ARENE_OFF_BY_DEFAULT  else behavior is undefined.
#define ARENE_IS_ON_BY_DEFAULT(OP_SYMBOL) \
  ARENE_IS_ON_BY_DEFAULT_RAW(OP_SYMBOL##_I_)  // NOLINT(bugprone-macro-parentheses)

/// @brief Evaluates if the given compile time configuration symbol is "off by default."
/// @param OP_SYMBOL The symbol to evaluate, unmodified.
/// @return true if the input symbol was defined as @c ARENE_OFF_BY_DEFAULT.
/// @return false otherwise.
/// @pre The symbol must be defined, and it must only be defined as one of @c ARENE_ON , @c ARENE_OFF ,
///      @c ARENE_ON_BY_DEFAULT , or @c ARENE_OFF_BY_DEFAULT  else behavior is undefined.
/// @note This is generally not used directly in user code, prefer @c ARENE_IS_OFF_BY_DEFAULT instead.
#define ARENE_IS_OFF_BY_DEFAULT_RAW(OP_SYMBOL) \
  ((3U OP_SYMBOL 3U OP_SYMBOL 1U) == (0U - 1U))  // NOLINT(bugprone-macro-parentheses)

/// @brief Evaluates if the given compile time configuration symbol is "off by default."
/// @param OP_SYMBOL The "query name" of symbol to evaluate. It will have @c _I_ concatenated to it before
///            evaluation.
/// @return true if the modified input symbol was defined as @c ARENE_OFF_BY_DEFAULT
/// @return false otherwise
/// @pre The modified symbol must be defined, and it must only be defined as one of @c ARENE_ON , @c ARENE_OFF ,
///      @c ARENE_ON_BY_DEFAULT , or @c ARENE_OFF_BY_DEFAULT  else behavior is undefined.
#define ARENE_IS_OFF_BY_DEFAULT(OP_SYMBOL) \
  ARENE_IS_OFF_BY_DEFAULT_RAW(OP_SYMBOL##_I_)  // NOLINT(bugprone-macro-parentheses)

// parasoft-end-suppress AUTOSAR-A16_0_1-d-2

// parasoft-begin-suppress AUTOSAR-A16_0_1-d-2 "Macros that define special symbols to use in user configuration macros;
// these become the OP_SYMBOL in ARENE_IS_ON_RAW and ARENE_IS_OFF_RAW that are used to determine if the given feature is
// explicitly enabled or disabled either by the user, or by the implementation"
//
/// @brief A compile-time config state that specifies that the given symbol should be considered explicitly "on."
///
/// Usage:
/// @code{c++}
/// #define FOO_CONFIG_I_ ARENE_ON
/// @endcode
// NOLINTNEXTLINE(bugprone-macro-parentheses)
#define ARENE_ON |

/// @brief A compile-time config state that specifies that the given symbol should be considered explicitly "off."
///
/// Usage:
/// @code{c++}
/// #define FOO_CONFIG_I_ ARENE_OFF
/// @endcode
// NOLINTNEXTLINE(bugprone-macro-parentheses)
#define ARENE_OFF ^

/// @brief A compile-time config state that specifies that the given symbol should be considered implicitly "on."
///
/// Usage:
/// @code{c++}
/// #define FOO_CONFIG_I_ ARENE_ON_BY_DEFAULT
/// @endcode
// NOLINTNEXTLINE(bugprone-macro-parentheses)
#define ARENE_ON_BY_DEFAULT +

/// @brief A compile-time config state that specifies that the given symbol should be considered implicitly "off."
///
/// Usage:
/// @code{c++}
/// #define FOO_CONFIG_I_ ARENE_OFF_BY_DEFAULT
/// @endcode
// NOLINTNEXTLINE(bugprone-macro-parentheses)
#define ARENE_OFF_BY_DEFAULT -
// parasoft-end-suppress AUTOSAR-A16_0_1-d-2

// parasoft-begin-suppress AUTOSAR-A16_0_1-d-2 "Function-like macros permitted by A16-0-1 Permit #1"
/// @brief Rough validation that a given compile time configuration symbol has been correctly defined.
/// @param OP_SYMBOL The query symbol to evaluate.
/// @return true If one of @c ARENE_IS_ON(OP_SYMBOL) , @c ARENE_IS_OFF(OP_SYMBOL) , @c ARENE_IS_ON_BY_DEFAULT(OP_SYMBOL)
///         , or @c ARENE_IS_OFF_BY_DEFAULT(OP_SYMBOL) returns @c true .
/// @return false otherwise
/// @pre The symbol must be defined in such a way as to be compatible such that the @c ARENE_IS_ON and similar queries
///      are well formed, otherwise behavior is undefined (usually, but not always, a compiler error). This means:
///      1. There is a symbol defined which is @c OP_SYMBOL_I_
///      1. This symbol is defined as one of @c ARENE_ON , @c ARENE_OFF , @c ARENE_ON_BY_DEFAULT , or
///         @c ARENE_OFF_BY_DEFAULT
/// @warning This validation does _not_ magically permute the set of possible states the define could be in. It just
///          tries to validate that for the _current_ conditions, the symbol works with @c ARENE_IS_ON and friends. It
///          is up to the user to construct a test harness that would permute the define into the appropriate states to
///          get full coverage.
///
/// Usage:
/// @snippet docs/examples/compiler_support_examples.cpp arene_guarantee_internal_definition_usage_example
///
#define ARENE_GUARANTEE_INTERNAL_DEFINITION(OP_SYMBOL)                        \
  (ARENE_IS_ON_RAW(/* NOLINTNEXTLINE(bugprone-macro-parentheses)*/            \
                   OP_SYMBOL##_I_                                             \
   ) ||                                                                       \
   ARENE_IS_OFF_RAW(/* NOLINTNEXTLINE(bugprone-macro-parentheses)*/           \
                    OP_SYMBOL##_I_                                            \
   ) ||                                                                       \
   ARENE_IS_ON_BY_DEFAULT_RAW(/* NOLINTNEXTLINE(bugprone-macro-parentheses)*/ \
                              OP_SYMBOL##_I_                                  \
   ) ||                                                                       \
   ARENE_IS_OFF_BY_DEFAULT_RAW(/* NOLINTLINE(bugprone-macro-parentheses)*/ OP_SYMBOL##_I_))

// NOLINTEND(cppcoreguidelines-macro-usage)
// parasoft-end-suppress AUTOSAR-A16_0_1-d-2
// parasoft-end-suppress AUTOSAR-A16_0_1-a-2
// parasoft-end-suppress AUTOSAR-M16_0_6-a-2
// parasoft-end-suppress AUTOSAR-M16_3_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_PREPROCESSOR_DEFINE_CONFIGURATION_HPP_

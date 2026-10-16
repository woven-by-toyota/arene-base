// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_TESTS_PROPERTY_COMPARE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_TESTS_PROPERTY_COMPARE_HPP_

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/compiler_support/platform_queries/stdlib_info.hpp"
#include "arene/base/compiler_support/preprocessor/define_configuration.hpp"

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "These tests are precise enough to use exact equality");
#include "arene/base/functional/detail/operator_equal.hpp"  // IWYU pragma: keep

ARENE_IGNORE_END();

#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
#include <iostream>

#include "arene/base/linalg/tests/fp_bit_fmt.hpp"
#else
#include "arene/base/stdlib_choice/ignore.hpp"
#endif

#include "arene/base/testing/property_assertions.hpp"  // IWYU pragma: export
#include "arene/base/testing/property_macros.hpp"      // IWYU pragma: export

namespace arene {
namespace base {
namespace linalg_testing {

/// @brief compare two values, reporting a diagnostic on failure
/// @param cmp callable invoked as @c cmp(lhs,rhs) returning a boolean
/// @param lhs the left-hand operand
/// @param rhs the right-hand operand
/// @param lhs_expr stringified source expression for @c lhs, used as a label
/// @param rhs_expr stringified source expression for @c rhs, used as a label
/// @param expr stringified comparison expression, printed on failure
/// @return @c true if the operands compare equal, otherwise @c false
///
/// If @c cmp reports the operands as equal, returns @c true. Otherwise prints
/// @c expr and each operand to standard output and returns @c false, so it can
/// be used as the condition of @c ARENE_PROP_ASSERT. Floating-point operands
/// are printed via @c fp_bit_fmt, exposing their sign, exponent and mantissa
/// bits; other types are printed via their own stream formatter.
template <typename Compare, typename Lhs, typename Rhs>
auto assert_compare(
    Compare const& cmp,
    Lhs const& lhs,
    Rhs const& rhs,
    char const* lhs_expr,
    char const* rhs_expr,
    char const* expr
) -> bool {
  if (cmp(lhs, rhs)) {
    return true;
  }

#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  auto non_floating_point_fmt = [](auto const& value) -> auto const& { return value; };

  using base = decltype(non_floating_point_fmt);

  // inherit from base so the formatter has a generic fallback
  struct formatter : base {
    explicit formatter(base fmt)
        : base{fmt} {}

    using base::operator();
    auto operator()(float value) const { return fp_bit_fmt(value); }
    auto operator()(double value) const { return fp_bit_fmt(value); }
  };

  auto const format = formatter{non_floating_point_fmt};

  std::cout << "comparison failed: " << expr << "\n";
  std::cout << lhs_expr << ": " << format(lhs) << "\n";
  std::cout << rhs_expr << ": " << format(rhs) << "\n";
#else
  std::ignore = lhs_expr;
  std::ignore = rhs_expr;
  std::ignore = expr;
#endif

  return false;
}

}  // namespace linalg_testing
}  // namespace base
}  // namespace arene

/// @brief helper macro to dispatch to the correct compare macro
/// @param _1   first positional slot (unused)
/// @param _2   second positional slot (unused)
/// @param _3   third positional slot (unused)
/// @param NAME the selected macro name to return
/// @param ...  any trailing tokens (ignored)
///
/// Used as an argument-count dispatcher for @c ARENE_PROP_LINALG_ASSERT_COMPARE.
/// The positional arguments @c _1, @c _2, @c _3, @c NAME capture tokens from the
/// caller, and the selected @c NAME is returned. The variadic tail (@c ...)
/// absorbs any additional tokens so that the dispatch works regardless of
/// overall argument count.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ARENE_PROP_LINALG_ASSERT_COMPARE_PICK(_1, _2, _3, NAME, ...) NAME

/// @brief three-argument form of @c ARENE_PROP_LINALG_ASSERT_COMPARE
/// @param cmp binary predicate
/// @param lhs the left-hand operand
/// @param rhs the right-hand operand
///
/// Asserts that @c cmp(lhs,rhs) is @c true, comparing with the given @c cmp.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ARENE_PROP_LINALG_ASSERT_COMPARE3(cmp, lhs, rhs)                                                        \
  ARENE_PROP_ASSERT(                                                                                            \
      ::arene::base::linalg_testing::assert_compare(cmp, (lhs), (rhs), #lhs, #rhs, #cmp "(" #lhs ", " #rhs ")") \
  )

/// @brief two-argument form of @c ARENE_PROP_LINALG_ASSERT_COMPARE
/// @param lhs the left-hand operand
/// @param rhs the right-hand operand
///
/// Asserts that @c lhs and @c rhs are equal, comparing with @c operator==
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ARENE_PROP_LINALG_ASSERT_COMPARE2(lhs, rhs)                \
  ARENE_PROP_ASSERT(::arene::base::linalg_testing::assert_compare( \
      ::arene::base::functional_detail::operator_equal,            \
      (lhs),                                                       \
      (rhs),                                                       \
      #lhs,                                                        \
      #rhs,                                                        \
      #lhs " == " #rhs                                             \
  ))

/// @brief assert that two values compare equal, displaying them on failure
/// @param ... the operands to compare, optionally preceded by a binary predicate
///
/// Compares two values and, on failure, prints the comparison expression along
/// with each operand before failing the enclosing property via
/// @c ARENE_PROP_ASSERT. Floating-point operands are shown with their bit-level
/// representation.
///
/// The macro has two forms:
///
/// @code{.cpp}
/// ARENE_PROP_LINALG_ASSERT_COMPARE(lhs, rhs)
/// ARENE_PROP_LINALG_ASSERT_COMPARE(cmp, lhs, rhs)
/// @endcode
///
/// The first form compares with @c operator==. The second form compares with
/// the given binary predicate @c cmp.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ARENE_PROP_LINALG_ASSERT_COMPARE(...) \
  ARENE_PROP_LINALG_ASSERT_COMPARE_PICK(      \
      __VA_ARGS__,                            \
      ARENE_PROP_LINALG_ASSERT_COMPARE3,      \
      ARENE_PROP_LINALG_ASSERT_COMPARE2,      \
      unused_token_to_match_ellipses          \
  )                                           \
  (__VA_ARGS__)

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_TESTS_PROPERTY_COMPARE_HPP_

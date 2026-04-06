// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_ABS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_ABS_HPP_

// IWYU pragma: private, include "arene/base/math.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_floating_point.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"
#include "arene/base/stdlib_choice/is_signed.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"

// parasoft-begin-suppress AUTOSAR-M17_0_3-a-2 "False positive: names are in a different namespace"
// parasoft-begin-suppress CERT_CPP-DCL51-f-3 "False positive: no reserved names are used"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

#if ARENE_IS_ON(ARENE_HAS_BUILTIN_FABS) && ARENE_IS_ON(ARENE_HAS_BUILTIN_FABSF)
#include "arene/base/math/detail/builtin_abs_float.hpp"
#else
#include "arene/base/math/detail/custom_abs_float.hpp"
#endif

namespace arene {
namespace base {

/// @brief Calculate the absolute value of a number.
/// @tparam Number The type of the number.
/// @param value The value to take the absolute value of.
/// @pre If @c Number is an Integral type, @c value is not the smallest negative number the type can represent.
/// @return The absolute value of @c value, which is equivalent to <c>return value < 0 ? -value : value;</c>.
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <
    typename Number,
    constraints<std::enable_if_t<std::is_integral<Number>::value>, std::enable_if_t<std::is_signed<Number>::value> > =
        nullptr>
constexpr auto abs(Number value) noexcept -> Number {
  // Getting the absolute value of the smallest negative number is undefined behavior.
  constexpr Number zero{static_cast<Number>(0)};
  if (value < zero) {
    ARENE_PRECONDITION(value != std::numeric_limits<Number>::min());
    return static_cast<Number>(-value);
  }
  return value;
}
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Calculate the absolute value of a number.
/// @tparam Number The type of the number.
/// @param value The value to take the absolute value of.
/// @pre @c value is not the smallest negative number the type can represent.
/// @return The absolute value of @c value, which is equivalent to <c>return value < 0 ? -value : value;</c>.
///
/// @note Care should be taken when calling @c abs with unsigned values. This overload for unsigned values does not
/// prevent potential logic errors in expressions like: <tt>arene::base::abs(a - b)</tt>, where unsigned
/// wraparound can lead to unexpected behavior if <tt>a < b</tt>. Ensure that you understand the implications of using
/// @c abs() with unsigned types and consider using <tt>arene::base::abs_diff(a, b)</tt> instead.
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <
    typename Number,
    constraints<std::enable_if_t<std::is_integral<Number>::value>, std::enable_if_t<!std::is_signed<Number>::value> > =
        nullptr>
constexpr auto abs(Number value) noexcept -> Number {
  return value;
}
// parasoft-end-suppress AUTOSAR-A2_7_3
/// @brief Calculate the absolute value of a number.
/// @tparam Number The type of the number.
/// @param value The value to take the absolute value of.
/// @return The absolute value of @c value, which is equivalent to <c>return value < 0 ? -value : value;</c>.
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename Number, constraints<std::enable_if_t<(std::is_floating_point<Number>::value)> > = nullptr>
constexpr auto abs(Number value) noexcept -> Number {
  return math::detail::abs(value);
}
// parasoft-end-suppress AUTOSAR-A2_7_3

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_ABS_HPP_

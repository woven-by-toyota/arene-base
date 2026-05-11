// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_DETAIL_BUILTIN_ABS_FLOAT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_DETAIL_BUILTIN_ABS_FLOAT_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/math/.*"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax pmeritted by A7-1-5 Permit #1 v1.0.0"
// parasoft-begin-suppress AUTOSAR-M17_0_3-a-2 "False positive: names are in a different namespace"
// parasoft-begin-suppress CERT_CPP-DCL51-f-3 "False positive: no reserved names are used"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"
// parasoft-begin-suppress AUTOSAR-M17_0_3-a-2 "False positive: names are in a different namespace"
// parasoft-begin-suppress CERT_CPP-DCL51-f-3 "False positive: no reserved names are used"

namespace arene {
namespace base {
namespace math {
namespace detail {

/// @brief Calculate the absolute value of a 32-bit floating number.
/// Delegate to the builtin function.
/// @param value
/// @return The absolute value of @c value .
// parasoft-begin-suppress AUTOSAR-A7_1_3-a "False positive: const is on rhs"
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
constexpr auto abs(float const value) noexcept -> float { return __builtin_fabsf(value); }
// parasoft-end-suppress AUTOSAR-A2_7_3
// parasoft-end-suppress AUTOSAR-A7_1_3-a

/// @brief Calculate the absolute value of a 64-bit floating number.
/// Delegate to the builtin function.
/// @param value
/// @return The absolute value of @c value .
// parasoft-begin-suppress AUTOSAR-A7_1_3-a "False positive: const is on rhs"
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
constexpr auto abs(double const value) noexcept -> double { return __builtin_fabs(value); }
// parasoft-end-suppress AUTOSAR-A2_7_3
// parasoft-end-suppress AUTOSAR-A7_1_3-a

}  // namespace detail
}  // namespace math
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_DETAIL_BUILTIN_ABS_FLOAT_HPP_

// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_DETAIL_CUSTOM_ABS_FLOAT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_DETAIL_CUSTOM_ABS_FLOAT_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/math/.*"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"
// parasoft-begin-suppress AUTOSAR-M17_0_3-a-2 "False positive: names are in a different namespace"
// parasoft-begin-suppress CERT_CPP-DCL51-f-3 "False positive: no reserved names are used"

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/math/float_properties.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_floating_point.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"

namespace arene {
namespace base {
namespace math {
namespace detail {

ARENE_IGNORE_START();
ARENE_IGNORE_ALL(
    "-Wfloat-equal",
    "In this situation testing for exact equality to zero to handle edge cases with NaN and Inf in floats."
);

/// @brief Calculate the absolute value of a 32-bit or 64-bit floating number.
/// @param value
/// @return The absolute value of @c value .
template <class Number, constraints<std::enable_if_t<(std::is_floating_point<Number>::value)>> = nullptr>
constexpr auto abs(Number value) noexcept -> Number {
  // Have to check for NaN before doing any math to work around a GCC bug which treats any use of NaN as not constexpr.
  if (::arene::base::isnan(value)) {
    // According to ISO/IEC 60559, the abs function should set the sign bit to 0 without decaying signaling NaNs to
    // quiet NaNs. However, there's no way to distinguish between quiet and signaling NaN at compile time in C++14, so
    // we have to assume that the NaN is quiet. If it were signaling, it may have already decayed due to being copied.
    return std::numeric_limits<Number>::quiet_NaN();
  }

  constexpr auto zero = static_cast<Number>(0);
  if (value < zero) {
    return -value;
  }
  // parasoft-begin-suppress AUTOSAR-M6_2_2-a "Equality is effectively part of a three-way comparison, not used alone"
  if (value == zero) {
    // parasoft-end-suppress AUTOSAR-M6_2_2-a
    // Both positive and negative zero compare equal to zero; this makes sure we return positive zero.
    return zero;
  }

  return value;
}

ARENE_IGNORE_END();

}  // namespace detail
}  // namespace math
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_DETAIL_CUSTOM_ABS_FLOAT_HPP_

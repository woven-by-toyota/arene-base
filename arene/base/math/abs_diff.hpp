// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_ABS_DIFF_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_ABS_DIFF_HPP_

// IWYU pragma: private, include "arene/base/math.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_arithmetic.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

/// @brief Calculate the absolute difference between two numbers
/// @tparam Number The type of the number.
/// @param first The first value to take the absolute difference of
/// @param second The second value to take the absolute difference of
/// @return The absolute difference between @c first and @c second
template <typename Number, constraints<std::enable_if_t<std::is_arithmetic<Number>::value>> = nullptr>
constexpr auto abs_diff(Number first, Number second) noexcept -> Number {
  if (first < second) {
    return static_cast<Number>(second - first);
  }
  return static_cast<Number>(first - second);
}

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_ABS_DIFF_HPP_

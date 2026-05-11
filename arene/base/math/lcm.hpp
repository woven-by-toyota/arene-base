// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_LCM_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_LCM_HPP_

// IWYU pragma: private, include "arene/base/math.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/math/abs.hpp"
#include "arene/base/math/gcd.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"

namespace arene {
namespace base {

/// @brief Calculates the Least Common Multiple (LCM) of two integral values.
/// @tparam Integral The type of the values
/// @param lhs First value
/// @param rhs Second value
/// @return value that is LCM of @c lhs and @c rhs. If either @c lhs or @c rhs is @c 0, then returns @c 0. The
/// result is always non-negative.
///
/// The LCM is the smallest positive integer that is divisible by both input values.
/// It is calculated using the mathematical identity: lcm(a,b) = abs(a*b) / gcd(a,b).
template <typename Integral, constraints<std::enable_if_t<std::is_integral<Integral>::value> > = nullptr>
constexpr auto lcm(Integral lhs, Integral rhs) noexcept -> Integral {
  if ((lhs == Integral{}) || (rhs == Integral{})) {
    return Integral{};
  }

  // lcm(a,b) can be rewritten as (a / gcd(a,b)) * b to prevent possible overflow
  return abs((lhs / gcd(lhs, rhs)) * rhs);
}

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_LCM_HPP_

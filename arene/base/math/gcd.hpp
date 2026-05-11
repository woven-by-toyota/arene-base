// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_GCD_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_GCD_HPP_

// IWYU pragma: private, include "arene/base/math.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/math/abs.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"

namespace arene {
namespace base {

/// @brief Calculates the Greatest Common Divisor (GCD) of two Integral values.
/// @tparam Integral The type of the values
/// @param lhs First value
/// @param rhs Second value
/// @return Integral value that is GCD of @c lhs and @c rhs . If either @c lhs or @c rhs is @c 0 , then the absolute
///          value of the non-zero operand is returned. If both are @c 0 , then returns @c 0 .
template <typename Integral, constraints<std::enable_if_t<std::is_integral<Integral>::value> > = nullptr>
constexpr auto gcd(Integral lhs, Integral rhs) noexcept -> Integral {
  while (rhs != Integral{}) {
    Integral tmp{lhs % rhs};
    lhs = rhs;
    rhs = tmp;
  }
  return abs(lhs);
}

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_GCD_HPP_

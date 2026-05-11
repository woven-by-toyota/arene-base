// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_LOG2_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_LOG2_HPP_

// IWYU pragma: private, include "arene/base/math.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax pmeritted by A7-1-5 Permit #1 v1.0.0"
// parasoft-begin-suppress AUTOSAR-M17_0_3-a-2 "False positive: names are in a different namespace"
// parasoft-begin-suppress CERT_CPP-DCL51-f-3 "False positive: no reserved names are used"

#include "arene/base/contracts/contract.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

/// @brief Computes the integer base-2 log of a value.
/// @param value The value to compute the base-2 log of.
/// @return std::size_t The greatest value. @c x ,  for which @c 2^x<=value .
/// @pre @c value must be greater than zero.
inline constexpr auto log2(std::size_t value) noexcept -> std::size_t {  // CODEQLFP(DCL51-CPP)
  ARENE_PRECONDITION(value > std::size_t{});

  std::size_t res{0U};
  constexpr std::size_t two{2U};
  while (value >= two) {
    value /= two;
    ++res;
  }
  return res;
}

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_LOG2_HPP_

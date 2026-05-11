// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_DETAIL_SUM_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_DETAIL_SUM_HPP_

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"  // IWYU pragma: export

namespace arene {
namespace base {
namespace units_detail {

/// @brief sum of a range of values
/// @param values values to sum
/// @return sum of @c values
/// @note @c constexpr and range friendly sum function
constexpr auto sum(std::initializer_list<std::size_t> const values) noexcept -> std::size_t {
  auto count = std::size_t{};

  // parasoft-begin-suppress CERT_C-INT36-b "False positive: 'const std::size_t*' not converted to integer"
  for (std::size_t const elem : values) {
    // parasoft-end-suppress CERT_C-INT36-b
    count += elem;
  }

  return count;
}

}  // namespace units_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_DETAIL_SUM_HPP_

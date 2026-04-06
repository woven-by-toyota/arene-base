// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file clamp.hpp
/// @brief Provides implementation of a backport of C++20's version of std::clamp
///
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_CLAMP_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_CLAMP_HPP_

// IWYU pragma: private, include "arene/base/algorithm.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/less.hpp"
#include "arene/base/type_traits/is_compare.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
// parasoft-begin-suppress AUTOSAR-A7_5_1-a "Returning a reference is required to match std::clamp."
/// @brief Clamp the supplied value between two bounds using the supplied comparator for ordering
/// @tparam T The type of the value and both bounds
/// @tparam Compare The type of a comparator which models a strict weak ordering, similar to @c std::less
/// @param val The value to clamp between the two bounds
/// @param low The lower bound
/// @param high The upper bound
/// @param comp The comparator instance to use
/// @pre @c low must be less than or equal to @c high ; that is, @c comp(high,low) must be @c false
/// @return A reference to @c val if it's between the bounds, or to the nearest bound if @c val is outside the bounds
template <typename T, class Compare, constraints<std::enable_if_t<is_compare_v<Compare&, T const&>>> = nullptr>
constexpr auto clamp(T const& val, T const& low, T const& high, Compare comp) noexcept(noexcept(comp(val, low)))
    -> T const& {
  ARENE_PRECONDITION(!comp(high, low));

  if (comp(val, low)) {
    return low;
  }

  if (comp(high, val)) {
    return high;
  }

  return val;
}

/// @brief Clamp the supplied value between two bounds using @c std::less for ordering
/// @tparam T The type of the value and both bounds
/// @param val The value to clamp between the two bounds
/// @param low The lower bound
/// @param high The upper bound
/// @pre @c low must be less than or equal to @c high
/// @return A reference to @c val if it's between the bounds, or to the nearest bound if @c val is outside the bounds
template <typename T>
constexpr auto clamp(T const& val, T const& low, T const& high) noexcept(noexcept(std::less<T>{}(val, low)))
    -> T const& {
  return ::arene::base::clamp(val, low, high, std::less<T>{});
}
// parasoft-end-suppress AUTOSAR-A7_5_1-a "Returning a reference is required to match std::clamp."
// parasoft-end-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"

}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_CLAMP_HPP_

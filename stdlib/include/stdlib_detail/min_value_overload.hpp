// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_MIN_VALUE_OVERLOAD_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_MIN_VALUE_OVERLOAD_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "stdlib/include/stdlib_detail/declval.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <algorithm>
// IWYU pragma: friend "stdlib_detail/.*"

namespace std {

// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
// parasoft-begin-suppress AUTOSAR-M17_0_3-a "This is an implementation of the standard library function"
// parasoft-begin-suppress AUTOSAR-M4_5_1-a "False positive: Arguments are only used with the conditional operator"
// parasoft-begin-suppress CERT_C-EXP46-b "False positive: Arguments are only used with the conditional operator"
/// @brief Returns the smaller of the given values.
/// @tparam T The type of the values
/// @param lhs The first value to compare
/// @param rhs The second value to compare
/// @return Returns the smaller of @c lhs and @c rhs
///
/// @note If T is not @c LessThanComparable, the behavior is undefined
template <class T>
constexpr auto min(T const& lhs, T const& rhs) noexcept(noexcept(rhs < lhs)) -> T const& {
  return rhs < lhs ? rhs : lhs;
}
// parasoft-end-suppress CERT_C-EXP46-b
// parasoft-end-suppress AUTOSAR-M4_5_1-a

/// @brief Returns the smaller of the given values.
/// @tparam T The type of the values
/// @tparam Compare The type of the comparison function
/// @param lhs The first value to compare
/// @param rhs The second value to compare
/// @param comp The comparison function which returns true if the first parameter is less than the second
/// @return Returns the smaller of @c lhs and @c rhs using the comparison function @c comp to compare the values
template <class T, class Compare>
constexpr auto min(T const& lhs, T const& rhs, Compare comp) noexcept(
    noexcept(std::declval<Compare&>()(std::declval<T&>(), std::declval<T&>()))
) -> T const& {
  // The callable comp checks if the first argument is less than the second. As lhs should be returned if the two
  // inputs are equal, the comparison of comp(rhs, lhs) is used to check if rhs should be returned.
  // NOLINTNEXTLINE(readability-suspicious-call-argument)
  return comp(rhs, lhs) ? rhs : lhs;
}

// parasoft-end-suppress AUTOSAR-M3_3_2-a-2
// parasoft-end-suppress AUTOSAR-M17_0_3-a

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_MIN_VALUE_OVERLOAD_HPP_

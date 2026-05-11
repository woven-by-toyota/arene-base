// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_GREATER_EQUAL_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_GREATER_EQUAL_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <functional>
// IWYU pragma: friend "stdlib_detail/.*"

#include "stdlib/include/stdlib_detail/forward.hpp"

namespace std {

// parasoft-begin-suppress AUTOSAR-M4_5_1-a "Standard library implementation must cover all types, including bool"
// parasoft-begin-suppress CERT_C-EXP46-b "Standard library implementation must cover all types, including bool"

/// @brief Function object that can be invoked for greater than or equal to comparisons
/// @tparam T The type of the values
/// @note This class contains the member types @c result_type, @c first_argument type, and @c second_argument type.
///   These aliases have been deprecated in C++17 and removed in C++20.
template <class T = void>
class greater_equal {
 public:
  /// @brief Result type of invoking this object
  using result_type = bool;
  /// @brief Type of the first value to compare
  using first_argument_type = T;
  /// @brief Type of the second value to compare
  using second_argument_type = T;

  /// @brief Returns if the first value is greater than or equal to the second
  /// @param lhs The first value to compare
  /// @param rhs The second value to compare
  /// @return Returns the boolean result of invoking @c operator>= on @c lhs and @c rhs
  constexpr auto operator()(T const& lhs, T const& rhs) const noexcept(noexcept(lhs >= rhs)) -> bool {
    return lhs >= rhs;
  }
};

/// @brief Function object that can be invoked for greater than or equal to comparisons, specialized for type deduction
template <>
class greater_equal<void> {
 public:
  /// @brief Denotes that this is a transparent function object type
  using is_transparent = void;

  /// @brief Returns if the first value is greater than or equal to the second
  /// @tparam T The first type
  /// @tparam U The second type
  /// @param lhs The first value to compare
  /// @param rhs The second value to compare
  /// @return Returns the boolean result of invoking @c operator>= on @c lhs and @c rhs
  template <class T, class U>
  constexpr auto operator()(T&& lhs, U&& rhs) const noexcept(noexcept(std::forward<T>(lhs) >= std::forward<U>(rhs)))
      -> decltype(std::forward<T>(lhs) >= std::forward<U>(rhs)) {
    return std::forward<T>(lhs) >= std::forward<U>(rhs);
  }
};

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_GREATER_EQUAL_HPP_

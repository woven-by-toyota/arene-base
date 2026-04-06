// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_EQUAL_TO_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_EQUAL_TO_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <functional>
// IWYU pragma: friend "stdlib_detail/.*"

#include "arene/base/compiler_support/diagnostics.hpp"
#include "stdlib/include/stdlib_detail/forward.hpp"

namespace std {

/// @brief Function object that can be invoked for equality comparisons
/// @tparam T The type of the values
/// @note This class contains the member types @c result_type, @c first_argument type, and @c second_argument type.
///   These aliases have been deprecated in C++17 and removed in C++20.
template <class T = void>
class equal_to {
 public:
  /// @brief Result type of invoking this object
  using result_type = bool;
  /// @brief Type of the first value to compare
  using first_argument_type = T;
  /// @brief Type of the second value to compare
  using second_argument_type = T;

  /// @brief Returns if two values are equal
  /// @param lhs The first value to compare
  /// @param rhs The second value to compare
  /// @return Returns the boolean result of invoking @c operator== on @c lhs and @c rhs
  constexpr auto operator()(T const& lhs, T const& rhs) const noexcept(noexcept(lhs == rhs)) -> bool {
    // parasoft-begin-suppress AUTOSAR-M6_2_2-a "The type being compared is up to the user"
    return lhs == rhs;
    // parasoft-end-suppress AUTOSAR-M6_2_2-a "The type being compared is up to the user"
  }
};

/// @brief Function object that can be invoked for equality comparisons, specialized for type deduction
template <>
class equal_to<void> {
 public:
  /// @brief Denotes that this is a transparent function object type
  using is_transparent = void;

  // parasoft-begin-suppress AUTOSAR-M6_2_2-a "The type being compared is up to the user"
  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "The type being compared is up to the user");
  /// @brief Returns if two values are equal with parameter and return type deduced
  /// @tparam T The type of the first value
  /// @tparam U The type of the second value
  /// @param lhs The first value to compare
  /// @param rhs The second value to compare
  /// @return Returns the result of invoking @c operator== on @c lhs and @c rhs
  template <class T, class U>
  constexpr auto operator()(T&& lhs, U&& rhs) const noexcept(noexcept(std::forward<T>(lhs) == std::forward<U>(rhs)))
      -> decltype(std::forward<T>(lhs) == std::forward<U>(rhs)) {
    return std::forward<T>(lhs) == std::forward<U>(rhs);
  }
  ARENE_IGNORE_END();
  // parasoft-end-suppress AUTOSAR-M6_2_2-a "The type being compared is up to the user"
};

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_EQUAL_TO_HPP_

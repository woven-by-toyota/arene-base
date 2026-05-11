// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_LOGICAL_AND_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_LOGICAL_AND_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <functional>
// IWYU pragma: friend "stdlib_detail/.*"

#include "arene/base/compiler_support/diagnostics.hpp"
#include "stdlib/include/stdlib_detail/declval.hpp"
#include "stdlib/include/stdlib_detail/forward.hpp"

namespace std {

/// @brief Function object for performing logical AND (logical conjunction). Effectively calls @c operator&& on type @c
/// T.
/// @tparam T The type of the values.
/// @note This class contains the member types @c result_type, @c first_argument type, and @c second_argument type.
///   These aliases have been deprecated in C++17 and removed in C++20.
template <class T = void>
class logical_and {
 public:
  /// @brief Result type of invoking this object (deprecated in C++17)(removed in C++20)
  using result_type = bool;
  /// @brief Type of the first value to logically AND (deprecated in C++17)(removed in C++20)
  using first_argument_type = T;
  /// @brief Type of the second value to logically AND (deprecated in C++17)(removed in C++20)
  using second_argument_type = T;

  ARENE_IGNORE_START();
  ARENE_IGNORE_ARMCLANG("-Wnull-conversion", "The standard expects the conversions here");
  /// @brief Returns the logical AND of @c lhs and @c rhs.
  /// @tparam U Template parameter to enable constraints.
  /// @param lhs The first value to compute logical AND of
  /// @param rhs The second value to compute logical AND of
  /// @return Returns the result of invoking <c>lhs && rhs</c>.
  constexpr auto operator()(T const& lhs, T const& rhs) const
      noexcept(noexcept(std::declval<T const&>() && std::declval<T const&>())) -> bool {
    // parasoft-begin-suppress AUTOSAR-M5_3_1-a "False positive: operator&& accepts non-bool types in std::logical_and"
    return lhs && rhs;
    // parasoft-end-suppress AUTOSAR-M5_3_1-a
  }
  ARENE_IGNORE_END();
};

/// @brief Function object for performing logical AND (logical conjunction), specialized for deduction of parameter and
/// return type.
template <>
class logical_and<void> {
 public:
  /// @brief Denotes that this is a transparent function object type
  using is_transparent = void;

  ARENE_IGNORE_START();
  ARENE_IGNORE_ARMCLANG("-Wnull-conversion", "The standard expects the conversions here");
  // parasoft-begin-suppress AUTOSAR-M5_0_4-a "Function return type is specified by the C++ Standard"
  /// @brief Returns the logical AND of @c lhs and @c rhs.
  /// @tparam T The type of @c lhs
  /// @tparam U The type of @c rhs
  /// @param lhs The first value to compute logical AND of
  /// @param rhs The second value to compute logical AND of
  /// @return Returns the result of invoking <c>lhs && rhs</c>.
  template <class T, class U>
  constexpr auto operator()(T&& lhs, U&& rhs) const
      noexcept(noexcept(std::forward<T>(std::declval<T&&>()) && std::forward<U>(std::declval<U&&>())))
          -> decltype(std::forward<T>(lhs) && std::forward<U>(rhs)) {
    // parasoft-begin-suppress AUTOSAR-M5_3_1-a "False positive: operator&& accepts non-bool types in std::logical_and"
    return std::forward<T>(lhs) && std::forward<U>(rhs);
    // parasoft-end-suppress AUTOSAR-M5_3_1-a
  }
  // parasoft-end-suppress AUTOSAR-M5_0_4-a
  ARENE_IGNORE_END();
};

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_LOGICAL_AND_HPP_

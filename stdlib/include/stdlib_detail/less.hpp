// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_LESS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_LESS_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <functional>
// IWYU pragma: friend "stdlib_detail/.*"

#include "arene/base/constraints.hpp"
#include "stdlib/include/stdlib_detail/cstddef.hpp"
#include "stdlib/include/stdlib_detail/decay.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/forward.hpp"
#include "stdlib/include/stdlib_detail/is_pointer.hpp"
#include "stdlib/include/stdlib_detail/is_same.hpp"

namespace std {

// parasoft-begin-suppress AUTOSAR-M4_5_1-a "Standard library implementation must cover all types, including bool"
// parasoft-begin-suppress CERT_C-EXP46-b "Standard library implementation must cover all types, including bool"

/// @brief Function object that can be invoked for less than comparisons
/// @tparam T The type of the values
/// @note This class contains the member types @c result_type, @c first_argument type, and @c second_argument type.
///   These aliases have been deprecated in C++17 and removed in C++20.
template <class T = void>
class less {
 public:
  /// @brief Result type of invoking this object
  using result_type = bool;
  /// @brief Type of the first value to compare
  using first_argument_type = T;
  /// @brief Type of the second value to compare
  using second_argument_type = T;

  /// @brief Returns if the first value is less than the second
  /// @param lhs The first value to compare
  /// @param rhs The second value to compare
  /// @return Returns the boolean result of invoking @c operator< on @c lhs and @c rhs
  constexpr auto operator()(T const& lhs, T const& rhs) const noexcept(noexcept(lhs < rhs)) -> bool {
    return lhs < rhs;
  }
};

// parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: No identifiers are hidden by this type"
// parasoft-begin-suppress AUTOSAR-A2_7_3-a "False positive: it does have a brief tag."
/// @brief Function object that can be invoked for less than comparisons, specialized for type deduction
template <>
class less<void> {
 public:
  /// @brief Denotes that this is a transparent function object type
  using is_transparent = void;

  /// @brief Returns if the first value is less than the second
  /// @tparam T The first type
  /// @tparam U The second type
  /// @param lhs The first value to compare
  /// @param rhs The second value to compare
  /// @return Returns the boolean result of invoking @c operator< on @c lhs and @c rhs
  template <
      class T,
      class U,
      arene::base::constraints<
          enable_if_t<!(is_pointer_v<decay_t<T>> && is_same_v<decay_t<U>, nullptr_t>)>,
          enable_if_t<!(is_pointer_v<decay_t<U>> && is_same_v<decay_t<T>, nullptr_t>)>> = nullptr>
  constexpr auto operator()(T&& lhs, U&& rhs) const noexcept(noexcept(std::forward<T>(lhs) < std::forward<U>(rhs)))
      -> decltype(std::forward<T>(lhs) < std::forward<U>(rhs)) {
    return std::forward<T>(lhs) < std::forward<U>(rhs);
  }

  /// @brief Returns if the first value is less than the second
  ///
  /// This overload checks if a pointer is less than @c nullptr which is always @c false
  /// @tparam T The first type
  /// @tparam U The second type
  /// @return Returns the boolean result of invoking @c operator< on the supplied values
  template <
      class T,
      class U,
      arene::base::constraints<enable_if_t<(is_pointer_v<decay_t<T>> && is_same_v<decay_t<U>, nullptr_t>)>> = nullptr>
  // parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"
  constexpr auto operator()(T&&, U&&) const noexcept -> bool {
    return false;
  }
  // parasoft-end-suppress CERT_C-EXP37-a

  /// @brief Returns if the first value is less than the second
  ///
  /// This overload checks if @c nullptr is less than a supplied pointer
  /// @tparam T The first type
  /// @tparam U The second type
  /// @param rhs The pointer value to compare
  /// @return Returns the boolean result of invoking @c operator< on the supplied values
  template <
      class T,
      class U,
      arene::base::constraints<enable_if_t<(is_pointer_v<decay_t<U>> && is_same_v<decay_t<T>, nullptr_t>)>> = nullptr>
  // parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"
  constexpr auto operator()(T&&, U&& rhs) const noexcept -> bool {
    return rhs != nullptr;
  }
  // parasoft-end-suppress CERT_C-EXP37-a
};
// parasoft-end-suppress AUTOSAR-A2_7_3-a "False positive: it does have a brief tag."
// parasoft-end-suppress AUTOSAR-A2_10_1-d "False positive: No identifiers are hidden by this type"

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_LESS_HPP_

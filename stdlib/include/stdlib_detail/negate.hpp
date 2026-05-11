// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_NEGATE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_NEGATE_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <functional>
// IWYU pragma: friend "stdlib_detail/.*"

#include "arene/base/constraints.hpp"
#include "stdlib/include/stdlib_detail/declval.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/forward.hpp"
#include "stdlib/include/stdlib_detail/is_integral.hpp"

namespace std {

/// @brief Function object for performing negation.
/// @tparam T The type of the values
/// @note This class contains the member types @c result_type, @c first_argument type, and @c second_argument type.
///   These aliases have been deprecated in C++17 and removed in C++20.
template <class T = void>
class negate {
 public:
  /// @brief Result type of invoking this object
  using result_type = T;
  /// @brief Type of the value to negate
  using argument_type = T;

  /// @brief Returns the negation of the value
  /// @tparam U Template parameter to enable constraints.
  /// @param value The value to negate
  /// @return Returns the result of invoking @c operator- on @c value
  template <class U = T, arene::base::constraints<std::enable_if_t<!is_integral_v<U>>> = nullptr>
  constexpr auto operator()(T const& value) const noexcept(noexcept(-std::declval<T>())) -> T {
    return -value;
  }

  /// @brief Returns the negation of the value
  /// @tparam U Template parameter to enable constraints.
  /// @param value The value to negate
  /// @return Returns the result of invoking @c operator- on @c value
  ///
  /// @note This overload explicitly casts the result of the computation to the type @c T. This avoids any warning for
  /// implicit conversion (present on gcc8).
  template <class U = T, arene::base::constraints<std::enable_if_t<is_integral_v<U>>> = nullptr>
  constexpr auto operator()(T const& value) const noexcept(noexcept(-std::declval<T>())) -> T {
    return static_cast<T>(-value);
  }
};

/// @brief Function object that can be invoked for negate, specialized for type deduction
template <>
class negate<void> {
 public:
  /// @brief Denotes that this is a transparent function object type
  using is_transparent = void;

  // parasoft-begin-suppress AUTOSAR-M5_0_4-a "Function return type is specified by the C++ Standard"
  /// @brief Returns the negation of the value
  /// @tparam T The value type
  /// @param value The value to negate
  /// @return Returns the result of invoking @c operator- on @c value
  template <class T>
  constexpr auto operator()(T&& value) const noexcept(noexcept(-std::forward<T>(std::declval<T>())))
      -> decltype(-std::forward<T>(value)) {
    return -std::forward<T>(value);
  }
  // parasoft-end-suppress AUTOSAR-M5_0_4-a
};

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_NEGATE_HPP_

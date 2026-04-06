// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_PLUS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_PLUS_HPP_

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

/// @brief Function object for performing addition.
/// @tparam T The type of the values
/// @note This class contains the member types @c result_type, @c first_argument type, and @c second_argument type.
///   These aliases have been deprecated in C++17 and removed in C++20.
template <class T = void>
class plus {
 public:
  /// @brief Result type of invoking this object
  using result_type = T;
  /// @brief Type of the first value to add
  using first_argument_type = T;
  /// @brief Type of the second value to add
  using second_argument_type = T;

  /// @brief Returns the first value plus the second value
  /// @tparam U Template parameter to enable constraints.
  /// @param lhs The first value to add
  /// @param rhs The second value to add
  /// @return Returns the result of invoking @c operator+ on @c lhs and @c rhs
  template <class U = T, arene::base::constraints<std::enable_if_t<!is_integral_v<U>>> = nullptr>
  constexpr auto operator()(T const& lhs, T const& rhs) const
      noexcept(noexcept(std::declval<T&>() + std::declval<T&>())) -> T {
    return lhs + rhs;
  }

  /// @brief Returns the first value plus the second value
  /// @tparam U Template parameter to enable constraints.
  /// @param lhs The first value to add
  /// @param rhs The second value to add
  /// @return Returns the result of invoking @c operator+ on @c lhs and @c rhs
  ///
  /// @note This overload explicitly casts the result of the computation to the type @c T. This avoids any warning for
  /// implicit conversion (present on gcc8).
  template <class U = T, arene::base::constraints<std::enable_if_t<is_integral_v<U>>> = nullptr>
  constexpr auto operator()(T const& lhs, T const& rhs) const noexcept(noexcept(std::declval<T>() + std::declval<T&>()))
      -> T {
    return static_cast<T>(lhs + rhs);
  }
};

/// @brief Function object that can be invoked for addition, specialized for type deduction
template <>
class plus<void> {
 public:
  /// @brief Denotes that this is a transparent function object type
  using is_transparent = void;

  // parasoft-begin-suppress AUTOSAR-M5_0_4-a "Function return type is specified by the C++ Standard"
  /// @brief Returns the first value plus the second value
  /// @tparam T The first type
  /// @tparam U The second type
  /// @param lhs The first value to add
  /// @param rhs The second value to add
  /// @return Returns the result of invoking @c operator+ on @c lhs and @c rhs
  template <class T, class U>
  constexpr auto operator()(T&& lhs, U&& rhs) const
      noexcept(noexcept(std::forward<T>(std::declval<T&>()) + std::forward<U>(std::declval<U&>())))
          -> decltype(std::forward<T>(lhs) + std::forward<U>(rhs)) {
    return std::forward<T>(lhs) + std::forward<U>(rhs);
  }
  // parasoft-end-suppress AUTOSAR-M5_0_4-a
};

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_PLUS_HPP_

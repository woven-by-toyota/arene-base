// parasoft-suppress AUTOSAR-A2_8_1-a "False positive: abs_impl implements the abs function as a CPO"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_ABS_CPO_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_ABS_CPO_HPP_

// IWYU pragma: private, include "arene/base/math.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/math/abs.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-M17_0_3-a "False positive: this name is not reserved in namespace arene::base::math"
// parasoft-begin-suppress CERT_CPP-DCL51-f "False positive: this name is not reserved in namespace arene::base::math"

namespace arene {
namespace base {
namespace math {

namespace abs_detail {

// parasoft-begin-suppress CERT_C-EXP37-a "False positive: rule does not mention naming all parameters"
// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: function does not have internal linkage"

/// @brief Dummy declaration of the ADL customization point so the call below will be well-formed but not match
/// @tparam U A type being checked for abs overloads
template <typename U>
auto abs(U) -> U = delete;

// parasoft-end-suppress CERT_C-EXP37-a-3
// parasoft-end-suppress AUTOSAR-M3_3_2-a

/// @brief Type trait to determine if a type has a const member function called @c abs()
/// @tparam T The type to check
template <typename T, typename = constraints<>>
constexpr bool has_member_abs_v{false};

/// @brief Type trait to determine if a type has a const member function called @c abs()
/// @tparam T The type to check
template <typename T>
constexpr bool has_member_abs_v<T, constraints<decltype(std::declval<T const&>().abs())>>{true};

/// @brief Type trait to determine if a type has an ADL-visible free function <c>abs(T const&)</c>
/// @tparam T The type to check
template <typename T, typename = constraints<>>
constexpr bool has_adl_abs_v{false};

/// @brief Type trait to determine if a type has an ADL-visible free function <c>abs(T const&)</c>
/// @tparam T The type to check
template <typename T>
constexpr bool has_adl_abs_v<T, constraints<decltype(abs(std::declval<T const&>()))>>{true};

/// @brief Function object class implementing @c arene::base::math::abs
class abs_impl {
 public:
  /// @brief Obtain the absolute value of a value
  /// @tparam T Type of the value to obtain the absolute value of
  /// @param value Value to obtain the absolute value of
  /// @return The absolute value of @c value
  template <typename T, constraints<std::enable_if_t<has_member_abs_v<T>>> = nullptr>
  constexpr auto operator()(T const& value) const noexcept(noexcept(std::declval<T const&>().abs()))
      -> decltype(value.abs()) {
    return value.abs();
  }

  /// @brief Obtain the absolute value of a value
  /// @tparam T Type of the value to obtain the absolute value of
  /// @param value Value to obtain the absolute value of
  /// @return The absolute value of @c value
  template <typename T, constraints<std::enable_if_t<!has_member_abs_v<T> && has_adl_abs_v<T>>> = nullptr>
  constexpr auto operator()(T const& value) const noexcept(noexcept(abs(std::declval<T const&>())))
      -> decltype(abs(value)) {
    return abs(value);
  }

  /// @brief Obtain the absolute value of a value
  /// @tparam T Type of the value to obtain the absolute value of
  /// @param value Value to obtain the absolute value of
  /// @return The absolute value of @c value
  template <
      typename T,
      constraints<std::enable_if_t<!has_member_abs_v<T>>, std::enable_if_t<!has_adl_abs_v<T>>> = nullptr>
  constexpr auto operator()(T const& value) const noexcept -> decltype(::arene::base::abs(value)) {
    return ::arene::base::abs(value);
  }
};
}  // namespace abs_detail

// parasoft-begin-suppress AUTOSAR-M7_3_3-a "Unnamed namespace creates a per-TU reference to a single global object."
// parasoft-begin-suppress CERT_CPP-DCL59-a "Unnamed namespace creates a per-TU reference to a single global object."
/// @brief Obtain the absolute value using a member function, an ADL overload, or @c arene::base::abs
ARENE_CPP14_INLINE_VARIABLE(abs_detail::abs_impl, abs);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace math
}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-M17_0_3-a
// parasoft-end-suppress CERT_CPP-DCL51-f

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_ABS_CPO_HPP_

// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::math_detail::abs_if_needed"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_DETAIL_ABS_IF_NEEDED_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_DETAIL_ABS_IF_NEEDED_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/math/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/math/abs.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/is_arithmetic.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"

namespace arene {
namespace base {
namespace math_detail {

/// @brief Dummy declaration so that the non-arithmetic overload below will be well-formed
/// @tparam U A type being checked for abs overloads
// parasoft-begin-suppress AUTOSAR-M17_0_3-a "False positive: doesn't override, provides an overload for new types"
// parasoft-begin-suppress CERT_CPP-DCL51-f "False positive: doesn't override, provides an overload for new types"
// parasoft-begin-suppress CERT_C-EXP37-a "False positive: rule does not mention naming all parameters"
template <typename U>
auto abs(U) -> U = delete;
// parasoft-end-suppress AUTOSAR-M17_0_3-a
// parasoft-end-suppress CERT_CPP-DCL51-f
// parasoft-end-suppress CERT_C-EXP37-a

/// @brief Implementation for the @c abs_if_needed function object
///
/// Calls @c arene::base::abs for arithmetic types and looks for @c abs by ADL for non-arithmetic types.
class abs_if_needed_impl {
  /// @brief For arithmetic types, preferentially use @c arene::base::abs
  /// @tparam T Type of the argument; constrained to be arithmetic
  /// @param value Value to get the absolute value of
  /// @return The absolute value of @c value as determined by @c arene::base::abs
  /// @pre If @c T is a signed integer type and @c value is the most negative value of that type,
  /// this crashes with an @c ARENE_PRECONDITION violation
  template <typename T, constraints<std::enable_if_t<std::is_arithmetic<std::remove_reference_t<T>>::value>> = nullptr>
  static constexpr auto impl(T&& value) noexcept -> std::remove_reference_t<T> {
    return ::arene::base::abs(std::forward<T>(value));
  }

  /// @brief For non-arithmetic types, look up @c abs using ADL
  /// @tparam T Type of the argument; constrained to not be arithmetic
  /// @param value Value to get the absolute value of
  /// @return The absolute value of @c value as determined by an unqualified call to @c abs
  template <typename T, constraints<std::enable_if_t<!std::is_arithmetic<std::remove_reference_t<T>>::value>> = nullptr>
  static constexpr auto impl(T&& value) noexcept(noexcept(abs(std::forward<T>(value))))
      -> decltype(abs(std::forward<T>(value))) {
    return abs(std::forward<T>(value));
  }

 public:
  /// @brief Take the absolute value of the given value if needed; a no-op for unsigned types
  /// @tparam T Type of the value to take the absolute value of
  /// @param value Value to take the absolute value of
  /// @return The absolute value of @c value
  /// @pre If @c T is a signed integer type and @c value is the most negative value of that type,
  /// this crashes with an @c ARENE_PRECONDITION violation
  template <typename T>
  constexpr auto operator()(T&& value) const noexcept(noexcept(impl(std::forward<T>(value))))
      -> decltype(impl(std::forward<T>(value))) {
    return impl(std::forward<T>(value));
  }
};

/// @copydoc arene::base::math_detail::abs_if_needed_impl::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "Unnamed namespace creates a per-TU reference to a single global object."
// parasoft-begin-suppress CERT_CPP-DCL59-a "Unnamed namespace creates a per-TU reference to a single global object."
ARENE_CPP14_INLINE_VARIABLE(abs_if_needed_impl, abs_if_needed);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace math_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_DETAIL_ABS_IF_NEEDED_HPP_

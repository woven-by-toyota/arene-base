// parasoft-suppress AUTOSAR-A2_8_1-a "False positive: complex_l1_norm_impl implements the complex norm"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_COMPLEX_NORM_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_COMPLEX_NORM_HPP_

// IWYU pragma: private, include "arene/base/math.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/math/abs_cpo.hpp"
#include "arene/base/math/complex_decomp.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

namespace complex_norm_detail {

/// @brief Function object class implementing @c arene::base::complex_l1_norm
class complex_l1_norm_impl {
 public:
  /// @brief Compute the L1-norm of a possibly-complex value: |re(x)| + |im(x)|
  /// @tparam T Type of the value to compute the norm of
  /// @param value Value to compute the norm of
  /// @return The sum of the absolute values of the real part of @c value and the imaginary part of @c value
  /// @warning This is not the same operation as the @c norm member function of <c>std::complex</c>.
  template <typename T, constraints<std::enable_if_t<math::is_complex_v<T>>> = nullptr>
  constexpr auto operator()(T const& value) const noexcept(
      noexcept(math::abs(math::real(std::declval<T const&>())) + math::abs(math::imag(std::declval<T const&>())))
  ) -> decltype(math::abs(math::real(value)) + math::abs(math::imag(value))) {
    // parasoft-begin-suppress AUTOSAR-M5_0_4-a "We must allow implicit conversions here to support user-defined types"
    return math::abs(math::real(value)) + math::abs(math::imag(value));
    // parasoft-end-suppress AUTOSAR-M5_0_4-a
  }

  /// @brief Compute the L1-norm of a possibly-complex value: |re(x)| + |im(x)|
  /// @tparam T Type of the value to compute the norm of
  /// @param value Value to compute the norm of
  /// @return The sum of the absolute values of the real part of @c value and the imaginary part of @c value
  /// @warning This is not the same operation as the @c norm member function of <c>std::complex</c>.
  template <typename T, constraints<std::enable_if_t<!math::is_complex_v<T>>> = nullptr>
  constexpr auto operator()(T const& value) const noexcept(noexcept(math::abs(std::declval<T const&>())))
      -> decltype(math::abs(value)) {
    return math::abs(value);
  }
};

}  // namespace complex_norm_detail

// parasoft-begin-suppress AUTOSAR-M7_3_3-a "Unnamed namespace creates a per-TU reference to a single global object."
// parasoft-begin-suppress CERT_CPP-DCL59-a "Unnamed namespace creates a per-TU reference to a single global object."
/// @brief Compute the L1 norm of a possibly complex value: |re(x)| + |im(x)|
/// @warning This is not the same operation as the @c norm member function of <c>std::complex</c>.
ARENE_CPP14_INLINE_VARIABLE(complex_norm_detail::complex_l1_norm_impl, complex_l1_norm);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_COMPLEX_NORM_HPP_

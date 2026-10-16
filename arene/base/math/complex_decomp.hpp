// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_COMPLEX_DECOMP_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_COMPLEX_DECOMP_HPP_

// IWYU pragma: private, include "arene/base/math.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/math/arithmetic_identities.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {
namespace math {

namespace complex_decomp_detail {

// parasoft-begin-suppress CERT_C-EXP37-a "False positive: rule does not mention naming all parameters"
// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: function does not have internal linkage"

/// @brief Dummy declaration of the ADL customization point so the call below will be well-formed but not match
/// @tparam U A type being checked for real overloads
template <typename U>
auto real(U) -> U = delete;

/// @brief Dummy declaration of the ADL customization point so the call below will be well-formed but not match
/// @tparam U A type being checked for imag overloads
template <typename U>
auto imag(U) -> U = delete;

// parasoft-end-suppress CERT_C-EXP37-a-3
// parasoft-end-suppress AUTOSAR-M3_3_2-a

/// @brief Type trait to determine if a type has a const member function called @c real()
/// @tparam T The type to check
template <typename T, typename = constraints<>>
constexpr bool is_member_complex_v{false};

/// @brief Type trait to determine if a type has a const member function called @c real()
/// @tparam T The type to check
template <typename T>
constexpr bool is_member_complex_v<
    T,
    constraints<decltype(std::declval<T const&>().real()), decltype(std::declval<T const&>().imag())>>{true};

/// @brief Type trait to determine if a type has an ADL free function <c>real(T const&)</c>
/// @tparam T The type to check
template <typename T, typename = constraints<>>
constexpr bool is_adl_complex_v{false};

/// @brief Type trait to determine if a type has an ADL free function <c>real(T const&)</c>
/// @tparam T The type to check
template <typename T>
constexpr bool is_adl_complex_v<
    T,
    constraints<
        decltype(real(std::declval<T const&>())),
        decltype(imag(std::declval<T const&>()))>>{!is_member_complex_v<T>};

/// @brief Function object class implementing @c arene::base::math::real
class real_impl {
 public:
  /// @brief Extract the real part of a value, assuming the whole value is real if no @c real function exists
  /// @tparam T Type of the value to extract the real part from
  /// @param value Value to extract the real part from
  /// @return The real part of @c value if it appears to be complex, or @c value itself if it does not
  template <typename T, constraints<std::enable_if_t<is_member_complex_v<T>>> = nullptr>
  constexpr auto operator()(T const& value) const noexcept(noexcept(std::declval<T const&>().real()))
      -> decltype(value.real()) {
    return value.real();
  }

  /// @brief Extract the real part of a value, assuming the whole value is real if no @c real function exists
  /// @tparam T Type of the value to extract the real part from
  /// @param value Value to extract the real part from
  /// @return The real part of @c value if it appears to be complex, or @c value itself if it does not
  template <typename T, constraints<std::enable_if_t<is_adl_complex_v<T>>> = nullptr>
  constexpr auto operator()(T const& value) const noexcept(noexcept(real(std::declval<T const&>())))
      -> decltype(real(value)) {
    return real(value);
  }

  /// @brief Extract the real part of a value, assuming the whole value is real if no @c real function exists
  /// @tparam T Type of the value to extract the real part from
  /// @param value Value to extract the real part from
  /// @return The real part of @c value if it appears to be complex, or @c value itself if it does not
  template <
      typename T,
      constraints<std::enable_if_t<!is_member_complex_v<T>>, std::enable_if_t<!is_adl_complex_v<T>>> = nullptr>
  constexpr auto operator()(T const& value) const noexcept -> T const& {
    return value;
  }
};

/// @brief Function object class implementing @c arene::base::math::imag
class imag_impl {
 public:
  /// @brief Extract the imaginary part of a value, assuming the whole value is real if no @c imag function exists
  /// @tparam T Type of the value to extract the imaginary part from
  /// @param value Value to extract the imaginary part from
  /// @return The imaginary part of @c value if it appears to be complex, or <c>numbers::zero_v<T></c> if it does not
  template <typename T, constraints<std::enable_if_t<is_member_complex_v<T>>> = nullptr>
  constexpr auto operator()(T const& value) const noexcept(noexcept(std::declval<T const&>().imag()))
      -> decltype(value.imag()) {
    return value.imag();
  }

  /// @brief Extract the imaginary part of a value, assuming the whole value is real if no @c imag function exists
  /// @tparam T Type of the value to extract the imaginary part from
  /// @param value Value to extract the imaginary part from
  /// @return The imaginary part of @c value if it appears to be complex, or <c>numbers::zero_v<T></c> if it does not
  template <typename T, constraints<std::enable_if_t<is_adl_complex_v<T>>> = nullptr>
  constexpr auto operator()(T const& value) const noexcept(noexcept(imag(std::declval<T const&>())))
      -> decltype(imag(value)) {
    return imag(value);
  }

  /// @brief Extract the imaginary part of a value, assuming the whole value is real if no @c imag function exists
  /// @tparam T Type of the value to extract the imaginary part from
  /// @return The imaginary part of @c value if it appears to be complex, or <c>numbers::zero_v<T></c> if it does not
  template <
      typename T,
      constraints<std::enable_if_t<!is_member_complex_v<T>>, std::enable_if_t<!is_adl_complex_v<T>>> = nullptr>
  constexpr auto operator()(T const&) const noexcept -> T const& {
    return numbers::zero_v<T>;
  }
};
}  // namespace complex_decomp_detail

// parasoft-begin-suppress AUTOSAR-M7_3_3-a "Unnamed namespace creates a per-TU reference to a single global object."
// parasoft-begin-suppress CERT_CPP-DCL59-a "Unnamed namespace creates a per-TU reference to a single global object."
/// @brief Extract the real part of a value, treating the whole value as real when no @c real function exists
ARENE_CPP14_INLINE_VARIABLE(complex_decomp_detail::real_impl, real);

/// @brief Extract the imaginary part of a value, returning zero when no @c imag function exists
ARENE_CPP14_INLINE_VARIABLE(complex_decomp_detail::imag_impl, imag);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

/// @brief Determine if a type models a complex number or not
/// @tparam T The type to check
///
/// Equal to @c true if either @c T has member functions @c real and @c imag or there are free functions @c real(T) and
/// @c imag(T) which can be found by argument-dependent lookup. Otherwise equal to <c>false</c>.
template <typename T>
constexpr bool
    is_complex_v{complex_decomp_detail::is_member_complex_v<T> || complex_decomp_detail::is_adl_complex_v<T>};

}  // namespace math
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_COMPLEX_DECOMP_HPP_

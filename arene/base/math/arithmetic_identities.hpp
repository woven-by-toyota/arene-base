// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_ARITHMETIC_IDENTITIES_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_ARITHMETIC_IDENTITIES_HPP_

// IWYU pragma: private, include "arene/base/math.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

namespace arene {
namespace base {
namespace numbers {

/// @brief The additive identity of @c T
/// @tparam T numeric type
///
/// Provides access to the additive identity of @c T through its @c static member @c value . This value is the additive
/// identity; <c> a + zero<T>::value == a </c> and <c> zero<T>::value + a == a </c> are @c true for valid values of @c a
/// of type @c T .
///
/// Users may specialize @c zero for cv-unqualified program-defined types. Such specializations shall have a @c static
/// member @c value with type @c T whose value is the additive identity of @c T . A specialization is required if any of
/// the following cases are true for the expression @c T{} :
///   * is not well formed
///   * does not match the required semantics
///   * is not invocable within a constant expression
///
/// Because @c zero_v binds a reference to @c value , every specialization must also provide a namespace-scope
/// definition of @c value (a separate out-of-line definition, as required for ODR use in C++14); otherwise naming
/// @c zero_v for that type is ill-formed.
///
/// ~~~{.cpp}
/// template <>
/// struct arene::base::numbers::zero<my_type> {
///   static constexpr my_type value{};
/// };
/// // Required for ODR use: zero_v<my_type> binds a reference to this member.
/// constexpr my_type arene::base::numbers::zero<my_type>::value;
/// ~~~
template <typename T>
struct zero {
  /// @brief The additive identity value.
  static constexpr T value{};
};

// Separate definition required for ODR use.
template <typename T>
constexpr T zero<T>::value;

/// @brief @c constexpr reference to the additive identity value @c zero<T>::value .
/// @tparam T numeric type
///
/// Binds a @c constexpr reference to @c zero<T>::value , so it is valid for every specialization, including one whose
/// @c value is not usable in a constant expression. Reading the accessor @e within a constant expression additionally
/// requires that @c zero<T>::value itself be usable in a constant expression.
template <typename T>
constexpr decltype(zero<T>::value) const& zero_v{zero<T>::value};

/// @brief The multiplicative identity of @c T
/// @tparam T numeric type
///
/// Provides access to the multiplicative identity of @c T through its @c static member @c value . This value is the
/// multiplicative identity; <c> a * one<T>::value == a </c> and <c> one<T>::value * a == a </c> are @c true for valid
/// values of @c a of type @c T .
///
/// Users may specialize @c one for cv-unqualified program-defined types. Such specializations shall have a @c static
/// member @c value with type @c T whose value is the multiplicative identity of @c T . A specialization is required if
/// any of the following cases are true for the expression @c T{1} :
///   * is not well formed
///   * does not match the required semantics
///   * is not invocable within a constant expression
///
/// Because @c one_v binds a reference to @c value , every specialization must also provide a namespace-scope
/// definition of @c value (a separate out-of-line definition, as required for ODR use in C++14); otherwise naming
/// @c one_v for that type is ill-formed.
///
/// ~~~{.cpp}
/// template <>
/// struct arene::base::numbers::one<my_type> {
///   static constexpr my_type value{1};
/// };
/// // Required for ODR use: one_v<my_type> binds a reference to this member.
/// constexpr my_type arene::base::numbers::one<my_type>::value;
/// ~~~
template <typename T>
struct one {
  /// @brief The multiplicative identity value.
  // parasoft-begin-suppress AUTOSAR-M5_0_5-a CERT_C-EXP39-a "Exact, intended int-to-T identity conversion"
  static constexpr T value{1};
  // parasoft-end-suppress AUTOSAR-M5_0_5-a CERT_C-EXP39-a
};

// Separate definition required for ODR use.
template <typename T>
constexpr T one<T>::value;

/// @brief @c constexpr reference to the multiplicative identity value @c one<T>::value .
/// @tparam T numeric type
///
/// Binds a @c constexpr reference to @c one<T>::value , so it is valid for every specialization, including one whose
/// @c value is not usable in a constant expression. Reading the accessor @e within a constant expression additionally
/// requires that @c one<T>::value itself be usable in a constant expression.
template <typename T>
constexpr decltype(one<T>::value) const& one_v{one<T>::value};

}  // namespace numbers
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_ARITHMETIC_IDENTITIES_HPP_

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
/// <c> a + zero_v<T> == a </c> and <c> zero_v<T> + a == a </c> are @c true for valid values
/// of @c a of type @c T.
///
/// Users may specialize @c zero_v for cv-unqualified program-defined types.
/// A specialization is required if any of the following cases are true for
/// the expression @c T{}:
///   * is not well formed
///   * does not match the required semantics
///   * is not invocable within a constant expression
///
/// A specialization may be non-@c constexpr; such specializations may not
/// be used in contexts that require a constant expression.
///
/// Example of a non-@c constexpr specialization (for a type whose
/// default constructor is not @c constexpr but does have the correct semantics):
///
/// ~~~{.cpp}
/// template <>
/// my_type const arene::base::numbers::zero_v<my_type>{};
/// ~~~
template <typename T>
extern constexpr T zero_v{};

/// @brief The multiplicative identity of @c T
/// @tparam T numeric type
///
/// <c> a * one_v<T> == a </c> and <c> one_v<T> * a == a </c> are @c true for valid values
/// of @c a of type @c T.
///
/// Users may specialize @c one_v for cv-unqualified program-defined types.
/// A specialization is required if any of the following cases are true for
/// the expression @c T{1}:
///   * is not well formed
///   * does not match the required semantics
///   * is not invocable within a constant expression
///
/// @see zero_v
///
template <typename T>
extern constexpr T one_v{1};

}  // namespace numbers
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_ARITHMETIC_IDENTITIES_HPP_

// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_ARITHMETIC_TRAITS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_ARITHMETIC_TRAITS_HPP_

// IWYU pragma: private, include "arene/base/type_traits.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"

namespace arene {
namespace base {

namespace arithmetic_traits_detail {

/// @brief Check if instances of @c Lhs can be added to instances of @c Rhs
/// @tparam Lhs The type of the left-hand operand
/// @tparam Rhs The type of the right-hand operand
template <typename Lhs, typename Rhs, typename = constraints<>>
extern constexpr bool is_addable_v = false;

/// @brief Check if instances of @c Lhs can be added to instances of @c Rhs --- this specialization is for the case
/// where they can be added
/// @tparam Lhs The type of the left-hand operand
/// @tparam Rhs The type of the right-hand operand
template <typename Lhs, typename Rhs>
extern constexpr bool is_addable_v<Lhs, Rhs, constraints<decltype(std::declval<Lhs>() + std::declval<Rhs>())>> = true;

}  // namespace arithmetic_traits_detail

/// @brief Check if instances of @c Lhs can be added to instances of @c Rhs. The value is @c true if they can be added,
/// @c false otherwise.
/// @tparam Lhs The type of the left-hand operand
/// @tparam Rhs The type of the right-hand operand, defaults to @c Lhs
template <typename Lhs, typename Rhs = Lhs>
extern constexpr bool is_addable_v = arithmetic_traits_detail::is_addable_v<Lhs, Rhs>;

namespace arithmetic_traits_detail {

/// @brief Check if instances of @c Lhs can be added to instances of @c Rhs without throwing an exception
/// @tparam Lhs The type of the left-hand operand
/// @tparam Rhs The type of the right-hand operand
template <typename Lhs, typename Rhs, typename = constraints<>>
extern constexpr bool is_nothrow_addable_v = false;

/// @brief Check if instances of @c Lhs can be added to instances of @c Rhs without throwing an exception --- this
/// specialization is for the case where they can be added
/// @tparam Lhs The type of the left-hand operand
/// @tparam Rhs The type of the right-hand operand
template <typename Lhs, typename Rhs>
extern constexpr bool is_nothrow_addable_v<Lhs, Rhs, constraints<std::enable_if_t<is_addable_v<Lhs, Rhs>>>> =
    noexcept(std::declval<Lhs>() + std::declval<Rhs>());

}  // namespace arithmetic_traits_detail

/// @brief Check if instances of @c Lhs can be added to instances of @c Rhs without throwing an exception. The value is
/// @c true if they can be added without throwing, @c false otherwise.
/// @tparam Lhs The type of the left-hand operand
/// @tparam Rhs The type of the right-hand operand, defaults to @c Lhs
template <typename Lhs, typename Rhs = Lhs>
extern constexpr bool is_nothrow_addable_v = arithmetic_traits_detail::is_nothrow_addable_v<Lhs, Rhs>;

namespace arithmetic_traits_detail {

/// @brief Check if instances of @c Rhs can be subtracted from an instance of @c Lhs
/// @tparam Lhs The type of the left-hand operand
/// @tparam Rhs The type of the right-hand operand
template <typename Lhs, typename Rhs, typename = constraints<>>
extern constexpr bool is_subtractable_v = false;

/// @brief Check if instances of @c Rhs can be subtracted from an instance of @c Lhs --- this specialization is for the
/// case where they can be added
/// @tparam Lhs The type of the left-hand operand
/// @tparam Rhs The type of the right-hand operand
template <typename Lhs, typename Rhs>
extern constexpr bool is_subtractable_v<Lhs, Rhs, constraints<decltype(std::declval<Lhs>() - std::declval<Rhs>())>> =
    true;

}  // namespace arithmetic_traits_detail

/// @brief Check if instances of @c Rhs can be subtracted from an instance of @c Lhs. The value is @c true if they can
/// be added, @c false otherwise.
/// @tparam Lhs The type of the left-hand operand
/// @tparam Rhs The type of the right-hand operand, defaults to @c Lhs
template <typename Lhs, typename Rhs = Lhs>
extern constexpr bool is_subtractable_v = arithmetic_traits_detail::is_subtractable_v<Lhs, Rhs>;

namespace arithmetic_traits_detail {

/// @brief Check if instances of @c Rhs can be subtracted from an instance of @c Lhs without throwing an exception
/// @tparam Lhs The type of the left-hand operand
/// @tparam Rhs The type of the right-hand operand
template <typename Lhs, typename Rhs, typename = constraints<>>
extern constexpr bool is_nothrow_subtractable_v = false;

/// @brief Check if instances of @c Rhs can be subtracted from an instance of @c Lhs without throwing an exception ---
/// this specialization is for the case where they can be added
/// @tparam Lhs The type of the left-hand operand
/// @tparam Rhs The type of the right-hand operand
template <typename Lhs, typename Rhs>
extern constexpr bool is_nothrow_subtractable_v<Lhs, Rhs, constraints<std::enable_if_t<is_subtractable_v<Lhs, Rhs>>>> =
    noexcept(std::declval<Lhs>() - std::declval<Rhs>());

}  // namespace arithmetic_traits_detail

/// @brief Check if instances of @c Rhs can be subtracted from an instance of @c Lhs without throwing an exception. The
/// value is @c true if they can be added without throwing, @c false otherwise.
/// @tparam Lhs The type of the left-hand operand
/// @tparam Rhs The type of the right-hand operand, defaults to @c Lhs
template <typename Lhs, typename Rhs = Lhs>
extern constexpr bool is_nothrow_subtractable_v = arithmetic_traits_detail::is_nothrow_subtractable_v<Lhs, Rhs>;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_ARITHMETIC_TRAITS_HPP_

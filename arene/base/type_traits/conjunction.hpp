// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_CONJUNCTION_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_CONJUNCTION_HPP_

// IWYU pragma: private, include "arene/base/type_traits.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/stdlib_choice/conditional.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"

namespace arene {
namespace base {

// parasoft-begin-suppress AUTOSAR-A2_7_3-a "False positive: all declarations are documented."
/// @brief Combines the values of multiple boolean type traits with a logical AND operation
/// @tparam BoolTraits the traits to combine
/// @note Evaluation short circuits: If @c bool(BoolTrait[N]::value) is @c false , then the remaining
/// @c BoolTraits::value... are not evaluated. The resulting type inherits from either the first @c BoolTraits[N] whose
/// value is @c false , or the final type in @c BoolTraits... if no such trait exists. If @c BoolTraits is empty,
/// then the type is @c std::true_type .
template <typename...>
class conjunction : public std::true_type {};

/// @brief Specialization of @c arene::base::conjunction for a single trait. The result is that trait.
/// @tparam BoolTrait The trait to evaluate
template <typename BoolTrait>
class conjunction<BoolTrait> : public BoolTrait {};

/// @brief Specialization of @c arene::base::conjunction for multiple traits.
/// @tparam BoolTrait The first trait to evaluate
/// @tparam Rest The remaining traits to evaluate
template <typename BoolTrait, typename... Rest>
class conjunction<BoolTrait, Rest...>
    : public std::conditional_t<bool(BoolTrait::value), conjunction<Rest...>, BoolTrait> {};

/// @brief  The combined value of multiple boolean type traits
/// @tparam BoolTraits the traits to combine
/// @returns bool Equivalent to <c> BoolTraits::value && ... </c> .
/// @note Evaluation short circuits: If @c bool(BoolTrait[N]::value) is @c false , then the remaining
/// @c BoolTraits::value... are not evaluated.
template <typename... BoolTraits>
extern constexpr bool conjunction_v{conjunction<BoolTraits...>::value};
// parasoft-end-suppress AUTOSAR-A2_7_3-a "False positive: all declarations are documented."

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_CONJUNCTION_HPP_

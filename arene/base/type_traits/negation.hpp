// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_NEGATION_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_NEGATION_HPP_

// IWYU pragma: private, include "arene/base/type_traits.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/stdlib_choice/integral_constant.hpp"

namespace arene {
namespace base {

// parasoft-begin-suppress AUTOSAR-A2_7_3-a "False positive: all declarations are documented."
/// @brief  Negates the value of a boolean type trait
/// @tparam BoolTrait the trait to negate
template <typename BoolTrait>
class negation : public std::integral_constant<bool, !bool(BoolTrait::value)> {};

/// @brief  The negated value of a boolean type trait
/// @tparam BoolTrait the trait to negate
/// @returns bool Equivalent to @c !BoolTrait::value .
template <typename BoolTrait>
extern constexpr bool negation_v{negation<BoolTrait>::value};

// parasoft-end-suppress AUTOSAR-A2_7_3-a "False positive: all declarations are documented."

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_NEGATION_HPP_

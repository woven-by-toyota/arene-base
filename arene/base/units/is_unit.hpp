// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_IS_UNIT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_IS_UNIT_HPP_

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/units/is_quantity_kind.hpp"

namespace arene {
namespace base {

namespace is_unit_detail {

/// @brief Type trait for detecting if a type is a unit. Evaluates to @c true if the type is a unit, @c false otherwise
/// @tparam Kind the type to check
template <typename Kind, typename = constraints<>>
extern constexpr bool is_unit_v = false;

/// @brief Type trait for detecting if a type is a unit. Evaluates to @c true if the type is a unit, @c false otherwise
/// @tparam Kind the type to check
template <typename Kind>
extern constexpr bool is_unit_v<Kind, constraints<typename Kind::unit_kind_type>> = is_quantity_kind_v<Kind>;

}  // namespace is_unit_detail

/// @brief Type trait for detecting if a type is a unit. Evaluates to @c true if the type is a unit, @c false otherwise
/// @tparam Kind the type to check
template <typename Kind>
extern constexpr bool is_unit_v = is_unit_detail::is_unit_v<Kind>;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_IS_UNIT_HPP_

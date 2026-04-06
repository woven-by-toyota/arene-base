// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_IS_POSSIBLE_UNIT_FOR_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_IS_POSSIBLE_UNIT_FOR_HPP_

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/units/has_parent_kind.hpp"
#include "arene/base/units/is_quantity_kind.hpp"
#include "arene/base/units/is_unit.hpp"

namespace arene {
namespace base {

namespace units_detail {
/// @brief Type trait for detecting if a type is explicitly declared to be applicable for a specific quantity
/// kind. Evaluates to @c true if @c Unit is a unit for @c Kind, @c false otherwise
/// @tparam Unit the type to check
/// @tparam Kind the type to check if the type is a unit for
template <typename Unit, typename Kind, typename = constraints<>>
extern constexpr bool is_declared_possible_unit_for_v = false;

/// @brief Type trait for detecting if a type is explicitly declared to be applicable for a specific quantity kind.
/// Evaluates to @c true if @c Unit is a unit for @c Kind, @c false otherwise
/// @tparam Unit the type to check
/// @tparam Kind the type to check if the type is a unit for
template <typename Unit, typename Kind>
extern constexpr bool is_declared_possible_unit_for_v<
    Unit,
    Kind,
    constraints<std::enable_if_t<std::is_same<Kind, typename Unit::unit_kind_type>::value>>> = true;
}  // namespace units_detail

namespace is_possible_unit_for_detail {

/// @brief Type trait for detecting if a type is a unit for a specific quantity kind. Evaluates to @c true if @c Unit
/// is a unit for @c Kind, @c false otherwise
/// @tparam Unit the type to check
/// @tparam Kind the type to check if the type is a unit for
template <typename Unit, typename Kind, typename = constraints<>>
extern constexpr bool is_possible_unit_for_v = false;

/// @brief Type trait for detecting if a type is a unit for a specific quantity kind. Evaluates to @c true if @c Unit
/// is a unit for @c Kind, @c false otherwise
/// @tparam Unit the type to check
/// @tparam Kind the type to check if the type is a unit for
template <typename Unit, typename Kind>
extern constexpr bool is_possible_unit_for_v<
    Unit,
    Kind,
    constraints<std::enable_if_t<units_detail::is_declared_possible_unit_for_v<Unit, Kind>>>> =
    is_quantity_kind_v<Unit>;

/// @brief Type trait for detecting if a type is a unit for a specific quantity kind. Evaluates to @c true if @c Unit
/// is a unit for @c Kind, @c false otherwise
/// @tparam Unit the type to check
/// @tparam Kind the type to check if the type is a unit for
template <typename Unit, typename Kind>
extern constexpr bool is_possible_unit_for_v<
    Unit,
    Kind,
    constraints<
        std::enable_if_t<!units_detail::is_declared_possible_unit_for_v<Unit, Kind>>,
        std::enable_if_t<!is_unit_v<typename Unit::unit_kind_type>>,
        std::enable_if_t<units_detail::has_parent_kind_v<Kind>>>> =
    is_quantity_kind_v<Unit> && is_possible_unit_for_v<Unit, typename Kind::parent_quantity_kind_type>;

/// @brief Type trait for detecting if a type is a unit for a specific quantity kind. Evaluates to @c true if @c Unit
/// is a unit for @c Kind, @c false otherwise
/// @tparam Unit the type to check
/// @tparam Kind the type to check if the type is a unit for
template <typename Unit, typename Kind>
extern constexpr bool is_possible_unit_for_v<
    Unit,
    Kind,
    constraints<
        std::enable_if_t<!units_detail::is_declared_possible_unit_for_v<Unit, Kind>>,
        std::enable_if_t<is_unit_v<typename Unit::unit_kind_type>>>> =
    is_possible_unit_for_v<typename Unit::unit_kind_type, Kind>;

}  // namespace is_possible_unit_for_detail

/// @brief Type trait for detecting if a type is a unit for a specific quantity kind. Evaluates to @c true if @c
/// Unit is a unit for @c Kind, @c false otherwise
/// @tparam Unit the type to check
/// @tparam Kind the type to check if the type is a unit for
template <typename Unit, typename Kind>
extern constexpr bool is_possible_unit_for_v = is_possible_unit_for_detail::is_possible_unit_for_v<Unit, Kind>;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_IS_POSSIBLE_UNIT_FOR_HPP_

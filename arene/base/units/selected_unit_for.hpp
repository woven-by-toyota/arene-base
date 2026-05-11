// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_SELECTED_UNIT_FOR_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_SELECTED_UNIT_FOR_HPP_

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/units/has_explicit_units.hpp"
#include "arene/base/units/has_parent_kind.hpp"
#include "arene/base/units/is_quantity_kind.hpp"
#include "arene/base/units/is_unit.hpp"

namespace arene {
namespace base {

namespace selected_unit_for_detail {
/// @brief Type trait for determining the unit for a quantity kind. If there is a unit for a given kind, then the @c
/// type member is a type alias for the unit type. Otherwise, the @c type member is not present.
/// @tparam Kind the type to check
template <typename Kind, typename = constraints<>>
struct selected_unit_for_impl;

/// @brief Type trait for determining the unit for a quantity kind. If there is a unit for a given kind, then the @c
/// type member is a type alias for the unit type. Otherwise, the @c type member is not present.
/// @tparam Kind the type to check
template <typename Kind>
struct selected_unit_for_impl<Kind, constraints<std::enable_if_t<is_unit_v<Kind>>>> {
  /// @brief A type alias for the unit type for @c Kind
  using type = Kind;
};

/// @brief Type trait for determining the unit for a quantity kind. If there is a unit for a given kind, then the @c
/// type member is a type alias for the unit type. Otherwise, the @c type member is not present.
/// @tparam Kind the type to check
template <typename Kind>
struct selected_unit_for_impl<
    Kind,
    constraints<
        std::enable_if_t<!is_unit_v<Kind>>,
        std::enable_if_t<is_quantity_kind_v<Kind>>,
        std::enable_if_t<units_detail::has_explicit_units_v<Kind>>>> {
  /// @brief A type alias for the unit type for @c Kind
  using type = typename Kind::unit_type;
};

/// @brief Type trait for determining the unit for a quantity kind. If there is a unit for a given kind, then the @c
/// type member is a type alias for the unit type. Otherwise, the @c type member is not present.
/// @tparam Kind the type to check
template <typename Kind>
struct selected_unit_for_impl<
    Kind,
    constraints<
        std::enable_if_t<!is_unit_v<Kind>>,
        std::enable_if_t<is_quantity_kind_v<Kind>>,
        std::enable_if_t<!units_detail::has_explicit_units_v<Kind>>,
        std::enable_if_t<units_detail::has_parent_kind_v<Kind>>>> {
  /// @brief A type alias for the unit type for @c Kind
  using type = typename selected_unit_for_impl<typename Kind::parent_quantity_kind_type>::type;
};

}  // namespace selected_unit_for_detail

/// @brief Type trait for determining the unit for a quantity kind. If there is a unit for a given kind, then it is a
/// type alias for the unit type. Otherwise, it is undefined.
/// @tparam Kind the type to check
template <typename Kind>
using selected_unit_for_t = typename selected_unit_for_detail::selected_unit_for_impl<std::remove_cv_t<Kind>>::type;

/// @brief Type trait for determining if the given quantity kind has a unit selected. It is @c true if a unit
/// is selected, @c false otherwise
/// @tparam Kind the quantity kind to check
template <typename Kind, typename = arene::base::constraints<>>
extern constexpr bool has_selected_unit_v = false;

/// @brief Type trait for determining if the given quantity kind has a unit selected. It is @c true if a unit
/// is selected, @c false otherwise
/// @tparam Kind the quantity kind to check
template <typename Kind>
extern constexpr bool has_selected_unit_v<Kind, arene::base::constraints<selected_unit_for_t<Kind>>> = true;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_SELECTED_UNIT_FOR_HPP_

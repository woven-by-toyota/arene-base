// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_QUANTITY_UNIT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_QUANTITY_UNIT_HPP_

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/units/detail/missing.hpp"
#include "arene/base/units/is_possible_unit_for.hpp"
#include "arene/base/units/quantity_kind.hpp"

namespace arene {
namespace base {

namespace quantity_unit_detail {

/// @brief implementation class for a quantity kind with a rebound unit
/// @tparam Kind quantity kind to rebind
/// @tparam Unit the unit type to rebind with
template <typename Kind, typename Unit>
class quantity_kind_with_rebound_unit
    : public quantity_kind<quantity_kind_with_rebound_unit<Kind, Unit>, Kind, Unit> {};

}  // namespace quantity_unit_detail

/// @brief Base class for declaring a quantity unit
/// @tparam Tag the derived class which represents the quantity unit
/// @tparam Kind The quantity kind for which this is a unit
///
/// @pre @c Tag must derive from @c quantity_unit<Tag>
template <typename Tag, typename Kind>
class quantity_unit
    : public quantity_kind_detail::quantity_kind_base<Tag, Kind, units_detail::missing, units_detail::missing> {
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Use of friend permitted for hidden friend idiom by A11-3-1 Permit #2
  // v1.0.0"
  //
  // parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: cannot apply static to a hidden friend function"
  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: rule does not mention naming all parameters"

  /// @brief helper used to rebind the unit type of a quantity kind
  /// @tparam OtherDerived derived quantity kind type to rebind
  /// @tparam OtherParentKind parent kind of the quantity type to rebind
  /// @tparam OtherUnit unit of the quantity type to rebind
  /// @return different quantity kind with the unit specified to be @c Tag
  /// @note Requires @c is_possible_unit_for<Tag, OtherDerived> to be @c true
  template <
      typename OtherDerived,
      typename OtherParentKind,
      typename OtherOrigin,
      typename OtherUnit,
      constraints<std::enable_if_t<is_possible_unit_for_v<Tag, OtherDerived>>> = nullptr>
  friend auto
  rebind_quantity_kind_unit_type(quantity_kind_detail::quantity_kind_base<OtherDerived, OtherParentKind, OtherOrigin, OtherUnit> const&, Tag const&)
      -> quantity_unit_detail::quantity_kind_with_rebound_unit<OtherDerived, Tag> {
    return {};
  }

  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2
  // parasoft-end-suppress AUTOSAR-M3_3_2-a
  // parasoft-end-suppress CERT_C-EXP37-a-3

 public:
  /// @brief Type alias to indicate the quantity kind for which this is a unit
  using unit_kind_type = Kind;
};

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_QUANTITY_UNIT_HPP_

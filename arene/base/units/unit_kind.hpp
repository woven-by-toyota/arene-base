// parasoft-begin-suppress AUTOSAR-A2_8_1-a "The helper class is an implementation detail of unit_kind_t"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_UNIT_KIND_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_UNIT_KIND_HPP_

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/units/is_unit.hpp"

namespace arene {
namespace base {

namespace unit_kind_detail {

/// @brief Helper template for finding the underlying quantity kind for a unit
/// @tparam Unit The unit to get the quantity kind for
template <typename Unit, typename = constraints<>>
class unit_kind_helper {
 public:
  /// @brief The quantity kind for the unit
  using type = typename Unit::unit_kind_type;
};

/// @brief Helper template for finding the underlying quantity kind for a unit
/// @tparam Unit The unit to get the quantity kind for
template <typename Unit>
class unit_kind_helper<Unit, constraints<std::enable_if_t<is_unit_v<typename Unit::unit_kind_type>>>> {
 public:
  /// @brief The quantity kind for the unit
  using type = typename unit_kind_helper<typename Unit::unit_kind_type>::type;
};

}  // namespace unit_kind_detail

/// @brief Alias template for the underlying quantity kind for a unit
/// @tparam Unit The unit to get the quantity kind for
template <typename Unit>
using unit_kind_t = typename unit_kind_detail::unit_kind_helper<Unit>::type;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_UNIT_KIND_HPP_

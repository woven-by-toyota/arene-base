// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_HAS_PARENT_KIND_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_HAS_PARENT_KIND_HPP_

#include "arene/base/constraints/constraints.hpp"

namespace arene {
namespace base {

namespace units_detail {
/// @brief Type trait to check if a quantity kind has a parent kind. The value is @c true if it does, @c false otherwise
/// @tparam Kind the quantity kind to check
template <typename Kind, typename = constraints<>>
extern constexpr bool has_parent_kind_v = false;

/// @brief Type trait to check if a quantity kind has a parent kind. The value is @c true if it does, @c false otherwise
/// @tparam Kind the quantity kind to check
template <typename Kind>
extern constexpr bool has_parent_kind_v<Kind, constraints<typename Kind::parent_quantity_kind_type>> = true;
}  // namespace units_detail

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_HAS_PARENT_KIND_HPP_

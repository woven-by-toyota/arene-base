// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_HAS_EXPLICIT_UNITS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_HAS_EXPLICIT_UNITS_HPP_

#include "arene/base/constraints/constraints.hpp"

namespace arene {
namespace base {
namespace units_detail {

/// @brief Implementation helper to detect if the type has the @c unit_type member
/// @tparam Kind the type to check
template <typename Kind, typename = constraints<>>
extern constexpr bool has_explicit_units_v = false;

/// @brief Implementation helper to detect if the type has the @c unit_type member
/// @tparam Kind the type to check
template <typename Kind>
extern constexpr bool has_explicit_units_v<Kind, constraints<typename Kind::unit_type>> = true;

}  // namespace units_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_HAS_EXPLICIT_UNITS_HPP_

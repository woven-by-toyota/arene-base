// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_IS_KIND_WITH_EXPONENT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_IS_KIND_WITH_EXPONENT_HPP_

#include "arene/base/units/kind_with_exponent_fwd.hpp"

namespace arene {
namespace base {

namespace units_detail {
/// @brief Type trait to check if a given type is a quantity kind with an exponent. Is @c true if it is, @c false
/// otherwise
/// @tparam Kind The type to check
template <typename Kind>
extern constexpr bool is_kind_with_exponent_v = false;

/// @brief Type trait to check if a given type is a quantity kind with an exponent. Is @c true if it is, @c false
/// otherwise
/// @tparam Kind The base quantity kind
/// @tparam PositiveExponent The positive exponent
/// @tparam NegativeExponent The negative exponent
template <typename Kind, exponent_t PositiveExponent, exponent_t NegativeExponent>
extern constexpr bool is_kind_with_exponent_v<quantity_kind_with_exponent<Kind, PositiveExponent, NegativeExponent>> =
    true;

}  // namespace units_detail

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_IS_KIND_WITH_EXPONENT_HPP_

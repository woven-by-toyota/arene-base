// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_KIND_WITH_EXPONENT_FWD_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_KIND_WITH_EXPONENT_FWD_HPP_

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"

namespace arene {
namespace base {

namespace units_detail {

/// @brief The integer type to use for the representation of exponents
using exponent_t = std::int64_t;

// IWYU pragma: begin_keep
/// @brief Helper class to determine the result of raising the specified quantity kind to the specified positive and
/// negative exponents. Both @c PositiveExponent and @c NegativeExponent must be positive. The effective exponent is @c
/// PositiveExponent - @c NegativeExponent however they are stored separately to allow distinguishing @c m/m from @c s/s
/// or @c m^2/m^2
/// @tparam Kind The quantity kind to raise to an exponent
/// @tparam PositiveExponent The positive exponent to raise it to
/// @tparam NegativeExponent The negative exponent to raise it to
template <typename Kind, exponent_t PositiveExponent, exponent_t NegativeExponent, typename = constraints<>>
class kind_with_exponent_helper;

/// @brief Implementation class for a quantity kind raised to the specified positive and
/// negative exponents. Both @c PositiveExponent and @c NegativeExponent must be positive. The effective exponent is @c
/// PositiveExponent - @c NegativeExponent however they are stored separately to allow distinguishing @c m/m from @c s/s
/// or @c m^2/m^2
/// @tparam Kind The quantity kind to raise to an exponent
/// @tparam PositiveExponent The positive exponent to raise it to
/// @tparam NegativeExponent The negative exponent to raise it to
template <typename Kind, exponent_t PositiveExponent, exponent_t NegativeExponent, typename = constraints<>>
class quantity_kind_with_exponent;
// IWYU pragma: end_keep

}  // namespace units_detail

/// @brief Alias template for raising the specified quantity kind to the specified exponent
/// @tparam Kind The quantity kind to raise to an exponent
/// @tparam Exponent The exponent to raise it to
template <typename Kind, units_detail::exponent_t Exponent>
using kind_with_exponent_t = typename units_detail::kind_with_exponent_helper<
    std::remove_cv_t<Kind>,
    (Exponent < 0) ? 0 : Exponent,
    (Exponent < 0) ? -Exponent : 0>::type;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_KIND_WITH_EXPONENT_FWD_HPP_

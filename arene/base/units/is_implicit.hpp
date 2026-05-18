// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_IS_IMPLICIT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_IS_IMPLICIT_HPP_

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/units/is_quantity_kind.hpp"

namespace arene {
namespace base {
namespace units_detail {

namespace is_implicit_detail {

/// @brief helper used for detecting an implicit quantity kind
/// @tparam Type the type to check
template <typename Type, typename = constraints<>>
extern constexpr bool provides_is_implicit_typedef_v = false;

/// @brief helper used for detecting an implicit quantity kind
/// @tparam Type the type to check.
template <typename Type>
extern constexpr bool provides_is_implicit_typedef_v<Type, constraints<typename Type::is_implicit>> = true;

}  // namespace is_implicit_detail

/// @brief Type trait for detecting an implicit quantity kind.
/// @tparam Type the type to check
///
/// Evaluates to @c true if the type is a quantity kind which provides an
/// unspecified @c is_implicit type alias, @c false otherwise
template <typename Type>
extern constexpr bool is_implicit_v =
    is_quantity_kind_v<Type> && is_implicit_detail::provides_is_implicit_typedef_v<Type>;

}  // namespace units_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_IS_IMPLICIT_HPP_

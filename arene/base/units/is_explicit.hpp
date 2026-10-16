// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_IS_EXPLICIT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_IS_EXPLICIT_HPP_

#include "arene/base/units/is_implicit.hpp"
#include "arene/base/units/is_quantity_kind.hpp"

namespace arene {
namespace base {
namespace units_detail {

/// @brief Type trait for detecting an explicit quantity kind.
/// @tparam Type the type to check
///
/// Evaluates to @c true if the type is a quantity kind which does not provide
/// an unspecified @c is_implicit type alias, @c false otherwise
template <typename Type>
extern constexpr bool is_explicit_v = is_quantity_kind_v<Type> && !is_implicit_v<Type>;

}  // namespace units_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_IS_EXPLICIT_HPP_

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_IS_QUANTITY_KIND_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_IS_QUANTITY_KIND_HPP_

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"

namespace arene {
namespace base {

namespace is_quantity_kind_detail {

/// @brief Type trait for detecting a quantity kind
/// @tparam Type the type to check
template <typename Type, typename = constraints<>>
extern constexpr bool is_quantity_kind_v = false;

/// @brief Type trait for detecting a quantity kind
/// @tparam Type the type to check.
template <typename Type>
extern constexpr bool is_quantity_kind_v<
    Type,
    constraints<std::enable_if_t<std::is_same<Type, typename Type::quantity_kind_type>::value>>> = true;

}  // namespace is_quantity_kind_detail

/// @brief Type trait for detecting a quantity kind. Evaluates to @c true if the type is a quantity kind, @c false
/// otherwise
/// @tparam Type the type to check
template <typename Type>
extern constexpr bool is_quantity_kind_v = is_quantity_kind_detail::is_quantity_kind_v<std::remove_cv_t<Type>>;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_IS_QUANTITY_KIND_HPP_

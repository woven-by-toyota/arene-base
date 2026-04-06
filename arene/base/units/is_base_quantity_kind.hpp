// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_IS_BASE_QUANTITY_KIND_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_IS_BASE_QUANTITY_KIND_HPP_

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/units/is_quantity_kind.hpp"

namespace arene {
namespace base {

namespace is_base_quantity_kind_detail {
/// @brief Type trait for checking if one type is a base quantity kind for another. Evaluates to @c true if the first
/// type is a base quantity kind of the second (the second is a derived quantity kind of the first), @c false  otherwise
/// @tparam BaseKind the base kind
/// @tparam Type the type to check.
template <typename BaseKind, typename Type, typename = constraints<>>
extern constexpr bool is_base_quantity_kind_of_v = false;

/// @brief Type trait for checking if one type is a base quantity kind for another. Evaluates to @c true if the first
/// type is a base quantity kind of the second (the second is a derived quantity kind of the first), @c false  otherwise
/// @tparam BaseKind the base kind
/// @tparam Type the type to check.
template <typename BaseKind, typename Type>
extern constexpr bool is_base_quantity_kind_of_v<
    BaseKind,
    Type,
    constraints<
        std::enable_if_t<is_quantity_kind_v<BaseKind>>,
        std::enable_if_t<is_quantity_kind_v<Type>>,
        std::enable_if_t<std::is_same<typename Type::parent_quantity_kind_type, BaseKind>::value>>> = true;

/// @brief Type trait for checking if one type is a base quantity kind for another. Evaluates to @c true if the first
/// type is a base quantity kind of the second (the second is a derived quantity kind of the first), @c false  otherwise
/// @tparam BaseKind the base kind
/// @tparam Type the type to check.
template <typename BaseKind, typename Type>
extern constexpr bool is_base_quantity_kind_of_v<
    BaseKind,
    Type,
    constraints<
        std::enable_if_t<is_quantity_kind_v<BaseKind>>,
        std::enable_if_t<is_quantity_kind_v<Type>>,
        std::enable_if_t<!std::is_same<typename Type::parent_quantity_kind_type, BaseKind>::value>,
        std::enable_if_t<is_base_quantity_kind_of_v<BaseKind, typename Type::parent_quantity_kind_type>>>> = true;

}  // namespace is_base_quantity_kind_detail

/// @brief Type trait for checking if one type is a base quantity kind for another. Evaluates to @c true if the first
/// type is a base quantity kind of the second (the second is a derived quantity kind of the first), @c false  otherwise
///
/// Note: a quantity kind is not considered to be a base quantity for itself, so @c is_base_quantity_kind_of_v<T,T> will
/// always be @c false, for all types @c T
///
/// @tparam BaseKind the base kind
/// @tparam Type the type to check.
template <typename BaseKind, typename Type>
extern constexpr bool is_base_quantity_kind_of_v =
    is_base_quantity_kind_detail::is_base_quantity_kind_of_v<BaseKind, Type>;

/// @brief Type trait for checking if one type is a base quantity kind for another, or the same type. Evaluates to @c
/// true if the first type is the same as the second, or a base quantity kind of the second (the second is a derived
/// quantity kind of the first), @c false  otherwise
///
/// @tparam BaseKind the base kind
/// @tparam Type the type to check.
template <typename BaseKind, typename Type>
extern constexpr bool is_base_quantity_kind_of_or_same_as_v =
    is_base_quantity_kind_of_v<BaseKind, Type> || std::is_same<BaseKind, Type>::value;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_IS_BASE_QUANTITY_KIND_HPP_

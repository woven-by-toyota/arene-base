// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_DETAIL_IS_DEREFERENCEABLE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_DETAIL_IS_DEREFERENCEABLE_HPP_

// IWYU pragma: private, include "arene/base/iterator.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/iterator/detail/can_reference.hpp"
#include "arene/base/stdlib_choice/declval.hpp"

namespace arene {
namespace base {
namespace iterator_detail {

namespace is_dereferenceable_detail {
/// @brief Helper alias to check if a type has a valid dereference operator.
/// @tparam Itr The type to check.
template <typename T>
using use_dereference = decltype(*std::declval<T&>());

}  // namespace is_dereferenceable_detail

/// @brief An exposition-only trait which tests if a given type has a valid dereference operator.
/// @tparam T The type to check.
/// @return bool If @c *std::declval<T&>() is a valid expression and yields a type @c U for which the expression @c U&
/// is well-formed, then @c true, else @c false .
template <typename T, bool = substitution_succeeds<is_dereferenceable_detail::use_dereference, T>>
extern constexpr bool is_dereferenceable_v = false;

/// @brief An exposition-only trait which tests if a given type has a valid dereference operator.
/// @tparam T The type to check.
/// @return bool If @c *std::declval<T&>() is a valid expression and yields a type @c U for which the expression @c U&
/// is well-formed, then @c true, else @c false .
template <typename T>
extern constexpr bool is_dereferenceable_v<T, true> = can_reference_v<is_dereferenceable_detail::use_dereference<T>>;

}  // namespace iterator_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_DETAIL_IS_DEREFERENCEABLE_HPP_

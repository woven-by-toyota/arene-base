// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_DETAIL_HAS_MEMBER_ELEMENT_TYPE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_DETAIL_HAS_MEMBER_ELEMENT_TYPE_HPP_

// IWYU pragma: private, include "arene/base/iterator.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/constraints/substitution_succeeds.hpp"

namespace arene {
namespace base {
namespace iterator_detail {

namespace has_member_element_type_detail {
/// @brief Helper alias to check if a type has a member type named @c element_type.
/// @tparam T The type to check.
template <typename T>
using use_member_element_type = typename T::element_type;
}  // namespace has_member_element_type_detail

/// @brief An exposition-only trait which tests if a given type has a member type named @c element_type.
/// @tparam T The type to check.
/// @return bool If @c T has a member type named @c element_type, then @c true, else @c false .
template <typename T>
extern constexpr bool has_member_element_type_v =
    substitution_succeeds<has_member_element_type_detail::use_member_element_type, T>;

}  // namespace iterator_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_DETAIL_HAS_MEMBER_ELEMENT_TYPE_HPP_

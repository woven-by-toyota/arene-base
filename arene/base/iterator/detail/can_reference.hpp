// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_DETAIL_CAN_REFERENCE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_DETAIL_CAN_REFERENCE_HPP_

// IWYU pragma: private, include "arene/base/iterator.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/constraints/substitution_succeeds.hpp"

namespace arene {
namespace base {
namespace iterator_detail {

namespace can_reference_detail {

/// @brief Helper alias to obtain a reference to a type.
/// @tparam T The type to obtain a reference to.
/// @return T& A reference to the type T.
template <typename T>
using take_reference = T&;

}  // namespace can_reference_detail

/// @brief An exposition-only trait which tests if a given type can be referenced.
/// @tparam T The type to check.
/// @return bool If the expression @c T& is well-formed, then @c true, else @c false .
template <typename T>
extern constexpr bool can_reference_v = substitution_succeeds<can_reference_detail::take_reference, T>;

}  // namespace iterator_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_DETAIL_CAN_REFERENCE_HPP_

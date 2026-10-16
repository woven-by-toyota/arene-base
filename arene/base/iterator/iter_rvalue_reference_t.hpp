// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::iter_rvalue_reference_t"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_ITER_RVALUE_REFERENCE_T_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_ITER_RVALUE_REFERENCE_T_HPP_

// IWYU pragma: private, include "arene/base/iterator.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/iterator/detail/can_reference.hpp"
#include "arene/base/iterator/detail/is_dereferenceable.hpp"
#include "arene/base/iterator/iter_move.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"

namespace arene {
namespace base {

namespace iter_rvalue_reference_detail {
/// @brief Determines the type returned by invoking @c ranges::iter_move on the given iterator.
/// @tparam Itr The iterator type
template <typename Itr>
using iter_move_t = decltype(ranges::iter_move(std::declval<Itr&>()));

/// @brief Helper struct to determine the rvalue-reference type of the given iterator.
/// @tparam Itr The iterator type
/// The specialization case for when the iterator is not dereferenceable or its iter_move result cannot be referenced.
template <typename Itr, typename = constraints<>>
class iter_rvalue_reference_t;

/// @brief Helper struct to determine the rvalue-reference type of the given iterator.
/// @tparam Itr The iterator type
/// The specialization case for when the iterator is dereferenceable and its iter_move result can be referenced.
template <typename Itr>
class iter_rvalue_reference_t<
    Itr,
    constraints<
        std::enable_if_t<iterator_detail::is_dereferenceable_v<Itr>>,
        std::enable_if_t<iterator_detail::can_reference_v<iter_move_t<Itr>>>>> {
 public:
  /// @brief The rvalue-reference type of the given iterator.
  using type = iter_move_t<Itr>;
};
}  // namespace iter_rvalue_reference_detail

/// @brief Determins the rvalue-reference type of the given iterator.
/// @tparam Itr The iterator type. Must satisfy @c is_dereferenceable_v<Itr> (see below).
///
/// Let @c can_reference_v<T> be an exposition only trait with the following definition:
///
/// If @c T is a type for which the expression @c T& is well-formed, then @c true, else @c false .
///
/// Let @c is_dereferenceable_v<Itr> be an exposition only trait with the following definition:
///
/// If dereferencing an lvalue of type @c Itr is valid and yields a type @c U, its value is that of @c can_reference_v
/// applied to @c U; otherwise, it is @c false.
///
///
/// Let @c iter_mv_t be the type of the expression @c arene::base::ranges::iter_move(std::declval<Itr&>()) . If both
/// @c is_dereferenceable_v<Itr> is satisfied and @c can_reference_v<iter_mv_t> is satisfied, then
/// resulting type of @c iter_rvalue_reference_t will be equal to @c iter_mv_t , otherwise it will be ill-formed.
template <typename Itr>
using iter_rvalue_reference_t = typename iter_rvalue_reference_detail::iter_rvalue_reference_t<Itr>::type;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_ITER_RVALUE_REFERENCE_T_HPP_

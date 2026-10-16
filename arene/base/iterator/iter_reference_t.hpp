// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::iter_reference_t"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_ITER_REFERENCE_T_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_ITER_REFERENCE_T_HPP_

// IWYU pragma: private, include "arene/base/iterator.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/iterator/detail/is_dereferenceable.hpp"
#include "arene/base/stdlib_choice/declval.hpp"

namespace arene {
namespace base {

namespace iter_reference_detail {
/// @brief Helper struct to determine the reference type of the given iterator.
/// @tparam Itr The iterator type
/// Specialization for when the iterator is not dereferenceable.
template <typename Itr, bool = iterator_detail::is_dereferenceable_v<Itr>>
class iter_reference_impl;

/// @brief Helper struct to determine the reference type of the given iterator.
/// @tparam Itr The iterator type
/// Specialization for when the iterator is dereferenceable.
template <typename Itr>
class iter_reference_impl<Itr, true> {
 public:
  /// @brief The reference type of the given iterator.
  using type = decltype(*std::declval<Itr&>());
};
}  // namespace iter_reference_detail

/// @brief Determins the reference type of the given iterator.
/// @tparam Itr The iterator type. Must satisfy @c is_dereferenceable_v<Itr> (see below).
///
/// Let @c is_dereferenceable_v<Itr> be an exposition only trait with the following definition:
///
/// If @c *std::declval<T&>() is a valid expression and yields a type @c U for which the expression @c U&
/// is well-formed, then @c true, else @c false .
///
/// If @c is_dereferenceable_v<Itr> is satisfied, then resulting type of @c iter_reference_t will be equal to
/// @c decltype(*std::declval<Itr&>()) , otherwise it will be ill-formed.
template <typename Itr>
using iter_reference_t = typename iter_reference_detail::iter_reference_impl<Itr>::type;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_ITER_REFERENCE_T_HPP_

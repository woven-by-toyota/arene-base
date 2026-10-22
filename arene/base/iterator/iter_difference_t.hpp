// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::iter_difference_t"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_ITER_DIFFERENCE_T_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_ITER_DIFFERENCE_T_HPP_

// IWYU pragma: private, include "arene/base/iterator.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/iterator/incrementable_traits.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

namespace arene {
namespace base {

namespace iter_difference_t_detail {

/// @brief Helper variable template to determine if @c std::iterator_traits<Itr>::difference_type exists.
/// @tparam Itr The iterator type to check.
template <typename Itr>
using use_iterator_traits_difference_type = typename std::iterator_traits<Itr>::difference_type;

/// @brief Helper variable template to determine if @c std::iterator_traits<Itr>::difference_type exists.
/// @tparam Itr The iterator type to check.
template <typename Itr>
constexpr bool has_iterator_traits_difference_type_v = substitution_succeeds<use_iterator_traits_difference_type, Itr>;

/// @brief Helper alias for checking if @c incrementable_traits<Itr>::difference_type exists.
/// @tparam Itr The iterator type to check.
template <typename Itr>
using use_incrementable_traits_difference_type = typename incrementable_traits<Itr>::difference_type;

/// @brief Helper variable to determine if @c incrementable_traits<Itr>::difference_type exists.
/// @tparam Itr The iterator type to check.
template <typename Itr>
constexpr bool has_incrementable_traits_difference_type_v =
    substitution_succeeds<use_incrementable_traits_difference_type, Itr>;

/// @brief Implementation detail for computing the difference type of an iterator.
/// @tparam Itr The iterator type for which to compute the difference type.
template <typename Itr, typename = constraints<>>
class iter_difference_t_impl {};

/// @brief Implementation detail for computing the difference type of an iterator.
/// @tparam Itr The iterator type for which to compute the difference type.
/// Specialization for when @c std::iterator_traits<Itr>::difference_type exists.
template <typename Itr>
class iter_difference_t_impl<Itr, constraints<std::enable_if_t<has_iterator_traits_difference_type_v<Itr>>>> {
 public:
  /// @brief The difference type of the iterator type.
  using type = typename std::iterator_traits<Itr>::difference_type;
};

/// @brief Implementation detail for computing the difference type of an iterator.
/// @tparam Itr The iterator type for which to compute the difference type.
/// Specialization for when @c std::iterator_traits<Itr>::difference_type does not exist but
/// @c incrementable_traits<Itr>::difference_type does.
template <typename Itr>
class iter_difference_t_impl<
    Itr,
    constraints<
        std::enable_if_t<!has_iterator_traits_difference_type_v<Itr>>,
        std::enable_if_t<has_incrementable_traits_difference_type_v<Itr>>>> {
 public:
  /// @brief The difference type of the iterator type.
  using type = typename incrementable_traits<Itr>::difference_type;
};

}  // namespace iter_difference_t_detail

/// @brief Computes the difference type of the given iterator type.
/// @tparam Itr The iterator type for which to compute the difference type.
/// The difference type is defined as follows:
/// - If @c std::iterator_traits<remove_cvref_t<Itr>>::difference_type exists, it is used.
/// - Otherwise, @c incrementable_traits<remove_cvref_t<Itr>>::difference_type is used.
template <typename Itr>
using iter_difference_t = typename iter_difference_t_detail::iter_difference_t_impl<remove_cvref_t<Itr>>::type;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_ITER_DIFFERENCE_T_HPP_

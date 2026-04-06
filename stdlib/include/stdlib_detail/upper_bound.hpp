// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_UPPER_BOUND_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_UPPER_BOUND_HPP_

#include "arene/base/algorithm/detail/functional.hpp"
#include "arene/base/algorithm/detail/traits.hpp"
#include "arene/base/constraints.hpp"
#include "arene/base/functional/bind_front.hpp"
#include "arene/base/functional/not_fn.hpp"
#include "arene/base/type_traits/is_binary_predicate.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "stdlib/include/stdlib_detail/declval.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/internal_partition_point.hpp"
#include "stdlib/include/stdlib_detail/move.hpp"

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <algorithm>
// IWYU pragma: friend "stdlib_detail/.*"

namespace std {

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"

/// @brief returns an iterator to the first element greater than the given value
/// @tparam ForwardIterator iterator type
/// @tparam T value type to compare against
/// @tparam Compare binary predicate type
/// @param first beginning of the partitioned range of elements
/// @param last end of the partitioned range of elements
/// @param value value to compare elements to
/// @param comp binary predicate which returns @c true if the first argument is ordered before the second
///
/// @pre @c ForwardIterator must satisfy the forward iterator requirements.
/// @pre <tt>[begin, end)</tt> must be a valid range.
/// @pre The expression <tt>comp(value, e)</tt> must be convertible to @c bool
///     for every argument @c e of type @c RT, where @c RT is the reference type
///     of @c ForwardIterator, regardless of value category, and must not modify
///     @c e or @c value.
/// @pre The elements @c e of <tt>[begin, end)</tt> must be partitioned with
///     respect to the expression <tt>comp(value, e)</tt>.
///
/// @return iterator to the first element of the range <tt>[first, last)</tt>
///     ordered after @c value, or @c last if no such element is found.
///
/// @note Complexity
///     Given N as <tt>std::distance(first, last)</tt>, at most <tt>log2(N) +
///     O(1)</tt> applications of the comparator @c comp.
///
template <
    class ForwardIterator,
    class T,
    class Compare,
    arene::base::constraints<
        std::enable_if_t<arene::base::is_forward_iterator_v<ForwardIterator>>,
        std::enable_if_t<arene::base::is_binary_predicate_v<
            Compare&,
            T const&,
            arene::base::algorithm_detail::iter_reference_t<ForwardIterator>>>> = nullptr>
constexpr auto upper_bound(ForwardIterator first, ForwardIterator last, T const& value, Compare comp) noexcept(
    noexcept(std::internal::partition_point(
        std::move(first),
        std::move(last),
        arene::base::bind_front(std::declval<decltype(arene::base::not_fn(comp))&>(), value)
    ))
) -> ForwardIterator {
  auto not_comp = arene::base::not_fn(comp);
  return std::internal::partition_point(std::move(first), std::move(last), arene::base::bind_front(not_comp, value));
}

/// @brief returns an iterator to the first element greater than the given value
/// @tparam ForwardIterator iterator type
/// @tparam T value type to compare against
/// @tparam Compare binary predicate type
/// @param first beginning of the partitioned range of elements
/// @param last end of the partitioned range of elements
/// @param value value to compare elements to
///
/// @pre @c ForwardIterator must satisfy the forward iterator requirements.
/// @pre <tt>[begin, end)</tt> must be a valid range.
/// @pre The expression <tt>value < e</tt> must be convertible to @c bool
///     for every argument @c e of type @c RT, where @c RT is the reference type
///     of @c ForwardIterator, regardless of value category, and must not modify
///     @c e or @c value.
/// @pre The elements @c e of <tt>[begin, end)</tt> must be partitioned with
///     respect to the expression <tt>value < e</tt>.
///
/// @return iterator to the first element of the range <tt>[first, last)</tt>
///     ordered after @c value, or @c last if no such element is found.
///
/// @note Complexity
///     Given N as <tt>std::distance(first, last)</tt>, at most <tt>log2(N) +
///     O(1)</tt> comparisons with @c value using @c operator<.
///
template <
    class ForwardIterator,
    class T,
    arene::base::constraints<
        std::enable_if_t<arene::base::is_forward_iterator_v<ForwardIterator>>,
        std::enable_if_t<arene::base::is_binary_predicate_v<
            decltype(arene::base::algorithm_detail::operator_less)&,
            T const&,
            arene::base::algorithm_detail::iter_reference_t<ForwardIterator>>>> = nullptr>
constexpr auto upper_bound(ForwardIterator first, ForwardIterator last, T const& value) noexcept(
    noexcept(std::upper_bound(std::move(first), std::move(last), value, arene::base::algorithm_detail::operator_less))
) -> ForwardIterator {
  return std::upper_bound(std::move(first), std::move(last), value, arene::base::algorithm_detail::operator_less);
}

// parasoft-end-suppress AUTOSAR-M3_3_2-a

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_UPPER_BOUND_HPP_

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_MAX_ELEMENT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_MAX_ELEMENT_HPP_

#include "arene/base/algorithm/detail/functional.hpp"
#include "arene/base/algorithm/detail/traits.hpp"
#include "arene/base/constraints.hpp"
#include "arene/base/type_traits/is_compare.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/internal_relative_find.hpp"
#include "stdlib/include/stdlib_detail/move.hpp"

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <algorithm>
// IWYU pragma: friend "stdlib_detail/.*"

namespace std {

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"

/// @brief returns the largest element in a range
/// @tparam ForwardIterator iterator type
/// @tparam Compare binary predicate type
/// @param first beginning of the range of elements
/// @param last end of the range of elements
/// @param comp binary predicate which returns @c true if the first argument is ordered before the second
///
/// @pre @c ForwardIterator must satisfy the forward iterator requirements.
/// @pre <tt>[begin, end)</tt> must be a valid range.
/// @pre The expression <tt>comp(e1, e2)</tt> must be convertible to @c bool
///     for every argument @c e1, @c e2 of type @c RT, where @c RT is the
///     reference type of @c ForwardIterator, regardless of value category, and
///     must not modify @c e1 or @c e2.
///
/// Finds the largest element, i.e. the element ordered after all other
/// elements, in the range <tt>[first, last)</tt>.
///
/// @return iterator to the largest element of the range <tt>[first, last)</tt>.
///     If several elements in the range are equivalent to the largest element,
///     returns the iterator to the first such element. Returns @c last if the
///     range is empty.
///
/// @note Complexity
///     Given N as <tt>std::distance(first, last)</tt>, exactly <tt>max(N - 1,
///     0)</tt> applications of the comparator @c comp.
///
template <
    class ForwardIterator,
    class Compare,
    arene::base::constraints<
        std::enable_if_t<arene::base::is_forward_iterator_v<ForwardIterator>>,
        std::enable_if_t<
            arene::base::is_compare_v<Compare&, arene::base::algorithm_detail::iter_reference_t<ForwardIterator>>>> =
        nullptr>
constexpr auto max_element(ForwardIterator first, ForwardIterator last, Compare comp) noexcept(
    noexcept(std::internal::relative_find(first, last, arene::base::algorithm_detail::flip(comp)))
) -> ForwardIterator {
  return std::internal::relative_find(first, last, arene::base::algorithm_detail::flip(comp));
}

/// @brief returns the largest element in a range
/// @tparam ForwardIterator iterator type
/// @tparam Compare binary predicate type
/// @param first beginning of the range of elements
/// @param last end of the range of elements
///
/// @pre @c ForwardIterator must satisfy the forward iterator requirements.
/// @pre <tt>[begin, end)</tt> must be a valid range.
/// @pre The expression <tt>e1 < e2</tt> must be convertible to @c bool
///     for every argument @c e1, @c e2 of type @c RT, where @c RT is the
///     reference type of @c ForwardIterator, regardless of value category, and
///     must not modify @c e1 or @c e2.
///
/// Finds the largest element, i.e. the element ordered after all other
/// elements, in the range <tt>[first, last)</tt>.
///
/// @return iterator to the largest element of the range <tt>[first, last)</tt>.
///     If several elements in the range are equivalent to the largest element,
///     returns the iterator to the first such element. Returns @c last if the
///     range is empty.
///
/// @note Complexity
///     Given N as <tt>std::distance(first, last)</tt>, exactly <tt>max(N - 1,
///     0)</tt> comparisons using @c operator<.
///
template <
    class ForwardIterator,
    arene::base::constraints<
        std::enable_if_t<arene::base::is_forward_iterator_v<ForwardIterator>>,
        std::enable_if_t<arene::base::is_compare_v<
            decltype(arene::base::algorithm_detail::operator_less)&,
            arene::base::algorithm_detail::iter_reference_t<ForwardIterator>>>> = nullptr>
constexpr auto max_element(ForwardIterator first, ForwardIterator last) noexcept(
    noexcept(std::max_element(std::move(first), std::move(last), arene::base::algorithm_detail::operator_less))
) -> ForwardIterator {
  return std::max_element(std::move(first), std::move(last), arene::base::algorithm_detail::operator_less);
}

// parasoft-end-suppress AUTOSAR-M3_3_2-a

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_MAX_ELEMENT_HPP_

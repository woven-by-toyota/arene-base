// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: defines std::is_sorted"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_SORTED_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_SORTED_HPP_

// IWYU pragma: private, include <algorithm>
// IWYU pragma: friend "stdlib_detail/.*"

#include "arene/base/algorithm/detail/functional.hpp"
#include "arene/base/algorithm/detail/traits.hpp"
#include "arene/base/constraints.hpp"
#include "arene/base/iterator/advance.hpp"
#include "arene/base/type_traits/is_compare.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/forward.hpp"
#include "stdlib/include/stdlib_detail/is_copy_assignable.hpp"
#include "stdlib/include/stdlib_detail/is_copy_constructible.hpp"
#include "stdlib/include/stdlib_detail/iterator_traits.hpp"
#include "stdlib/include/stdlib_detail/move.hpp"

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

namespace std {

// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
/// @brief Check if a range is sorted. Returns @c false if there are any pair of sequential elements @c i and @c j such
/// that @c comp(j,i) is @c true otherwise returns @c true
/// @tparam ForwardIterator The iterator type used to identify the range
/// @tparam Comparator The type of the comparator
/// @param first The start of the range
/// @param last The end of the range
/// @param comp The comparator
/// @return @c true if the range is sorted, @c false otherwise
/// @pre @c [first,last) must be a valid range
/// @pre @c comp(*first,*first) must be valid and return a boolean-testable result
/// @pre @c comp must provide a strict weak order over the elements of the range
template <
    typename ForwardIterator,
    typename Comparator,
    arene::base::constraints<
      enable_if_t<arene::base::is_forward_iterator_v<ForwardIterator>>,
      enable_if_t<arene::base::is_compare_v<
            Comparator&,
            typename std::iterator_traits<ForwardIterator>::reference>>
            > = nullptr>
auto is_sorted(ForwardIterator first, ForwardIterator last, Comparator comp) noexcept(
             noexcept(first != last) && //
             noexcept(arene::base::advance(first, 1)) && //
             noexcept(comp(*first, *first)) && //
             is_nothrow_copy_constructible_v<ForwardIterator> && //
             is_nothrow_copy_assignable_v<ForwardIterator>
) -> bool {
  if (first != last) {
    ForwardIterator prev{first};
    arene::base::advance(first, 1);
    while (first != last) {
      if (comp(*first, *prev)) {
        return false;
      }
      prev = first;
      arene::base::advance(first, 1);
    }
  }
  return true;
}
// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2

/// @brief Check if a range is sorted. Returns @c false if there are any pair of sequential elements @c i and @c j such
/// that @c j<i is @c true otherwise returns @c true
/// @tparam ForwardIterator The iterator type used to identify the range
/// @param first The start of the range
/// @param last The end of the range
/// @return @c true if the range is sorted, @c false otherwise
/// @pre @c [first,last) must be a valid range
/// @pre @c *first<*first must be valid and return a boolean-testable result
/// @pre Less-than comparison of elements must provide a strict weak order over the elements of the range
template <
    typename ForwardIterator,
    arene::base::constraints<
        enable_if_t<arene::base::is_forward_iterator_v<ForwardIterator>>,
        enable_if_t<arene::base::is_compare_v<
            decltype(arene::base::algorithm_detail::operator_less)&,
            typename std::iterator_traits<ForwardIterator>::reference>>> = nullptr>
auto is_sorted(ForwardIterator first, ForwardIterator last) noexcept(
    noexcept(is_sorted(std::move(first), std::move(last), arene::base::algorithm_detail::operator_less))
) -> bool {
  return is_sorted(std::move(first), std::move(last), arene::base::algorithm_detail::operator_less);
}

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_SORTED_HPP_

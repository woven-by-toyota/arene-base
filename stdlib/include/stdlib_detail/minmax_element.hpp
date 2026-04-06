// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_MINMAX_ELEMENT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_MINMAX_ELEMENT_HPP_

#include "arene/base/algorithm/detail/functional.hpp"
#include "arene/base/algorithm/detail/traits.hpp"
#include "arene/base/constraints.hpp"
#include "arene/base/type_traits/is_compare.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "arene/base/utility/swap.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/internal_disable_constexpr_in_cpp14.hpp"
#include "stdlib/include/stdlib_detail/is_constructible.hpp"
#include "stdlib/include/stdlib_detail/is_copy_assignable.hpp"
#include "stdlib/include/stdlib_detail/move.hpp"
#include "stdlib/include/stdlib_detail/pair.hpp"

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"

// IWYU pragma: private, include <algorithm>
// IWYU pragma: friend "stdlib_detail/.*"

namespace std {

/// @brief returns the smallest and largest elements in a range
/// @tparam ForwardIterator iterator type
/// @tparam Compare binary predicate type
/// @param first beginning of the range of elements
/// @param last end of the range of elements
/// @param comp binary predicate which returns @c true if the first argument is
///     ordered before the second
///
/// @pre @c ForwardIterator must satisfy the forward iterator requirements.
/// @pre <tt>[begin, end)</tt> must be a valid range.
/// @pre The expression <tt>comp(e1, e2)</tt> must be convertible to @c bool
///     for every argument @c e1, @c e2 of type @c RT, where @c RT is the
///     reference type of @c ForwardIterator, regardless of value category, and
///     must not modify @c e1 or @c e2.
///
/// Finds both the first smallest element, i.e. the element ordered before all
/// other elements, and the last largest element, i.e. the element ordered after
/// all other elements, in the range <tt>[first, last)</tt>.
///
/// @return pair of iterators to the smallest element of the range <tt>[first,
///     last)</tt>. If several elements in the range are equivalent to the
///     smallest element, returns the iterator to the first such element. If
///     several elements in the range are equivalent to the largest element,
///     returns the iterator to the last such element. Returns
///     <tt>make_pair(first, first)</tt> if the range is empty.
///
/// @note Complexity
///     Given N as <tt>std::distance(first, last)</tt>, at most
///     <tt>max(floor(3*(N-1)/2), 0)</tt> applications of the comparator
///     @c comp.
///
template <
    class ForwardIterator,
    class Compare,
    arene::base::constraints<
        std::enable_if_t<arene::base::is_forward_iterator_v<ForwardIterator>>,
        std::enable_if_t<arene::base::is_compare_v<
            Compare&,
            arene::base::algorithm_detail::iter_reference_t<ForwardIterator>>>> = nullptr>
constexpr auto minmax_element(ForwardIterator first, ForwardIterator last, Compare comp) noexcept(
    noexcept(!comp(*first, *first)) &&  //
    noexcept(first == first) && //
    noexcept(first != first) && //
    noexcept(++first) && //
    noexcept(first++) && //
    std::is_nothrow_copy_assignable_v<ForwardIterator> &&  //
    std::is_nothrow_constructible_v<
        std::pair<ForwardIterator, ForwardIterator>,
        ForwardIterator&,
        ForwardIterator&>
) -> std::pair<ForwardIterator, ForwardIterator> {
  std::detail::disable_constexpr_in_cpp14();
  auto result = std::pair<ForwardIterator, ForwardIterator>{first, first};

  if (first == last || ++first == last) {
    return result;
  }

  auto& min = result.first;
  auto& max = result.second;

  if (comp(*first, *min)) {
    min = first;
  } else {
    max = first;
  }
  ++first;

  auto const update_with_last = [&min, &max, &comp](auto const& iter) {
    if (comp(*iter, *min)) {
      min = iter;
    } else if (!comp(*iter, *max)) {
      max = iter;
    } else {
      // empty clause required by M6-4-2
    }
  };

  auto const update_with_next_two = [&min, &max, &comp](auto lower, auto upper) {
    if (!comp(*lower, *upper)) {
      arene::base::swap(lower, upper);
    }
    if (comp(*lower, *min)) {
      min = lower;
    }
    if (!comp(*upper, *max)) {
      max = upper;
    }
  };

  while (first != last) {
    auto const iter = first++;

    if (first == last) {
      update_with_last(iter);
    } else {
      update_with_next_two(first, iter);
      ++first;
    }
  }

  return result;
}

/// @brief returns the smallest and largest elements in a range
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
/// Finds both the first smallest element, i.e. the element ordered before all
/// other elements, and the last largest element, i.e. the element ordered after
/// all other elements, in the range <tt>[first, last)</tt>.
///
/// @return pair of iterators to the smallest element of the range <tt>[first,
///     last)</tt>. If several elements in the range are equivalent to the
///     smallest element, returns the iterator to the first such element. If
///     several elements in the range are equivalent to the largest element,
///     returns the iterator to the last such element. Returns
///     <tt>make_pair(first, first)</tt> if the range is empty.
///
/// @note Complexity
///     Given N as <tt>std::distance(first, last)</tt>, at most
///     <tt>max(floor(3*(N-1)/2), 0)</tt> applications of the comparator
///     @c operator<.
///
template <
    class ForwardIterator,
    arene::base::constraints<
        std::enable_if_t<arene::base::is_forward_iterator_v<ForwardIterator>>,
        std::enable_if_t<arene::base::is_compare_v<
            decltype(arene::base::algorithm_detail::operator_less)&,
            arene::base::algorithm_detail::iter_reference_t<ForwardIterator>>>> = nullptr>
constexpr auto minmax_element(ForwardIterator first, ForwardIterator last) noexcept(
    noexcept(std::minmax_element(std::move(first), std::move(last), arene::base::algorithm_detail::operator_less))
) -> std::pair<ForwardIterator, ForwardIterator> {
  std::detail::disable_constexpr_in_cpp14();
  return std::minmax_element(std::move(first), std::move(last), arene::base::algorithm_detail::operator_less);
}

// parasoft-end-suppress AUTOSAR-M3_3_2-a

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_MINMAX_ELEMENT_HPP_

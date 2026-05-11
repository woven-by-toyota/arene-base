// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_PARTIAL_SORT_COPY_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_PARTIAL_SORT_COPY_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <algorithm>
// IWYU pragma: friend "stdlib_detail/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/algorithm/detail/functional.hpp"
#include "arene/base/algorithm/detail/traits.hpp"
#include "arene/base/algorithm/rotate.hpp"
#include "arene/base/constraints.hpp"
#include "arene/base/type_traits.hpp"
#include "arene/base/type_traits/is_compare.hpp"
#include "arene/base/utility.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/ignore.hpp"
#include "stdlib/include/stdlib_detail/is_assignable.hpp"
#include "stdlib/include/stdlib_detail/is_move_assignable.hpp"
#include "stdlib/include/stdlib_detail/is_move_constructible.hpp"
#include "stdlib/include/stdlib_detail/lower_bound.hpp"
#include "stdlib/include/stdlib_detail/move.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace std {
// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
/// @brief Copy the first _K_ elements from the source range, as sorted with respect to the supplied comparator, into
/// the target range, where _K_ is the minimum of @c std::distance(source_begin,source_end) and @c
/// std::distance(target_begin,target_end).
///
/// This performs at most <c>N * (log2(K) + 1)</c> comparisons, where _N_ is @c std::distance(source_begin,source_end)
///
/// @tparam SourceIterator The type of the iterator for the input range
/// @tparam TargetIterator The type of the iterator for the output range
/// @tparam Comparator The comparator type
/// @param source_begin The start of the input range
/// @param source_end The end of the input range
/// @param target_begin The start of the output range
/// @param target_end The end of the output range
/// @param comp The comparator
/// @return A @c TargetIterator referring to one-past the last stored element in the output
/// @pre The @c SourceIterator must be an input iterator, and @c [source_begin,source_end) must be a valid range
/// @pre The @c TargetIterator must be a random access iterator, and @c [target_begin,target_end) must be a valid range
/// @pre The result of dereferencing a source iterator must be assignable to the result of dereferencing a target
/// iterator
/// @pre The value type of the iterators must be less-than-comparable
/// @pre The comparator must be a binary predicate such that @c comp(*source_begin,*target_begin) and @c
/// comp(*target_begin,*target_begin) are both well-formed and convertible to @c bool
template <
    typename SourceIterator,
    typename TargetIterator,
    typename Comparator,
    arene::base::constraints<
        enable_if_t<arene::base::denotes_range_v<SourceIterator>>,
        enable_if_t<arene::base::denotes_range_v<TargetIterator>>,
        enable_if_t<arene::base::is_random_access_iterator_v<TargetIterator>>,
        enable_if_t<is_assignable_v<
            arene::base::algorithm_detail::iter_reference_t<TargetIterator>,
            arene::base::algorithm_detail::iter_reference_t<SourceIterator>>>,
        enable_if_t<
            arene::base::is_less_than_comparable_v<arene::base::algorithm_detail::iter_reference_t<TargetIterator>>>,
        enable_if_t<is_move_assignable_v<arene::base::algorithm_detail::iter_value_t<TargetIterator>>>,
        enable_if_t<is_move_constructible_v<arene::base::algorithm_detail::iter_value_t<TargetIterator>>>,
        enable_if_t<arene::base::is_compare_v<
            Comparator&,
            arene::base::algorithm_detail::iter_reference_t<SourceIterator>,
            arene::base::algorithm_detail::iter_reference_t<TargetIterator>>>,
        enable_if_t<
            arene::base::is_compare_v<Comparator&, arene::base::algorithm_detail::iter_reference_t<TargetIterator>>>> =
        nullptr>
auto partial_sort_copy(
    SourceIterator source_begin,
    SourceIterator source_end,
    TargetIterator target_begin,
    TargetIterator target_end,
    Comparator comp
) -> TargetIterator {
  if ((target_begin == target_end) || (source_begin == source_end)) {
    return target_begin;
  }
  auto current_last = target_begin;
  auto const target_last = target_end - 1;
  *current_last = *source_begin;
  ++source_begin;
  for (auto&& elem : arene::base::make_subrange(std::move(source_begin), std::move(source_end))) {
    auto const next_last = current_last + 1;
    auto target_pos = std::lower_bound(target_begin, next_last, elem, comp);
    bool const is_room{current_last != target_last};
    bool const new_is_smaller{target_pos != next_last};
    if ((!new_is_smaller) && (!is_room)) {
      continue;
    }
    if (is_room) {
      current_last = next_last;
    }
    *current_last = std::forward<decltype(elem)>(elem);
    if (new_is_smaller) {
      ignore = arene::base::rotate(target_pos, current_last, current_last + 1);
    }
  }
  return current_last + 1;
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a

/// @brief Copy the first _K_ elements from the source range, as sorted with respect to @c operator< , into the target
/// range, where _K_ is the minimum of @c std::distance(source_begin,source_end) and @c
/// std::distance(target_begin,target_end)
///
/// This performs at most <c>N * (log2(K) + 1)</c> comparisons, where _N_ is @c std::distance(source_begin,source_end)
///
/// @tparam SourceIterator The type of the iterator for the input range
/// @tparam TargetIterator The type of the iterator for the output range
/// @param source_begin The start of the input range
/// @param source_end The end of the input range
/// @param target_begin The start of the output range
/// @param target_end The end of the output range
/// @return A @c TargetIterator referring to one-past the last stored element in the output
/// @pre The @c SourceIterator must be an input iterator, and @c [source_begin,source_end) must be a valid range
/// @pre The @c TargetIterator must be a random access iterator, and @c [target_begin,target_end) must be a valid range
/// @pre The result of dereferencing a source iterator must be assignable to the result of dereferencing a target
/// iterator
/// @pre The value type of the iterators must be less-than-comparable
template <
    typename SourceIterator,
    typename TargetIterator,
    arene::base::constraints<
        enable_if_t<arene::base::denotes_range_v<SourceIterator>>,
        enable_if_t<arene::base::denotes_range_v<TargetIterator>>,
        enable_if_t<arene::base::is_random_access_iterator_v<TargetIterator>>,
        enable_if_t<is_assignable_v<
            arene::base::algorithm_detail::iter_reference_t<TargetIterator>,
            arene::base::algorithm_detail::iter_reference_t<SourceIterator>>>,
        enable_if_t<
            arene::base::is_less_than_comparable_v<arene::base::algorithm_detail::iter_reference_t<TargetIterator>>>,
        enable_if_t<is_move_assignable_v<arene::base::algorithm_detail::iter_value_t<TargetIterator>>>,
        enable_if_t<is_move_constructible_v<arene::base::algorithm_detail::iter_value_t<TargetIterator>>>> = nullptr>
auto partial_sort_copy(
    SourceIterator source_begin,
    SourceIterator source_end,
    TargetIterator target_begin,
    TargetIterator target_end
) -> TargetIterator {
  return partial_sort_copy(
      std::move(source_begin),
      std::move(source_end),
      std::move(target_begin),
      std::move(target_end),
      arene::base::algorithm_detail::operator_less
  );
}

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_PARTIAL_SORT_COPY_HPP_

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_ADJACENT_FIND_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_ADJACENT_FIND_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <algorithm>
// IWYU pragma: friend "stdlib_detail/.*"

#include "arene/base/algorithm/detail/functional.hpp"
#include "arene/base/algorithm/detail/traits.hpp"
#include "arene/base/constraints.hpp"
#include "arene/base/type_traits/is_predicate.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"

namespace std {

/// @brief finds the first two adjacent items that satisfy a given predicate
/// @tparam ForwardIterator iterator type
/// @tparam BinaryPred binary predicate type
/// @param first the beginning of the range
/// @param last the end of the range
/// @param pred binary predicate which returns @c true if the elements should be treated as equal
///
/// @pre @c ForwardIteratorerator must satisfy the forward iterator requirements.
/// @pre <tt>[begin, end)</tt> must be a valid range.
/// @pre The expression <tt>pred(e1, e2)</tt> must be convertible to @c bool
///     for every argument @c e1, @c e2 of type @c RT, where @c RT is the reference type
///     of @c ForwardIterator, regardless of value category, and must not modify
///     @c e1 or @c e2.
///
/// @return iterator to the first element of the range <tt>[first, last)</tt>
///         such that <tt>pred(*it, *(it + 1)) != false</tt> or @c last if no such element is found.
///
/// @note Complexity
///     For a nonempty range, exactly <tt>min((i - first) + 1, (last - first) - 1)</tt> applications of the
///     corresponding predicate, where i is adjacent_find's return value.
template <
    class ForwardIterator,
    class BinaryPred,
    arene::base::constraints<
        std::enable_if_t<arene::base::is_forward_iterator_v<ForwardIterator>>,
        std::enable_if_t<arene::base::is_predicate_v<
            BinaryPred&,
            arene::base::algorithm_detail::iter_reference_t<ForwardIterator>,
            arene::base::algorithm_detail::iter_reference_t<ForwardIterator>>>> = nullptr>
auto adjacent_find(ForwardIterator first, ForwardIterator last, BinaryPred pred) noexcept(  //
    noexcept(++first) &&                                                                    //
    noexcept(first++) &&                                                                    //
    noexcept(first == last) &&                                                              //
    noexcept(first != last) &&                                                              //
    noexcept(pred(*first, *first))
) -> ForwardIterator {
  if (first == last) {
    return last;
  }

  auto prev = first++;
  while (first != last) {
    if (pred(*prev, *first)) {
      return prev;
    }

    ++first;
    ++prev;
  }

  return last;
}

/// @brief finds the first two adjacent items that are equal
/// @tparam ForwardIterator iterator type
/// @param first the beginning of the range
/// @param last the end of the range
///
/// @pre @c ForwardIteratorerator must satisfy the forward iterator requirements.
/// @pre <tt>[begin, end)</tt> must be a valid range.
/// @pre The expression <tt>e1 == e2</tt> must be convertible to @c bool
///     for every argument @c e1, @c e2 of type @c RT, where @c RT is the reference type
///     of @c ForwardIterator, regardless of value category, and must not modify
///     @c e1 or @c e2.
///
/// @return iterator to the first element of the range <tt>[first, last)</tt>
///         such that <tt>*it == *(it + 1)</tt> or @c last if no such element is found.
///
/// @note Complexity
///     For a nonempty range, exactly <tt>min((i - first) + 1, (last - first) - 1)</tt> equality comparisons, where i is
///     adjacent_find's return value.
template <
    class ForwardIterator,
    arene::base::constraints<
        std::enable_if_t<arene::base::is_forward_iterator_v<ForwardIterator>>,
        std::enable_if_t<arene::base::is_predicate_v<
            arene::base::algorithm_detail::operator_equal_t&,
            arene::base::algorithm_detail::iter_reference_t<ForwardIterator>,
            arene::base::algorithm_detail::iter_reference_t<ForwardIterator>>>> = nullptr>
auto adjacent_find(ForwardIterator first, ForwardIterator last) noexcept(
    noexcept(std::adjacent_find(first, last, arene::base::algorithm_detail::operator_equal))
) -> ForwardIterator {
  return std::adjacent_find(first, last, arene::base::algorithm_detail::operator_equal);
}

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_ADJACENT_FIND_HPP_

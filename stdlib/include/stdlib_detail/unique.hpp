// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_UNIQUE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_UNIQUE_HPP_

#include "arene/base/algorithm/detail/functional.hpp"
#include "arene/base/algorithm/detail/traits.hpp"
#include "arene/base/constraints.hpp"
#include "arene/base/iterator/next.hpp"
#include "arene/base/type_traits/is_binary_predicate.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "stdlib/include/stdlib_detail/declval.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/internal_named_requirements.hpp"
#include "stdlib/include/stdlib_detail/is_assignable.hpp"
#include "stdlib/include/stdlib_detail/is_copy_constructible.hpp"
#include "stdlib/include/stdlib_detail/move.hpp"
#include "stdlib/include/stdlib_detail/remove_reference.hpp"

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <algorithm>
// IWYU pragma: friend "stdlib_detail/.*"

namespace std {

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"

/// @brief removes consecutive elements in a range
/// @tparam I forward iterator type
/// @tparam P binary predicate type
/// @param first beginning of the range of elements
/// @param last end of the range of elements
/// @param pred binary predicate which returns @c true if the elements are
///   equivalent
///
/// @pre @c ForwardIterator must satisfy the forward iterator requirements.
/// @pre <tt>[begin, end)</tt> must be a valid range.
/// @pre The expression <tt>pred(r1, r2)</tt> must be convertible to @c bool
///   for every argument @c r1 and @c r2 of type @c RT, where @c RT is the
///   reference type of @c I, regardless of value category, and must not modify
///   @c r1 or @c r2.
/// @pre pred must define an *equivalence relation*. For values
///   @c a, @c b, and @c c, the following properties must be satisfied:
///   * reflexivity: <tt> pred(a, a) </tt> is @c true
///   * symmetry: <tt> pred(a, b) == pred(b, a) </tt> is @c true
///   * transitivity: if @c pred(a, b) and @c pred(b, c) are both @c true, then
///     @c pred(a, c) is @c true
///
/// Removes all except the first element from a consecutive group of equivalent
/// elements in the range <tt> [first, last) </tt>, and and returns a
/// past-the-end iterator for the new end of the range.
///
/// Removal is performed by shifting the elements in the range in such a way
/// that the elements that are not to be removed appear in the beginning of the
/// range. The relative ordering of non removed elements is maintained by
/// @c std::unique, (i.e. relative ordering is stable).
///
/// @return past-the-end iterator for the new range of values. If this is not
///   @c end(), then it points to an unspecified value, and so do iterators to
///   any values between this iterator and @c end().
///
/// @note Complexity
///   Given N as <tt> std::distance(first, last) </tt>, exactly
///   <tt> max(0, N - 1) </tt> applications of the predicate @c pred.
///
template <
    class I,
    class P,
    arene::base::constraints<
        std::enable_if_t<arene::base::is_forward_iterator_v<I>>,
        std::enable_if_t<
            internal::named_requirement::is_move_assignable_v<arene::base::algorithm_detail::iter_value_t<I>>>,
        std::enable_if_t<arene::base::is_binary_predicate_v<P&, arene::base::algorithm_detail::iter_reference_t<I>>>> =
        nullptr>
auto unique(I first, I last, P pred) noexcept(                                          //
    std::is_nothrow_copy_constructible_v<I>&&                                           //
    noexcept(std::declval<I&>() == std::declval<I&>()) &&                               //
    noexcept(std::declval<I&>() != std::declval<I&>()) &&                               //
    noexcept(++std::declval<I&>()) &&                                                   //
    noexcept(!std::declval<P&>()(*std::declval<I&>(), *std::declval<I&>())) &&          //
    std::is_nothrow_assignable_v<                                                       //
        arene::base::algorithm_detail::iter_reference_t<I>,                             //
        std::remove_reference_t<arene::base::algorithm_detail::iter_reference_t<I>>&&>  //
) -> I {
  if (first == last) {
    return last;
  }

  for (auto iter = arene::base::next(first); iter != last; ++iter) {
    if (!pred(*first, *iter)) {
      ++first;
      if (first != iter) {
        *first = std::move(*iter);
      }
    }
  }

  return ++first;
}

/// @brief removes consecutive elements in a range
/// @tparam I forward iterator type
/// @param first beginning of the range of elements
/// @param last end of the range of elements
///
/// @pre @c ForwardIterator must satisfy the forward iterator requirements.
/// @pre <tt>[begin, end)</tt> must be a valid range.
/// @pre The expression <tt>pred(r1, r2)</tt> must be convertible to @c bool
///   for every argument @c r1 and @c r2 of type @c RT, where @c RT is the
///   reference type of @c I, regardless of value category, and must not modify
///   @c r1 or @c r2.
/// @pre @c operator== must define an *equivalence relation*. For values
///   @c a, @c b, and @c c, the following properties must be satisfied:
///   * reflexivity: <tt> pred(a, a) </tt> is @c true
///   * symmetry: <tt> pred(a, b) == pred(b, a) </tt> is @c true
///   * transitivity: if @c pred(a, b) and @c pred(b, c) are both @c true, then
///     @c pred(a, c) is @c true
///
/// Removes all except the first element from a consecutive group of equal
/// elements in the range <tt> [first, last) </tt>, and and returns a
/// past-the-end iterator for the new end of the range.
///
/// Removal is performed by shifting the elements in the range in such a way
/// that the elements that are not to be removed appear in the beginning of the
/// range. The relative ordering of non removed elements is maintained by
/// @c std::unique, (i.e. relative ordering is stable).
///
/// @return past-the-end iterator for the new range of values. If this is not
///   @c end(), then it points to an unspecified value, and so do iterators to
///   any values between this iterator and @c end().
///
/// @note Complexity
///   Given N as <tt> std::distance(first, last) </tt>, exactly
///   <tt> max(0, N - 1) </tt> applications of @c operator==.
///
template <
    class I,
    arene::base::constraints<
        std::enable_if_t<arene::base::is_forward_iterator_v<I>>,
        std::enable_if_t<
            internal::named_requirement::is_move_assignable_v<arene::base::algorithm_detail::iter_value_t<I>>>,
        std::enable_if_t<arene::base::is_predicate_v<
            arene::base::algorithm_detail::operator_equal_t&,
            arene::base::algorithm_detail::iter_reference_t<I>,
            arene::base::algorithm_detail::iter_reference_t<I>>>> = nullptr>
auto unique(I first, I last) noexcept(  //
    noexcept(std::unique(std::move(first), std::move(last), arene::base::algorithm_detail::operator_equal))
) -> I {
  return std::unique(std::move(first), std::move(last), arene::base::algorithm_detail::operator_equal);
}

// parasoft-end-suppress AUTOSAR-M3_3_2-a

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_UNIQUE_HPP_

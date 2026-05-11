// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_SORT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_SORT_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <algorithm>
// IWYU pragma: friend "stdlib_detail/.*"

#include "arene/base/algorithm/detail/functional.hpp"
#include "arene/base/algorithm/detail/traits.hpp"
#include "arene/base/algorithm/sort.hpp"
#include "arene/base/type_traits/is_compare.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/is_movable.hpp"
#include "arene/base/type_traits/is_swappable.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/move.hpp"

namespace std {

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
/// @brief sorts a range into ascending order
/// @tparam I iterator type
/// @tparam C compare type
/// @param first beginning of the range
/// @param last end of the range
/// @param comp comparison function object
///
/// @pre @c I must satisfy the random access iterator requirements.
/// @pre @c I must be ValueSwappable.
/// @pre the type of <c>*first</c> must be Movable.
/// @pre <c>[first, last)</c> must be a valid range.
/// @pre The expression <c>comp(r1, r2)</c> must be convertible to @c bool
///   for every argument @c r1 and @c r2 of type @c RT, where @c RT is the
///   reference type of @c I, regardless of value category, and must not modify
///   @c r1 or @c r2.
/// @pre comp must define a *strict weak ordering relation*. For values
///   @c a, @c b, and @c c, the following properties must be satisfied:
///   * irreflexivity: <c> comp(a, a) </c> is @c false
///   * transitivity: if <c> comp(a, b) </c> and <c> comp(b, c) </c> are both
///     @c true, then <c> pred(a, c) </c> is @c true
///   * transitivity of incomparability: let <c> e(a, b) </c> be
///     <c> !comp(a, b) && !comp(b, a) </c>, then @c e is transitive
///
/// Sorts the elements in the range <c> [first, last) </c> in non-descending
/// order, specified by @c comp. The order of equal elements is not guaranteed
/// to be preserved.
///
/// @post <c> is_sorted(first, last, comp) </c> is @c true
///
/// @note Complexity
///   Given N as <c> last - first </c>, <c> O(N * log(N)) </c> applications of
///   @c comp.
///
template <
    class I,
    class C,
    class = enable_if_t<
        arene::base::is_random_access_iterator_v<I> &&
        arene::base::is_swappable_v<arene::base::algorithm_detail::iter_reference_t<I>> &&
        arene::base::is_movable_v<arene::base::remove_cvref_t<arene::base::algorithm_detail::iter_reference_t<I>>> &&
        arene::base::is_compare_v<C&, arene::base::algorithm_detail::iter_reference_t<I>>
        >
    >
auto sort(I first, I last, C comp) noexcept(  //
    arene::base::is_nothrow_invocable_v<decltype(arene::base::sort), I&&, I&&, C&&>
) -> void {
  return arene::base::sort(std::move(first), std::move(last), std::move(comp));
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a

/// @brief sorts a range into ascending order
/// @tparam I iterator type
/// @param first beginning of the range
/// @param last end of the range
///
/// @pre @c I must satisfy the random access iterator requirements.
/// @pre @c I must be ValueSwappable.
/// @pre the type of <c>*first</c> must be Movable.
/// @pre <c>[first, last)</c> must be a valid range.
/// @pre The expression <c>r1 < r2</c> must be convertible to @c bool
///   for every argument @c r1 and @c r2 of type @c RT, where @c RT is the
///   reference type of @c I, regardless of value category, and must not modify
///   @c r1 or @c r2.
/// @pre @c operator< must define a *strict weak ordering relation*. For values
///   @c a, @c b, and @c c, the following properties must be satisfied:
///   * irreflexivity: <c> a < a </c> is @c false
///   * transitivity: if <c> a < b </c> and <c> b < c </c> are both
///     @c true, then <c> a < c </c> is @c true
///   * transitivity of incomparability: let <c> e(a, b) </c> be
///     <c> !(a < b) && !(b < a) </c>, then @c e is transitive
///
/// Sorts the elements in the range <c> [first, last) </c> in non-descending
/// order, specified by @c operator<. The order of equal elements is not
/// guaranteed to be preserved.
///
/// @note Complexity
///   Given N as <c> last - first </c>, <c> O(N * log(N)) </c> applications of
///   @c operator<.
///
template <
    class I,
    class = enable_if_t<
        arene::base::is_random_access_iterator_v<I> &&
        arene::base::is_swappable_v<arene::base::algorithm_detail::iter_reference_t<I>> &&
        arene::base::is_movable_v<arene::base::remove_cvref_t<arene::base::algorithm_detail::iter_reference_t<I>>> &&
        arene::base::is_compare_v<
            decltype(arene::base::algorithm_detail::operator_less)&,
            arene::base::algorithm_detail::iter_reference_t<I>>
        >
    >
auto sort(I first, I last) noexcept(  //
    arene::base::is_nothrow_invocable_v<decltype(arene::base::sort), I&&, I&&, decltype(arene::base::algorithm_detail::operator_less)>
) -> void {
  return arene::base::sort(std::move(first), std::move(last), arene::base::algorithm_detail::operator_less);
}

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_SORT_HPP_

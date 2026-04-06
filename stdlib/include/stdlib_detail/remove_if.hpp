// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_REMOVE_IF_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_REMOVE_IF_HPP_

#include "arene/base/algorithm/detail/functional.hpp"
#include "arene/base/algorithm/detail/traits.hpp"
#include "arene/base/algorithm/find_if.hpp"
#include "arene/base/constraints.hpp"
#include "arene/base/iterator/advance.hpp"
#include "arene/base/iterator/next.hpp"
#include "arene/base/type_traits/is_predicate.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "stdlib/include/stdlib_detail/declval.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/internal_named_requirements.hpp"
#include "stdlib/include/stdlib_detail/is_assignable.hpp"
#include "stdlib/include/stdlib_detail/is_move_assignable.hpp"
#include "stdlib/include/stdlib_detail/move.hpp"
#include "stdlib/include/stdlib_detail/remove_reference.hpp"

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <algorithm>
// IWYU pragma: friend "stdlib_detail/.*"

namespace std {

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"

/// @brief removes elements satisfying specific criteria
/// @tparam I forward iterator type
/// @tparam P removal predicate type
/// @param first beginning of the range of elements
/// @param last end of the range of elements
/// @param pred unary predicate which returns @c true if the element should be
///   removed
///
/// @pre @c ForwardIterator must satisfy the forward iterator requirements.
/// @pre <tt>[begin, end)</tt> must be a valid range.
/// @pre The expression <tt>pred(v)</tt> must be convertible to @c bool
///   for every argument @c r of type @c RT, where @c RT is the reference type
///   of @c I, regardless of value category, and must not modify @c r.
///
/// Removes all elements satisfying specific criteria from the range <tt>[first,
/// last)</tt> and returns a past-the-end iterator for the new end of the range.
///
/// Removal is performed by shifting the elements in the range in such a way
/// that the elements that are not to be removed appear in the beginning of the
/// range. The relative ordering of non removed elements is maintained by
/// @c std::remove_if.
///
/// @return past-the-end iterator for the new range of values. If this is not
///   @c end(), then it points to an unspecified value, and so do iterators to
///   any values between this iterator and @c end().
///
/// @note Complexity
///     Given N as <tt>std::distance(first, last)</tt>, exactly @c N
///     applications of the predicate @c pred.
///
template <
    class I,
    class P,
    arene::base::constraints<
        std::enable_if_t<arene::base::is_forward_iterator_v<I>>,
        std::enable_if_t<
            std::internal::named_requirement::is_move_assignable_v<arene::base::algorithm_detail::iter_value_t<I>>>,
        std::enable_if_t<arene::base::is_predicate_v<P&, arene::base::algorithm_detail::iter_reference_t<I>>>> =
        nullptr>
auto remove_if(I first, I last, P pred) noexcept(                                                  //
    noexcept(arene::base::find_if(std::declval<I&>(), std::declval<I&>(), std::declval<P&>())) &&  //
    noexcept(!std::declval<P&>()(*std::declval<I&>())) &&                                          //
    std::is_nothrow_move_assignable_v<I> &&                                                        //
    std::is_nothrow_assignable_v<
        arene::base::algorithm_detail::iter_reference_t<I>,
        std::remove_reference_t<arene::base::algorithm_detail::iter_reference_t<I>>&&>  //
) -> I {
  first = arene::base::find_if(first, last, pred);
  if (first != last) {
    for (auto iter = arene::base::next(first); iter != last; ++iter) {
      if (!pred(*iter)) {
        *first = std::move(*iter);
        arene::base::advance(first, arene::base::algorithm_detail::iter_difference_t<I>{1});
      }
    }
  }
  return first;
}

// parasoft-end-suppress AUTOSAR-M3_3_2-a

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_REMOVE_IF_HPP_

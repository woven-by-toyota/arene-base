// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_FIND_IF_NOT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_FIND_IF_NOT_HPP_

// IWYU pragma: private, include <algorithm>
// IWYU pragma: friend "stdlib_detail/.*"

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

#include "arene/base/algorithm/detail/functional.hpp"
#include "arene/base/algorithm/detail/traits.hpp"
#include "arene/base/constraints.hpp"
#include "arene/base/iterator/advance.hpp"
#include "arene/base/type_traits/is_predicate.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"

namespace std {
// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
/// @brief Find the first position in a range where the predicate is false. Returns the end of the range if there is no
/// such position.
/// @tparam InputIterator The type of the iterator used to mark the range
/// @tparam Predicate The type of the predicate
/// @param first The start of the range
/// @param last The end of the range
/// @param pred The predicate
/// @return The first iterator in @c [first,last) for which @c pred(*it) is @c false, or @c last if no such iterator
/// exists
/// @pre @c InputIterator must be an input iterator
/// @pre @c [first,last) must denote a valid range
/// @pre @c pred(*first) must be well-formed and return a boolean-testable value
template <
    typename InputIterator,
    typename Predicate,
    arene::base::constraints<
        enable_if_t<arene::base::is_input_iterator_v<InputIterator>>,
        enable_if_t<
            arene::base::is_predicate_v<Predicate&, arene::base::algorithm_detail::iter_reference_t<InputIterator>>>> =
        nullptr>
auto find_if_not(InputIterator first, InputIterator last, Predicate pred) noexcept(
    noexcept(arene::base::advance(first, 1)) &&  //
    noexcept(first != last) &&                   //
    noexcept(!pred(*first))
) -> InputIterator {
  while (first != last) {
    if (!pred(*first)) {
      break;
    }
    arene::base::advance(first, 1);
  }
  return first;
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_FIND_IF_NOT_HPP_

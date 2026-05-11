// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_NONE_OF_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_NONE_OF_HPP_

// IWYU pragma: private, include <algorithm>
// IWYU pragma: friend "stdlib_detail/.*"

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

#include "arene/base/algorithm/detail/functional.hpp"
#include "arene/base/algorithm/detail/traits.hpp"
#include "arene/base/constraints.hpp"
#include "arene/base/type_traits/is_predicate.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "arene/base/utility/make_subrange.hpp"
#include "stdlib/include/stdlib_detail/declval.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/forward.hpp"

namespace std {
// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
/// @brief Check if the predicate is false for every element in the range. Returns @c true if there is no element in the
/// range for which the predicate returns @c true, @c false otherwise
/// @tparam InputIterator The type of the iterator used to mark the range
/// @tparam Predicate The type of the predicate
/// @param first The start of the range
/// @param last The end of the range
/// @param pred The predicate
/// @return @c true if there is no element in the range @c [first,last) for which @c pred(*it) returns @c true
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
auto none_of(InputIterator first, InputIterator last, Predicate pred) noexcept(
    noexcept(arene::base::make_subrange(first, last)) &&  //
    noexcept(pred(*first)) &&                             //
    noexcept(++first)
) -> bool {
  // parasoft-begin-suppress AUTOSAR-A7_1_5-a "False Positive: 'element' has return type of range element, which may
  // be non-fundamental"
  for (auto&& element : arene::base::make_subrange(first, last)) {
    // parasoft-end-suppress AUTOSAR-A7_1_5-a
    if (pred(std::forward<decltype(element)>(element))) {
      return false;
    }
  }
  return true;
}
// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2
}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_NONE_OF_HPP_

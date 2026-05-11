// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_COUNT_IF_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_COUNT_IF_HPP_

#include "arene/base/algorithm/detail/functional.hpp"
#include "arene/base/algorithm/detail/traits.hpp"
#include "arene/base/constraints.hpp"
#include "arene/base/type_traits/is_predicate.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/is_convertible.hpp"

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <algorithm>
// IWYU pragma: friend "stdlib_detail/.*"

namespace std {

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
/// @brief Count the number of elements in a range satisfying a predicate
/// @tparam InputIterator The type of the iterators denoting the input range.
/// @tparam UnaryPredicate The type of the unary callable that returns a boolean testable value
/// @param begin The start of the range
/// @param end The end of the range
/// @param predicate The unary predicate which returns @c true for the counted elements
/// @return The count of elements in the range for which @c predicate returns @c true
/// @pre @c InputIterator must satisfy the input iterator requirements.
/// @pre @c [begin,end) must be a valid range.
/// @pre The expression @c predicate(v) must be convertible to bool for every argument v of type (possibly const) VT,
/// where VT is the value type of InputIterator, regardless of value category, and must not modify v.
template <
    class InputIterator,
    class UnaryPredicate,
    arene::base::constraints<
        enable_if_t<arene::base::is_input_iterator_v<InputIterator>>,
        enable_if_t<arene::base::is_predicate_v<
            UnaryPredicate&,
            arene::base::algorithm_detail::iter_reference_t<InputIterator>>>> = nullptr>
constexpr auto count_if(InputIterator begin, InputIterator end, UnaryPredicate predicate) ->
    typename std::iterator_traits<InputIterator>::difference_type {
  typename std::iterator_traits<InputIterator>::difference_type count{0};
  while (begin != end) {
    if (static_cast<bool>(predicate(*begin))) {
      ++count;
    }
    // parasoft-begin-suppress AUTOSAR-M5_0_15-a-2 "This is an iterator type, so incrementing is OK"
    ++begin;
    // parasoft-end-suppress AUTOSAR-M5_0_15-a-2
  }
  return count;
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_COUNT_IF_HPP_

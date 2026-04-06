// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_FIND_IF_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_FIND_IF_HPP_

// IWYU pragma: private, include "arene/base/algorithm.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/iterator/advance.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/type_traits/denotes_range.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
/// @brief Returns an iterator to the first element in the range [first, last)
/// that satisfies equality criteria: an element for which predicate @c pred
/// returns @c true
/// @tparam Iterator The type of the Iterator to conduct the search
/// @tparam Value Value to find
/// @tparam Predicate Condition predicate to find a value
/// @param first Start of the range to find the value
/// @param last End (exclusive) of the range to find the value
/// @param pred Unary predicate which returns @c true for the elements to find
/// @return Iterator for the first found item
template <
    typename Iterator,
    class Predicate,
    constraints<
        std::enable_if_t<base::is_input_iterator_v<Iterator>>,
        std::enable_if_t<
            base::is_invocable_r_v<bool, Predicate&, typename std::iterator_traits<Iterator>::reference>>> = nullptr>
constexpr auto find_if(Iterator first, Iterator last, Predicate pred)  // CODEQLFP(A15-4-4)
    noexcept(denotes_nothrow_iterable_range_v<Iterator> && base::is_nothrow_invocable_v<Predicate&, typename std::iterator_traits<Iterator>::reference>)
        -> Iterator {
  // parasoft-begin-suppress AUTOSAR-A6_5_2-a-2 "False Positive: The loop counter is 'pos'"
  for (Iterator& pos{first}; pos != last; arene::base::advance(pos, 1)) {  // CODEQLFP(M6-5-5) CODEQLFP(CTR55-CPP)
    if (pred(*pos)) {
      return pos;
    }
  }  // CODEQLFP(A5-0-2)
  // parasoft-end-suppress AUTOSAR-A6_5_2-a-2
  return last;
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_FIND_IF_HPP_

// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_FIND_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_FIND_HPP_

// IWYU pragma: private, include "arene/base/algorithm.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/iterator/advance.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/type_traits/denotes_range.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

/// @brief Constexpr-compatible implementation of @c std::find.
/// @tparam Iterator The type of the iterators
/// @tparam Value The type of the value being searched for
/// @param first The iterator for the start of the range
/// @param last The iterator for the end of the range
/// @param value_to_find The value to search for
/// @return Iterator The first iterator for which @c *it==value, or @c last if there is
/// no such iterator
template <
    typename Iterator,
    typename Value,
    constraints<
        std::enable_if_t<base::is_input_iterator_v<Iterator>>,
        std::enable_if_t<
            base::is_equality_comparable_v<typename std::iterator_traits<Iterator>::reference, Value const&>>> =
        nullptr>
constexpr auto find(
    Iterator first,
    Iterator last,  // CODEQLFP(DCL51-CPP) CODEQLFP(A7-1-1) CODEQLFP(A15-4-4)
    Value const& value_to_find
)  //
    noexcept(
        denotes_nothrow_iterable_range_v<Iterator, Iterator>&& noexcept(*first == value_to_find) &&
        std::is_nothrow_copy_constructible<Iterator>::value
    ) -> Iterator {  // CODEQLFP(EXP52-CPP)
  // CODEQLFP(A5-2-6)
  // parasoft-begin-suppress AUTOSAR-A6_5_2-a-2 "False positive: The loop counter is 'pos'"
  for (Iterator& pos{first}; pos != last; arene::base::advance(pos, 1)) {  // CODEQLFP(CTR55-CPP) CODEQLFP(M6-5-5)
    if (*pos == value_to_find) {
      return pos;
    }
  }  // CODEQLFP(A5-0-2)
  // parasoft-end-suppress AUTOSAR-A6_5_2-a-2
  return last;
}

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_FIND_HPP_

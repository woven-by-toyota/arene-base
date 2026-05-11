// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_SWAP_RANGES_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_SWAP_RANGES_HPP_

// IWYU pragma: private, include "arene/base/algorithm.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/algorithm/iter_swap.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

/// @brief Exchanges elements between range [first1, last1) and another range
/// starting at first2.
/// Precondition: the two ranges [first1, last1) and [first2, last2) do not
/// overlap, where last2 = std::next(first2, std::distance(first1, last1)).
/// @tparam SourceIterator The type of the Iterator for the first range
/// @tparam DestIterator The type of the Iterator for the second range
/// @param first1 Beginning of the first range of elements to swap
/// @param last1 End of the first range of elements to swap
/// @param first2 Beginning of the second range of elements to swap
/// @return End of the second range of swapped elements // CODEQLFP(EXP52-CPP) // CODEQLFP(DCL51-CPP)
template <
    class SourceIterator,
    class DestIterator,
    constraints<
        std::enable_if_t<is_input_iterator_v<SourceIterator>>,
        std::enable_if_t<is_input_iterator_v<DestIterator>>,
        std::enable_if_t<is_output_iterator_v<SourceIterator>>,
        std::enable_if_t<is_output_iterator_v<DestIterator>>> = nullptr>
constexpr auto swap_ranges(SourceIterator first1, SourceIterator last1, DestIterator first2) noexcept(
    noexcept(::arene::base::iter_swap(first1++, first2++)) && noexcept(first1 != last1)
) -> DestIterator {
  while (first1 != last1) {
    ::arene::base::iter_swap(first1++, first2++);
  }  // CODEQLFP(A5-0-2)
  return first2;
}

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_SWAP_RANGES_HPP_

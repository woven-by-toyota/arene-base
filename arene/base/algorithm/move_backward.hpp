// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_MOVE_BACKWARD_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_MOVE_BACKWARD_HPP_

// IWYU pragma: private, include "arene/base/algorithm.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_assignable.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

/// @brief Moves the elements from the range <tt>[first, last)</tt>, to another
/// range ending at @c result. The elements are moved in reverse order (the last
/// element is moved first), but their relative order is preserved.
/// @tparam InIterator The type of the Iterator for the range to take items from
/// @tparam OutIterator The type of the Iterator for the range to put items into
/// @param first Start of the range to take items from
/// @param last End (exclusive) of the range to take items from
/// @param result Start of the range to put items into
/// @return OutIterator for the last moved item

template <
    class InIterator,
    class OutIterator,
    constraints<
        std::enable_if_t<base::is_input_iterator_v<InIterator>>,
        std::enable_if_t<base::is_output_iterator_v<OutIterator>>,
        std::enable_if_t<std::is_assignable<
            decltype(*std::declval<OutIterator>()),
            typename std::iterator_traits<InIterator>::value_type&&>::value>> = nullptr>
// clang-format off
constexpr auto move_backward(            // CODEQLFP(DCL51-CPP) CODEQLFP(A15-4-4)
    InIterator first, InIterator last,          // CODEQLFP(A7-1-1)
    OutIterator result) noexcept(               //
    noexcept(--result) && noexcept(*result) &&  // CODEQLFP(EXP52-CPP) CODEQLFP(A5-2-6) CODEQLFP(M5-14-1)
    noexcept(*last) && noexcept(--last) &&      // CODEQLFP(EXP52-CPP) CODEQLFP(A5-2-6) CODEQLFP(M5-14-1)
    noexcept(first != last) &&                  // CODEQLFP(EXP52-CPP) CODEQLFP(A5-2-6) CODEQLFP(M5-14-1)
    std::is_nothrow_assignable<decltype(*std::declval<OutIterator>()),
                               typename std::iterator_traits<InIterator>::value_type>::value) -> OutIterator  // CODEQLFP(A5-1-1) CODEQLFP(M5-3-1)
// clang-format on
{
  while (first != last) {
    // parasoft-begin-suppress AUTOSAR-A18_9_3-a-2 "False Positive: Generic code, moved object is not explicitly const"
    *--result = std::move(*--last);  // CODEQLFP(A4-7-1) CODEQLFP(M5-2-10)
    // parasoft-end-suppress AUTOSAR-A18_9_3-a-2
  }  // CODEQLFP(A5-0-2)
  return result;
}

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_MOVE_BACKWARD_HPP_

// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_MOVE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_MOVE_HPP_

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
#include "arene/base/utility/make_subrange.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
/// @brief Moves the elements in the range <tt>[first, last)</tt>, to another
/// range beginning at @c output, starting from @c first and proceeding to
/// <tt>last - 1</tt>. After this operation the elements in the moved-from range
/// will still contain valid values of the appropriate type, but not necessarily
/// the same values as before the move.
/// @tparam InIterator The type of the Iterator for the range to take items from
/// @tparam OutIterator The type of the Iterator for the range to put items into
/// @param first Start of the range to take items from
/// @param last End (exclusive) of the range to take items from
/// @param output Start of the range to put items into
/// @return OutIterator for the element past the last moved item
template <
    class InIterator,
    class OutIterator,
    constraints<
        std::enable_if_t<base::is_input_iterator_v<InIterator>>,
        std::enable_if_t<base::is_output_iterator_v<OutIterator>>,
        std::enable_if_t<std::is_assignable<
            decltype(*std::declval<OutIterator>()),
            typename std::iterator_traits<InIterator>::value_type&&>::value>> = nullptr>
constexpr auto move(
    InIterator first,
    InIterator last,  // CODEQLFP(DCL51-CPP) CODEQLFP(A15-4-4)
    OutIterator output
)  //
    noexcept(
        noexcept(*first++) && noexcept(output++) &&  // CODEQLFP(EXP52-CPP) CODEQLFP(A5-2-6)
        noexcept(first != last) &&                   // CODEQLFP(A5-2-6)
        std::is_nothrow_assignable<
            decltype(*std::declval<OutIterator>()),
            typename std::iterator_traits<InIterator>::value_type>::value
    ) -> OutIterator {                                             // CODEQLFP(M5-3-1)
  for (auto& element : arene::base::make_subrange(first, last)) {  // CODEQLFP(A7-1-1) CODEQLFP(A7-1-2)
    // parasoft-begin-suppress AUTOSAR-A18_9_3-a-2 "False Positive: Generic code, 'element' is not explicitly const"
    *output++ = std::move(element);  // CODEQLFP(CTR55-CPP) CODEQLFP(A4-7-1) CODEQLFP(M5-2-10)
    // parasoft-end-suppress AUTOSAR-A18_9_3-a-2
  }  // CODEQLFP(A5-0-2)
  return output;
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_MOVE_HPP_

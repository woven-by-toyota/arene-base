// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_COPY_IF_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_COPY_IF_HPP_

// IWYU pragma: private, include "arene/base/algorithm.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_assignable.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "arene/base/utility/make_subrange.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
/// @brief Copies the elements in the range, defined by <tt>[first, last)</tt>,
/// to another range beginning at @c output. Only copies the elements for which
/// the predicate @c pred returns true. The relative order of the elements that
/// are copied is preserved.
/// @tparam InputIterator The type of the Source iterator
/// @tparam OutputIterator The type of the Destination iterator
/// @tparam Predicate Function to test whether a value might be copied
/// @param first Start of the range of elements to copy from
/// @param last End (non-inclusive) of the range of elements to copy from
/// @param output Start of the range of elements to copy into
/// @param pred Unary predicate which returns @c true for the required elements
/// @return OutputIterator for the last inserted item
template <class InputIterator, class OutputIterator, class Predicate,
          constraints<std::enable_if_t<base::is_input_iterator_v<InputIterator>>,
                      std::enable_if_t<base::is_output_iterator_v<OutputIterator>>,
                      std::enable_if_t<base::is_invocable_r_v<
                          bool, Predicate &, typename std::iterator_traits<InputIterator>::value_type>>> = nullptr>
// clang-format off
constexpr auto copy_if(  // CODEQLFP(DCL51-CPP) CODEQLFP(A7-1-1) CODEQLFP(A15-4-4)
    InputIterator first, InputIterator last, OutputIterator output,
    Predicate pred) noexcept(noexcept(*output++) &&      // CODEQLFP(EXP52-CPP)
                             noexcept(++first) &&        // CODEQLFP(EXP52-CPP) CODEQLFP(A5-2-6) CODEQLFP(M5-14-1)
                             noexcept(*first) &&         // CODEQLFP(EXP52-CPP) CODEQLFP(A5-2-6) CODEQLFP(M5-14-1)
                             noexcept(first != last) &&  // CODEQLFP(A5-2-6)
                             std::is_nothrow_assignable<decltype(*std::declval<OutputIterator&>()),
                                                        typename std::iterator_traits<InputIterator>::reference>::
                                 value &&                   // CODEQLFP(A5-1-1) CODEQLFP(M5-3-1)
                             base::is_nothrow_invocable_v<  //
                                 Predicate&,
                                 typename std::iterator_traits<InputIterator>::value_type>) -> OutputIterator  // CODEQLFP(M5-3-1) CODEQLFP(A5-2-6)
// clang-format on
{
  // parasoft-begin-suppress AUTOSAR-A7_1_5-a "False positive: 'element' has return type of range element, which may
  // be non-fundamental"
  for (auto& element : arene::base::make_subrange(first, last)) {  // CODEQLFP(A7-1-1) CODEQLFP(A7-1-2)
    if (pred(element)) {                                           // CODEQLFP(M0-1-2)
      // parasoft-begin-suppress AUTOSAR-M5_0_15-a "These are iterator types, so incrementing is ok"
      // parasoft-begin-suppress AUTOSAR-M5_2_10-a "idiomatic iterator operations permitted by M5-2-10 Permit #1"
      *output++ = element;  // CODEQLFP(CTR55-CPP) CODEQLFP(A4-7-1) CODEQLFP(M5-2-10)
      // parasoft-end-suppress AUTOSAR-M5_0_15-a
      // parasoft-end-suppress AUTOSAR-M5_2_10-a
    }
  }  // CODEQLFP(A5-0-2)
  // parasoft-end-suppress AUTOSAR-A7_1_5-a

  return output;
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_COPY_IF_HPP_

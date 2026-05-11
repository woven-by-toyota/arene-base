// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_COPY_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_COPY_HPP_

// IWYU pragma: private, include "arene/base/algorithm.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_assignable.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: this function does not have internal linkage"
// parasoft-begin-suppress AUTOSAR-M5_0_15-a "False positive: this is generic iterator arithmetic, not pointer-specific"

/// @brief Copies the elements in the range, defined by <tt>[first, last)</tt>,
/// to another range beginning at @c output.
/// @tparam InputIterator The type of the Source iterator
/// @tparam OutputIterator The type of the Destination iterator
/// @param first Start of the range of elements to copy from
/// @param last End (non-inclusive) of the range of elements to copy from
/// @param output Start of the range of elements to copy into
/// @return OutputIterator An iterator after the last copied item
template <
    class InputIterator,
    class OutputIterator,
    constraints<
        std::enable_if_t<base::is_input_iterator_v<InputIterator>>,
        std::enable_if_t<base::is_output_iterator_v<OutputIterator>>> = nullptr>
constexpr auto copy(InputIterator first, InputIterator last, OutputIterator output) noexcept(
    noexcept(*(++output)) && noexcept(*(++first)) && noexcept(first != last) &&
    std::is_nothrow_assignable<
        decltype(*std::declval<OutputIterator&>()),
        typename std::iterator_traits<InputIterator>::reference>::value
) -> OutputIterator {
  while (first != last) {
    *output = *first;
    ++first;
    ++output;
  }

  return output;
}

// parasoft-end-suppress AUTOSAR-M3_3_2-a
// parasoft-end-suppress AUTOSAR-M5_0_15-a

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_COPY_HPP_

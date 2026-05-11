// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_ACCUMULATE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_ACCUMULATE_HPP_

// IWYU pragma: private, include <numeric>
// IWYU pragma: friend "stdlib_detail/.*"

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

#include "arene/base/constraints.hpp"
#include "arene/base/utility/make_subrange.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/forward.hpp"
#include "stdlib/include/stdlib_detail/is_assignable.hpp"
#include "stdlib/include/stdlib_detail/is_copy_assignable.hpp"
#include "stdlib/include/stdlib_detail/is_copy_constructible.hpp"
#include "stdlib/include/stdlib_detail/iterator_concepts.hpp"

namespace std {

// parasoft-begin-suppress CERT_CPP-ERR55-a-2 "False positive: Exception specification is conditional"
// parasoft-begin-suppress CERT_CPP-ERR50-h-3 "False positive: Exception specification is conditional"
// parasoft-begin-suppress AUTOSAR-A15_5_3-h-2 "False positive: Exception specification is conditional"
// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
/// @brief Perform a left-fold on the input range @c [first,last) starting with the value @c init . For each element @c
/// e in the range, does @c init=init+e
/// @tparam InputIterator The iterator type of the range
/// @tparam T The type of the initial value and return value
/// @param first The start of the range
/// @param last The end of the range
/// @param init The initial value
/// @return @c init as modified by the operations
/// @pre @c InputIterator must be a valid input iterator
/// @pre @c [first,last) must be a valid range
/// @pre @c init=init+*first must be defined.
/// @throws Any exception thrown from iterator operations, or @c init=init+e
template <
    typename InputIterator,
    typename T,
    arene::base::constraints<enable_if_t<internal::has_basic_input_iterator_operations_v<InputIterator>>> = nullptr>
auto accumulate(InputIterator first, InputIterator last, T init) noexcept(
    internal::has_nothrow_basic_input_iterator_operations_v<InputIterator>&& noexcept(
        std::declval<T&>() = std::declval<T&>() + *std::declval<InputIterator&>()
    )
) -> T {
  static_assert(
      is_copy_constructible_v<T> && is_copy_assignable_v<T>,
      "The 'init' value must be a copy-constructible and copy-assignable type"
  );
  // parasoft-begin-suppress AUTOSAR-A7_1_5-a "False Positive: 'element' has return type of range element, which may
  // be non-fundamental"
  for (auto&& element : arene::base::make_subrange(first, last)) {
    // parasoft-end-suppress AUTOSAR-A7_1_5-a
    // parasoft-begin-suppress CERT_CPP-ERR51-b    "False positive: Thrown exception is propagated"
    // parasoft-begin-suppress AUTOSAR-A15_3_2-a   "False positive: Thrown exception is propagated"
    // parasoft-begin-suppress AUTOSAR-A15_5_3-g   "False positive: Thrown exception is propagated"
    // parasoft-begin-suppress CERT_CPP-ERR50-g    "False positive: Thrown exception is propagated"
    // parasoft-begin-suppress AUTOSAR-M15_3_4-b   "False positive: Thrown exception is propagated"
    // parasoft-begin-suppress AUTOSAR-A15_2_1   "False positive: Thrown exception is propagated"
    init = init + std::forward<decltype(element)>(element);
    // parasoft-end-suppress AUTOSAR-A15_2_1
    // parasoft-end-suppress AUTOSAR-M15_3_4-b
    // parasoft-end-suppress CERT_CPP-ERR50-g
    // parasoft-end-suppress AUTOSAR-A15_5_3-g
    // parasoft-end-suppress AUTOSAR-A15_3_2-a
    // parasoft-end-suppress CERT_CPP-ERR51-b
  }

  return init;
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2
// parasoft-end-suppress CERT_CPP-ERR51-b-3
// parasoft-end-suppress AUTOSAR-A15_5_3-h-2
// parasoft-end-suppress CERT_CPP-ERR55-a-2
// parasoft-end-suppress CERT_CPP-ERR50-h-3

// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
/// @brief Perform a left-fold on the input range @c [first,last) starting with the value @c init using the specified @c
/// binary_op . For each element @c e in the range, does @c init=binary_op(init,e)
/// @tparam InputIterator The iterator type of the range
/// @tparam T The type of the initial value and return value
/// @tparam BinaryOperation The type of the binary operation
/// @param first The start of the range
/// @param last The end of the range
/// @param init The initial value
/// @param binary_op The operator to use to accumulate the values
/// @return @c init as modified by the operations
/// @pre @c InputIterator must be a valid input iterator
/// @pre @c [first,last) must be a valid range
/// @pre @c init=binary_op(init,*first) must be defined.
/// @throws Any exception thrown from iterator operations, or @c init=binary_op(init,*first)
template <
    typename InputIterator,
    typename T,
    typename BinaryOperation,
    arene::base::constraints<
        enable_if_t<internal::has_basic_input_iterator_operations_v<InputIterator>>,
        enable_if_t<is_assignable_v<
            T&,
            decltype(std::declval<BinaryOperation&>()(std::declval<T&>(), *std::declval<InputIterator&>()))>>> =
        nullptr>
auto accumulate(InputIterator first, InputIterator last, T init, BinaryOperation binary_op) noexcept(
    internal::has_nothrow_basic_input_iterator_operations_v<InputIterator>&& noexcept(
        std::declval<T&>() = std::declval<BinaryOperation&>()(std::declval<T&>(), *std::declval<InputIterator&>())
    )
) -> T {
  static_assert(
      is_copy_constructible_v<T> && is_copy_assignable_v<T>,
      "The 'init' value must be a copy-constructible and copy-assignable type"
  );
  // parasoft-begin-suppress AUTOSAR-A7_1_5-a "False Positive: 'element' has return type of range element, which may
  // be non-fundamental"
  for (auto&& element : arene::base::make_subrange(first, last)) {
    // parasoft-end-suppress AUTOSAR-A7_1_5-a
    init = binary_op(init, std::forward<decltype(element)>(element));
  }
  return init;
}
// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_ACCUMULATE_HPP_

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_MOVE_ALGORITHM_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_MOVE_ALGORITHM_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <algorithm>
// IWYU pragma: friend "stdlib_detail/.*"

#include "arene/base/constraints.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/ignore.hpp"
#include "stdlib/include/stdlib_detail/iterator_concepts.hpp"
#include "stdlib/include/stdlib_detail/move.hpp"

namespace std {

// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"

/// @brief Move elements from a source range to a destination range. Implements @c std::move
///
/// For each element @c e in the half-open range @c [begin,end) in order, writes @c std::move(e) to @c *result and
/// increments @c result
///
/// @tparam InputIterator The type of the iterators denoting the input range.
/// @tparam OutputIterator The type of the iterator for the output range
/// @param begin The start of the source range
/// @param end The end of the source range
/// @param result The start of the destination range
/// @return An @c OutputIterator equivalent to the final value of @c result
/// @pre @c InputIterator must satisfy the input iterator requirements.
/// @pre @c [begin,end) must be a valid range.
/// @pre @c OutputIterator must satisfy the output iterator requirements.
/// @pre @c result must be a valid iterator for an output range that can be written to at least as many times as there
/// are elements in the input range.
/// @pre The value types of the input and output range must be compatible such that @c *result=std::move(*begin) is
/// well-formed.
/// @throws Any exception thrown by the iteration or move-assignment operations
template <
    typename InputIterator,
    typename OutputIterator,
    arene::base::constraints<
        enable_if_t<internal::has_basic_input_iterator_operations_v<InputIterator>>,
        enable_if_t<internal::has_basic_output_iterator_operations_v<OutputIterator>>,
        enable_if_t<internal::is_indirectly_move_assignable_v<InputIterator, OutputIterator>>> = nullptr>
constexpr auto move(
    InputIterator begin,
    InputIterator end,
    OutputIterator result
  ) noexcept(noexcept(*result = std::move(*begin)) && //
             internal::has_nothrow_basic_input_iterator_operations_v<InputIterator> && //
             internal::has_nothrow_basic_output_iterator_operations_v<OutputIterator>)
    -> OutputIterator {
  while (begin != end) {
    // parasoft-begin-suppress AUTOSAR-A18_9_3-a-2 "Generic function; const or not is specified by user"
    *result = std::move(*begin);
    // parasoft-end-suppress AUTOSAR-A18_9_3-a-2
    // parasoft-begin-suppress AUTOSAR-M5_0_15-a-2 "This is an iterator type, so incrementing is OK"
    ++begin;
    ++result;
    // parasoft-end-suppress AUTOSAR-M5_0_15-a-2
  }
  return result;
}

/// @brief Move elements from a source range to a destination range. Implements @c std::move
///
/// For each element @c e in the half-open range @c [begin,end) writes @c std::move(e) to @c *result and increments @c
/// result.
///
/// This overload is to generate an error for the case that the elements from the source cannot be assigned to the
/// destination
///
/// @tparam InputIterator The type of the iterators denoting the input range.
/// @tparam OutputIterator The type of the iterator for the output range
/// @param begin The start of the source range
/// @param end The end of the source range
/// @param result The start of the destination range
/// @return An @c OutputIterator equivalent to the final value of @c result
/// @pre @c InputIterator must satisfy the input iterator requirements.
/// @pre @c [begin,end) must be a valid range.
/// @pre @c OutputIterator must satisfy the output iterator requirements.
/// @pre @c result must be a valid iterator for an output range that can be written to at least as many times as there
/// are elements in the input range.
/// @pre The value types of the input and output range must be compatible such that @c *result=std::move(*begin) is
/// well-formed.
/// @throws Any exception thrown by the iteration or move-assignment operations
template <
    typename InputIterator,
    typename OutputIterator,
    arene::base::constraints<
        enable_if_t<internal::has_basic_input_iterator_operations_v<InputIterator>>,
        enable_if_t<internal::has_basic_output_iterator_operations_v<OutputIterator>>,
        enable_if_t<!internal::is_indirectly_move_assignable_v<InputIterator, OutputIterator>>> = nullptr>
auto move(InputIterator begin, InputIterator end, OutputIterator result) -> OutputIterator {
  ignore = begin;
  ignore = end;
  constexpr bool
      can_move_assign_via_iterators{internal::is_indirectly_move_assignable_v<InputIterator, OutputIterator>};
  static_assert(can_move_assign_via_iterators, "Must be able to move-assign to output from input");
  return result;
}

// parasoft-end-suppress AUTOSAR-M3_3_2-a-2

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_MOVE_ALGORITHM_HPP_

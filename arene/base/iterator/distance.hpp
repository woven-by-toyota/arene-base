// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_DISTANCE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_DISTANCE_HPP_

// IWYU pragma: private, include "arene/base/iterator.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"

namespace arene {
namespace base {

namespace distance_impl {

// parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"

// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
/// @brief Implementation helper for @c arene::base::iterator::distance for random-access iterators.
/// @tparam Iterator the type of the iterator
/// @param first The first iterator in the range
/// @param last The last iterator in the range
/// @return The number of times @c first must be incremented to reach @c last
template <typename Iterator>
constexpr auto do_distance(
    Iterator first,
    Iterator last,
    std::random_access_iterator_tag
) noexcept(noexcept(last - first))  //
    -> typename std::iterator_traits<Iterator>::difference_type {
  // parasoft-begin-suppress AUTOSAR-M5_0_15-a-2 "False positive: first is an iterator, bounded by last"
  return last - first;
  // parasoft-end-suppress AUTOSAR-M5_0_15-a-2
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2

/// @brief Implementation helper for @c arene::base::iterator::distance for input iterators.
/// @tparam Iterator the type of the iterator
/// @param first The first iterator in the range
/// @param last The last iterator in the range
/// @return The number of times @c first must be incremented to reach @c last
template <typename Iterator>
constexpr auto do_distance(
    Iterator first,
    Iterator last,
    std::input_iterator_tag
) noexcept(noexcept(first != last) && noexcept(++first))  //
    -> typename std::iterator_traits<Iterator>::difference_type {
  typename std::iterator_traits<Iterator>::difference_type result{0};
  while (first != last) {
    ++first;
    ++result;
  }
  return result;
}

// parasoft-end-suppress CERT_C-EXP37-a

}  // namespace distance_impl

// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
/// @brief Find the number of elements between @c first and @c last.
/// @tparam Iterator The type of iterator to compute the distance between
/// @param first The iterator to start the distance computation from
/// @param last The iterator end the distance computation at
/// @return The number of elements needed to go from @c first to @c last.
///         For iterators satisfying @c random_access_iterator, this value
///         can be negative if @c first is reachable from @c last.
/// @pre @c last must be reachable from @c first, or else the behavior is
///         undefined.
template <typename Iterator>
constexpr auto distance(Iterator first, Iterator last) noexcept(
    noexcept(distance_impl::do_distance(first, last, typename std::iterator_traits<Iterator>::iterator_category{}))
)  //
    -> typename std::iterator_traits<Iterator>::difference_type {
  return distance_impl::do_distance(first, last, typename std::iterator_traits<Iterator>::iterator_category{});
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_DISTANCE_HPP_

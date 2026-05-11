// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file fill.hpp
/// @brief Provides implementation of constexpr backport of @c std::fill
///
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_FILL_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_FILL_HPP_

// IWYU pragma: private, include "arene/base/algorithm.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_assignable.hpp"
#include "arene/base/type_traits/denotes_range.hpp"
#include "arene/base/utility/make_subrange.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

///
/// @brief Assigns all elements in a sequence to a given value.
///
/// @tparam Itr The type of the iterator for the sequence. Its @c reference must be @c std::assignable from @c T
/// @tparam Sent The type of the sentinel to mark the end of the sequence. It must be inequality comparable with @c Itr
/// @tparam T The type of the value to fill the sequence with.
/// @param first Iterator to the first position in the sequence to fill.
/// @param last Sentinel to mark the end of the sequence to fill.
/// @param value The value to fill the sequence with.
/// @post All elements in the sequence @c [first,last) will have their element assigned to @c value
/// @pre Repeated incrementing of @c first must eventually produce an iterator which compares equal to @c last , else
/// behavior is undefined.
///
/// Performs exactly @c std::distance(first,last) assignments of @c value . Backport of the @c constexpr
/// [std::fill](https://en.cppreference.com/w/cpp/algorithm/fill) from C++20 .
///
template <
    typename Itr,
    typename Sent,
    typename T,
    constraints<
        std::enable_if_t<denotes_range_v<Itr, Sent>>,
        std::enable_if_t<std::is_assignable<decltype(*std::declval<Itr>()), T const&>::value>> = nullptr>
constexpr void fill(
    Itr first,
    Sent last,
    T const& value
) noexcept(std::is_nothrow_assignable<decltype(*std::declval<Itr>()), T const&>::value && denotes_nothrow_iterable_range_v<Itr, Sent>) {
  for (auto& element : make_subrange(first, last)) {
    element = value;
  }
}

}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_FILL_HPP_

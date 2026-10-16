#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_DETAIL_IS_SENTINEL_FOR_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_DETAIL_IS_SENTINEL_FOR_HPP_

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// IWYU pragma: private, include "arene/base/ranges.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/ranges/detail/is_input_or_output_iterator.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

namespace arene {
namespace base {
namespace ranges {
namespace detail {

/// @brief Checks if a given sentinel and iterator form a valid sentinel-iterator pair.
/// @tparam Sentinel The type of the sentinel.
/// @tparam Itr The type of the iterator.
/// @return bool If @c Itr models @c is_input_or_output_iterator, @c Sentinel is copy constructible, and @c Sentinel and
///         @c Itr are equality and inequality comparable then @c true , else @c false
template <typename Sentinel, typename Itr>
constexpr bool is_sentinel_for_v =
    is_input_or_output_iterator_v<Itr> && std::is_default_constructible<Sentinel>::value &&
    std::is_copy_constructible<Sentinel>::value && is_equality_comparable_v<Sentinel, Itr> &&
    is_inequality_comparable_v<Sentinel, Itr>;

}  // namespace detail
}  // namespace ranges
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_DETAIL_IS_SENTINEL_FOR_HPP_

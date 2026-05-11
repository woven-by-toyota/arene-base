// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_REVERSE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_REVERSE_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <algorithm>
// IWYU pragma: friend "stdlib_detail/.*"

#include "arene/base/constraints.hpp"
#include "arene/base/type_traits/is_swappable.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/iter_swap.hpp"
#include "stdlib/include/stdlib_detail/iterator_concepts.hpp"
#include "stdlib/include/stdlib_detail/iterator_traits.hpp"

namespace std {

/// @brief reverses the order of elements in a range
/// @tparam BidirIt The iterator used to iterate over the input range
/// @param first the beginning of the range
/// @param last the end of the range
template <
    typename BidirIt,
    arene::base::constraints<
        std::enable_if_t<arene::base::is_bidirectional_iterator_v<BidirIt>>,
        std::enable_if_t<arene::base::is_swappable_v<typename std::iterator_traits<BidirIt>::value_type>>> = nullptr>
// clang-format cannot format the noexcept clause correctly
// clang-format off
constexpr auto reverse(
    BidirIt first,
    BidirIt last
) noexcept(
    internal::has_nothrow_basic_bidirectional_iterator_operations_v<BidirIt> &&
    arene::base::is_nothrow_swappable_v<typename std::iterator_traits<BidirIt>::value_type>) -> void {
  // clang-format on
  while (first != last && first != --last) {
    std::iter_swap(first, last);
    ++first;
  }
}

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_REVERSE_HPP_

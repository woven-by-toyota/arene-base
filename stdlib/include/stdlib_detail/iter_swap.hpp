// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_ITER_SWAP_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_ITER_SWAP_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <utility>
// IWYU pragma: friend "stdlib_detail/.*"

#include "arene/base/constraints.hpp"
#include "arene/base/type_traits/is_swappable.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/swap.hpp"

namespace std {

/// @brief swaps the objects pointed to by two iterators
///
/// @tparam ForwardIterator1 type of the first iterator
/// @tparam ForwardIterator2 type of the second iterator
/// @param lhs first iterator to object to swap
/// @param rhs second iterator to object to swap
template <
    class ForwardIterator1,
    class ForwardIterator2,
    arene::base::constraints<
        std::enable_if_t<arene::base::is_forward_iterator_v<ForwardIterator1>>,
        std::enable_if_t<arene::base::is_forward_iterator_v<ForwardIterator2>>,
        std::enable_if_t<arene::base::is_swappable_with_v<
            typename std::iterator_traits<ForwardIterator1>::reference,
            typename std::iterator_traits<ForwardIterator2>::reference>>> = nullptr>
constexpr auto iter_swap(
    ForwardIterator1 lhs,
    ForwardIterator2 rhs
) noexcept(arene::base::
               is_nothrow_swappable_with_v<
                   typename std::iterator_traits<ForwardIterator1>::reference,
                   typename std::iterator_traits<ForwardIterator2>::reference>) -> void {
  using std::swap;
  swap(*lhs, *rhs);
}

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_ITER_SWAP_HPP_

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_DETAIL_STD_REVERSE_ITERATOR_ENABLED_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_DETAIL_STD_REVERSE_ITERATOR_ENABLED_HPP_

#include <iterator>

namespace arene {
namespace base {
namespace reverse_iterator_detail {
/// @brief An alias for @c std::reverse_iterator for the supplied underlying iterator
/// @tparam Itr The type of the underlying iterator
template <typename Itr>
using std_reverse_iterator = std::reverse_iterator<Itr>;
}  // namespace reverse_iterator_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_DETAIL_STD_REVERSE_ITERATOR_ENABLED_HPP_

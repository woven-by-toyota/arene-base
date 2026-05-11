// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "defines traits used in algorithms"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_TRAITS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_TRAITS_HPP_

#include "arene/base/stdlib_choice/iterator_traits.hpp"

namespace arene {
namespace base {
namespace algorithm_detail {

/// @brief convenience alias for @c typename iterator_traits<I>::value_type
/// @tparam I iterator type
///
template <class I>
using iter_value_t = typename std::iterator_traits<I>::value_type;

/// @brief convenience alias for @c typename iterator_traits<I>::reference
/// @tparam I iterator type
///
template <class I>
using iter_reference_t = typename std::iterator_traits<I>::reference;

/// @brief convenience alias for @c typename iterator_traits<I>::difference_type
/// @tparam I iterator type
///
template <class I>
using iter_difference_t = typename std::iterator_traits<I>::difference_type;

}  // namespace algorithm_detail
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_TRAITS_HPP_

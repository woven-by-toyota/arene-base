// parasoft-begin-suppress AUTOSAR-A2_8_1-a "The header name reflects that it is defined if the std class is disabled"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_DETAIL_STD_REVERSE_ITERATOR_DISABLED_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_DETAIL_STD_REVERSE_ITERATOR_DISABLED_HPP_

#include "arene/base/type_manipulation/non_constructible_dummy.hpp"

namespace arene {
namespace base {
namespace reverse_iterator_detail {
/// @brief Non-constructible type to use where a @c std::reverse_iterator might be expected, but cannot be used
/// @tparam Itr The type of the underlying iterator
template <typename Itr>
class std_reverse_iterator : non_constructible_dummy {
 public:
  /// @brief Do-nothing function to allow @c noexcept clauses for @c reverse_iterator to compile
  void base() noexcept {}
};
}  // namespace reverse_iterator_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_DETAIL_STD_REVERSE_ITERATOR_DISABLED_HPP_

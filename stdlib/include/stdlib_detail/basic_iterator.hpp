// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "basic_iterator.hpp matches the section name in the standard"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_BASIC_ITERATOR_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_BASIC_ITERATOR_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <iterator>
#include "stdlib/include/stdlib_detail/cstddef.hpp"

namespace std {

/// @brief base class to ease the definition of required types for simple
/// iterators
/// @tparam Category iterator category tag type
/// @tparam T iterator value type
/// @tparam Distance iterator difference type
/// @tparam Pointer iterator pointer type
/// @tparam Reference iterator reference type
///
/// The @c std::iterator template may be used as a base class to ease the definition
/// of required types for new iterators.
///
template <class Category, class T, class Distance = ptrdiff_t, class Pointer = T*, class Reference = T&>
class iterator {
 public:
  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: No identifiers are being hidden"

  /// @brief iterator value type
  using value_type = T;
  /// @brief iterator difference type
  using difference_type = Distance;
  /// @brief iterator pointer type
  using pointer = Pointer;
  /// @brief iterator reference type
  using reference = Reference;
  /// @brief iterator category tag type
  using iterator_category = Category;

  // parasoft-end-suppress AUTOSAR-A2_10_1-d
};

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_BASIC_ITERATOR_HPP_

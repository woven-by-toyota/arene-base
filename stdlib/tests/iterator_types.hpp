// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_TESTS_ITERATOR_TYPES_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_TESTS_ITERATOR_TYPES_HPP_

#include "stdlib/include/cstdint"
#include "stdlib/include/iterator"

namespace testing {
template <typename ValueType>
struct non_throwing_forward_iterator {
  using iterator_category = std::forward_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = ValueType&;
  using value_type = ValueType;
  using pointer = value_type*;

  auto operator++() noexcept -> non_throwing_forward_iterator&;
  auto operator*() const noexcept -> reference;
  auto operator->() const noexcept -> pointer;

  auto operator++(int) noexcept -> non_throwing_forward_iterator;

  auto operator==(non_throwing_forward_iterator const&) const noexcept -> bool;
  auto operator!=(non_throwing_forward_iterator const&) const noexcept -> bool;
};

template <typename ValueType>
struct throwing_forward_iterator {
  using iterator_category = std::forward_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = ValueType&;
  using value_type = ValueType;
  using pointer = value_type*;

  auto operator++() noexcept -> throwing_forward_iterator&;
  auto operator*() const noexcept(false) -> reference;
  auto operator->() const noexcept -> pointer;

  auto operator++(int) noexcept -> throwing_forward_iterator;

  auto operator==(throwing_forward_iterator const&) const noexcept -> bool;
  auto operator!=(throwing_forward_iterator const&) const noexcept -> bool;
};

}  // namespace testing

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_TESTS_ITERATOR_TYPES_HPP_

// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_TEST_ITERATORS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_TEST_ITERATORS_HPP_

#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"

/*
 * Iterators for using in tests for noexcept conditions
 */
struct test_value {};

struct noexcept_iterator {
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = test_value&;
  using value_type = test_value;
  using pointer = void;

  auto operator*() noexcept -> reference;

  auto operator++() noexcept -> noexcept_iterator&;
  auto operator++(int) noexcept -> noexcept_iterator;
  auto operator--() noexcept -> noexcept_iterator&;
  auto operator--(int) noexcept -> noexcept_iterator;
  auto operator==(noexcept_iterator const&) noexcept -> bool;
  auto operator!=(noexcept_iterator const&) noexcept -> bool;
};

struct throwing_increment_iterator {
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = test_value&;
  using value_type = test_value;
  using pointer = void;

  auto operator*() noexcept -> reference;

  auto operator++() -> throwing_increment_iterator&;
  auto operator++(int) -> throwing_increment_iterator;
  auto operator--() noexcept -> throwing_increment_iterator&;
  auto operator--(int) noexcept -> throwing_increment_iterator;
  auto operator==(throwing_increment_iterator const&) noexcept -> bool;
  auto operator!=(throwing_increment_iterator const&) noexcept -> bool;
};

struct throwing_decrement_iterator {
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = test_value&;
  using value_type = test_value;
  using pointer = void;

  auto operator*() noexcept -> reference;

  auto operator++() noexcept -> throwing_decrement_iterator&;
  auto operator++(int) noexcept -> throwing_decrement_iterator;
  auto operator--() -> throwing_decrement_iterator&;
  auto operator--(int) -> throwing_decrement_iterator;
  auto operator==(throwing_decrement_iterator const&) noexcept -> bool;
  auto operator!=(throwing_decrement_iterator const&) noexcept -> bool;
};

struct throwing_comparison_iterator {
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = test_value&;
  using value_type = test_value;
  using pointer = void;

  auto operator*() noexcept -> reference;

  auto operator++() noexcept -> throwing_comparison_iterator&;
  auto operator++(int) noexcept -> throwing_comparison_iterator;
  auto operator--() noexcept -> throwing_comparison_iterator&;
  auto operator--(int) noexcept -> throwing_comparison_iterator;
  auto operator==(throwing_comparison_iterator const&) -> bool;
  auto operator!=(throwing_comparison_iterator const&) -> bool;
};

struct throwing_reference_iterator {
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = test_value&;
  using value_type = test_value;
  using pointer = void;

  auto operator*() -> reference;

  auto operator++() noexcept -> throwing_reference_iterator&;
  auto operator++(int) noexcept -> throwing_reference_iterator;
  auto operator--() noexcept -> throwing_reference_iterator&;
  auto operator--(int) noexcept -> throwing_reference_iterator;
  auto operator==(throwing_reference_iterator const&) noexcept -> bool;
  auto operator!=(throwing_reference_iterator const&) noexcept -> bool;
};

struct throwing_iterator {
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = test_value&;
  using value_type = test_value;
  using pointer = void;

  auto operator*() -> reference;

  auto operator++() -> throwing_iterator&;
  auto operator++(int) -> throwing_iterator;
  auto operator--() -> throwing_iterator&;
  auto operator--(int) -> throwing_iterator;
  auto operator==(throwing_iterator const&) -> bool;
  auto operator!=(throwing_iterator const&) -> bool;
};

struct throwing_assignment_iterator {
  // NOLINTNEXTLINE(hicpp-special-member-functions)
  struct value {
    value(value const&) = default;
    auto operator=(value const&) -> value&;
    auto operator==(value const&) -> bool;
  };

  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = value&;
  using value_type = value;
  using pointer = void;

  auto operator*() noexcept -> reference;

  auto operator++() noexcept -> throwing_assignment_iterator&;
  auto operator++(int) noexcept -> throwing_assignment_iterator;
  auto operator--() noexcept -> throwing_assignment_iterator&;
  auto operator--(int) noexcept -> throwing_assignment_iterator;
  auto operator==(throwing_assignment_iterator const&) noexcept -> bool;
  auto operator!=(throwing_assignment_iterator const&) noexcept -> bool;
};

using integral = int;

struct noexcept_int_iterator {
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = integral&;
  using value_type = integral;
  using pointer = void;

  auto operator*() noexcept -> reference;

  auto operator++() noexcept -> noexcept_int_iterator&;
  auto operator++(int) noexcept -> noexcept_int_iterator;
  auto operator--() noexcept -> noexcept_int_iterator&;
  auto operator--(int) noexcept -> noexcept_int_iterator;
  auto operator==(noexcept_int_iterator const&) noexcept -> bool;
  auto operator!=(noexcept_int_iterator const&) noexcept -> bool;
};

struct throwing_increment_int_iterator {
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = integral&;
  using value_type = integral;
  using pointer = void;

  auto operator*() noexcept -> reference;

  auto operator++() -> throwing_increment_int_iterator&;
  auto operator++(int) -> throwing_increment_int_iterator;
  auto operator--() noexcept -> throwing_increment_int_iterator&;
  auto operator--(int) noexcept -> throwing_increment_int_iterator;
  auto operator==(throwing_increment_int_iterator const&) noexcept -> bool;
  auto operator!=(throwing_increment_int_iterator const&) noexcept -> bool;
};

struct throwing_decrement_int_iterator {
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = integral&;
  using value_type = integral;
  using pointer = void;

  auto operator*() noexcept -> reference;

  auto operator++() noexcept -> throwing_decrement_int_iterator&;
  auto operator++(int) noexcept -> throwing_decrement_int_iterator;
  auto operator--() -> throwing_decrement_int_iterator&;
  auto operator--(int) -> throwing_decrement_int_iterator;
  auto operator==(throwing_decrement_int_iterator const&) noexcept -> bool;
  auto operator!=(throwing_decrement_int_iterator const&) noexcept -> bool;
};

struct throwing_comparison_int_iterator {
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = integral&;
  using value_type = integral;
  using pointer = void;

  auto operator*() noexcept -> reference;

  auto operator++() noexcept -> throwing_comparison_int_iterator&;
  auto operator++(int) noexcept -> throwing_comparison_int_iterator;
  auto operator--() noexcept -> throwing_comparison_int_iterator&;
  auto operator--(int) noexcept -> throwing_comparison_int_iterator;
  auto operator==(throwing_comparison_int_iterator const&) -> bool;
  auto operator!=(throwing_comparison_int_iterator const&) -> bool;
};

struct throwing_reference_int_iterator {
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = integral&;
  using value_type = integral;
  using pointer = void;

  auto operator*() -> reference;

  auto operator++() noexcept -> throwing_reference_int_iterator&;
  auto operator++(int) noexcept -> throwing_reference_int_iterator;
  auto operator--() noexcept -> throwing_reference_int_iterator&;
  auto operator--(int) noexcept -> throwing_reference_int_iterator;
  auto operator==(throwing_reference_int_iterator const&) noexcept -> bool;
  auto operator!=(throwing_reference_int_iterator const&) noexcept -> bool;
};

struct throwing_int_iterator {
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = integral&;
  using value_type = integral;
  using pointer = void;

  auto operator*() -> reference;

  auto operator++() -> throwing_int_iterator&;
  auto operator++(int) -> throwing_int_iterator;
  auto operator--() -> throwing_int_iterator&;
  auto operator--(int) -> throwing_int_iterator;
  auto operator==(throwing_int_iterator const&) -> bool;
  auto operator!=(throwing_int_iterator const&) -> bool;
};

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_TEST_ITERATORS_HPP_

// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/algorithm/iter_swap.hpp"

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "arene/base/array.hpp"
#include "arene/base/detail/wrapped_iterator.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

/// @test `iter_swap` two elements in a vector.
TEST(IterSwap, SwapVectorFrontAndBack) {
  auto values = std::vector<int>{1, 0};
  arene::base::iter_swap(values.begin(), values.end() - 1);
  ASSERT_EQ(values[0], 0);
  ASSERT_EQ(values[1], 1);
}

/// @test `iter_swap` a vector of four elements by two.
TEST(IterSwap, SwapIterator) {
  auto values = std::vector<int>{1, 2, 0, 0};
  arene::base::iter_swap(values.begin(), values.begin() + 2);
  ASSERT_EQ(values[0], 0);
  ASSERT_EQ(values[1], 2);
  ASSERT_EQ(values[2], 1);
  ASSERT_EQ(values[3], 0);
}

/// @test `iter_swap` move-only values in a vector.
TEST(IterSwap, SwapMoveOnlyValues) {
  auto origin = std::vector<int>{1, 2, 0, 0};
  auto values = std::vector<std::unique_ptr<int>>{};
  for (auto const& value : origin) {
    values.emplace_back(std::make_unique<int>(value));
  }
  arene::base::iter_swap(values.begin(), values.begin() + 2);
  ASSERT_EQ(*values[0], 0);
  ASSERT_EQ(*values[1], 2);
  ASSERT_EQ(*values[2], 1);
  ASSERT_EQ(*values[3], 0);
}

constexpr auto do_iter_swap(arene::base::array<std::uint32_t, 4> values) -> arene::base::array<std::uint32_t, 4> {
  auto values_copy = arene::base::array<std::uint32_t, 4>{values};
  arene::base::iter_swap(values_copy.begin(), values_copy.end() - 1);
  return values_copy;
}

/// @test `iter_swap` two elements in a constexpr context.
TEST(IterSwap, SwapIteratorsInConstexprContext) {
  static constexpr auto values = arene::base::array<std::uint32_t, 4>{1, 2, 3, 4};

  constexpr auto dst = do_iter_swap(values);

  static constexpr auto reference = arene::base::array<std::uint32_t, 4>{4, 2, 3, 1};
  STATIC_ASSERT_EQ(dst[0], reference[0]);
  STATIC_ASSERT_EQ(dst[1], reference[1]);
  STATIC_ASSERT_EQ(dst[2], reference[2]);
  STATIC_ASSERT_EQ(dst[3], reference[3]);
}

/*
 * Tests for noexcept conditions
 */

struct int_iterator {
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = int&;
  using value_type = int;
  using pointer = void;

  auto operator*() noexcept -> reference;

  auto operator++() noexcept -> int_iterator&;
  auto operator++(int) noexcept -> int_iterator;
  auto operator==(int_iterator const&) noexcept -> bool;
  auto operator!=(int_iterator const&) noexcept -> bool;
};

struct iterator_noexcept_value {
  // NOLINTBEGIN(hicpp-special-member-functions)
  struct value {
    auto operator=(value) noexcept -> value&;
  };
  // NOLINTEND(hicpp-special-member-functions)

  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = value&;
  using value_type = value;
  using pointer = void;

  auto operator*() noexcept -> reference;

  auto operator++() noexcept -> iterator_noexcept_value&;
  auto operator++(int) noexcept -> iterator_noexcept_value;
  auto operator==(iterator_noexcept_value const&) noexcept -> bool;
  auto operator!=(iterator_noexcept_value const&) noexcept -> bool;
};

struct iterator_throwing_value {
  // NOLINTBEGIN(hicpp-special-member-functions)
  struct value {
    auto operator=(value) -> value&;
  };
  // NOLINTEND(hicpp-special-member-functions)

  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = value&;
  using value_type = value;
  using pointer = void;

  auto operator*() noexcept -> reference;

  auto operator++() noexcept -> iterator_throwing_value&;
  auto operator++(int) noexcept -> iterator_throwing_value;
  auto operator==(iterator_throwing_value const&) noexcept -> bool;
  auto operator!=(iterator_throwing_value const&) noexcept -> bool;
};

/// @test Verify `iter_swap` is noexcept if the value type is a scalar.
TEST(IterSwap, IsNoexceptIfScalar) {
  STATIC_ASSERT_TRUE(noexcept(arene::base::iter_swap(int_iterator(), int_iterator())));
}

/// @test Verify `iter_swap` is noexcept if the operators of iterator argument are noexcept.
TEST(IterSwap, IsNoexceptIfNothrowAssignable) {
  STATIC_ASSERT_TRUE(noexcept(arene::base::iter_swap(iterator_noexcept_value(), iterator_noexcept_value())));
}

/// @test Verify `iter_swap` is not noexcept if the assignment operator of the iterator value type throws.
TEST(IterSwap, IsNotNoexceptIfAssignmentThrows) {
  STATIC_ASSERT_FALSE(noexcept(arene::base::iter_swap(iterator_throwing_value(), iterator_throwing_value())));
}

}  // namespace

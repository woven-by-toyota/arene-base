// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/detail/count.hpp"

#include <gtest/gtest.h>

#include "arene/base/algorithm/transform.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/stdlib_choice/begin.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/end.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/utility/safe_comparisons.hpp"

// NOLINTBEGIN(readability-identifier-length)

namespace {

namespace test {

template <std::size_t N>
constexpr auto assert_dynamic_extent_count_is(std::size_t expected_count, arene::base::array<std::size_t, N> values) {
  auto const count = arene::base::mdspan_detail::count(values.cbegin(), values.cend(), arene::base::dynamic_extent);
  CONSTEXPR_ASSERT(arene::base::cmp_equal(expected_count, count));
}

template <std::size_t N>
// NOLINTNEXTLINE(hicpp-avoid-c-arrays)
constexpr auto assert_dynamic_extent_count_is(std::size_t expected_count, int const (&arr)[N]) {
  // create an array for coverage of 'count' with the same iterator type
  auto values = arene::base::array<std::size_t, N>{};

  struct convert {
    constexpr auto operator()(int value) const { return static_cast<std::size_t>(value); }
  };
  std::ignore = arene::base::transform(std::begin(arr), std::end(arr), values.begin(), convert{});

  return assert_dynamic_extent_count_is(expected_count, values);
}

}  // namespace test

/// @test @c count returns the number of elements equal to a given value
CONSTEXPR_TEST(MdspanDetailCount, Count) {
  // we can't use arene::base::dynamic_extent due to GCC8 deduction issues
  constexpr auto _ = int{-1};

  test::assert_dynamic_extent_count_is(0U, arene::base::array<std::size_t, 0>{});

  test::assert_dynamic_extent_count_is(1U, {_});
  test::assert_dynamic_extent_count_is(2U, {_, _});
  test::assert_dynamic_extent_count_is(2U, {_, 0, _});
  test::assert_dynamic_extent_count_is(2U, {_, 0, _, 1});
  test::assert_dynamic_extent_count_is(0U, {1, 2, 3});
}

}  // namespace

// NOLINTEND(readability-identifier-length)

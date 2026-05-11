// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/compare/compare_three_way.hpp"
#include "arene/base/compare/strong_ordering.hpp"
#include "arene/base/detail/raw_c_string.hpp"
#include "arene/base/inline_container/set.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/stdexcept.hpp"

namespace {

// A capacity large enough for most tests
constexpr std::size_t standard_capacity = 20;

struct some_exception : std::runtime_error {
  using std::runtime_error::runtime_error;
};

// Throws if any of the arguments are bad_value
struct throwing_compare {
  static constexpr std::int32_t bad_value = 42;
  auto operator()(std::int32_t lhs, std::int32_t rhs) const -> arene::base::strong_ordering {
    if (lhs == bad_value || rhs == bad_value) {
      throw some_exception("throwing compare");
    }
    return arene::base::compare_three_way{}(lhs, rhs);
  }
};
constexpr std::int32_t throwing_compare::bad_value;

using throwing_compare_set = arene::base::inline_set<std::int32_t, standard_capacity, throwing_compare>;

/// @test `find` throws if the comparison throws.
TEST(InlineSet, FindThrowsIfComparisonThrows) {
  using set_t = throwing_compare_set;

  set_t const set{1, 2};

  ASSERT_THROW(set.find(throwing_compare::bad_value), some_exception);
  ASSERT_NO_THROW(set.find(100));
}

/// @test `contains` throws if the comparison throws.
TEST(InlineSet, ContainsThrowsIfComparisonThrows) {
  using set_t = throwing_compare_set;

  set_t const set{1, 2};

  ASSERT_THROW(set.contains(throwing_compare::bad_value), some_exception);
  ASSERT_NO_THROW(set.contains(5));
}

/// @test `count` throws if the comparison throws.
TEST(InlineSet, CountThrowsIfComparisonThrows) {
  using set_t = throwing_compare_set;

  set_t const set{1, 2};

  ASSERT_THROW(set.count(throwing_compare::bad_value), some_exception);
  ASSERT_NO_THROW(set.count(5));
}

/// @test `lower_bound`, `upper_bound` and `equal_range` throw if the comparison throws.
TEST(InlineSet, OtherQueriesThrowsIfComparisonThrows) {
  using set_t = throwing_compare_set;

  set_t const set{1, 2};

  ASSERT_THROW(set.lower_bound(throwing_compare::bad_value), some_exception);
  ASSERT_NO_THROW(set.lower_bound(5));
  ASSERT_THROW(set.upper_bound(throwing_compare::bad_value), some_exception);
  ASSERT_NO_THROW(set.upper_bound(5));
  ASSERT_THROW(set.equal_range(throwing_compare::bad_value), some_exception);
  ASSERT_NO_THROW(set.equal_range(5));
}

/// @test `erase` throws if it is invoked with a value and the comparison throws.
TEST(InlineSet, EraseElementByKeyThrowsIfCompareThrows) {
  using set_t = throwing_compare_set;

  std::int32_t const key1 = 4;
  std::int32_t const key2 = 1;
  std::int32_t const key3 = 2;
  std::int32_t const key4 = 23;
  std::int32_t const key5 = 11;
  set_t set{key1, key2, key3, key4, key5};

  ASSERT_THROW(set.erase(throwing_compare::bad_value), some_exception);
  ASSERT_NO_THROW(set.erase(11));
  static_assert(noexcept(set.erase(set.begin())), "Must be noexcept");
}

}  // namespace

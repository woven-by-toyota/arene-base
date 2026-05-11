// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/expect.hpp"

#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <type_traits>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"

namespace {
/// @test Ensure that `ARENE_EXPECT` is defined
TEST(Expect, ExpectIsDefined) {
#ifndef ARENE_EXPECT
  FAIL() << "Expected ARENE_EXPECT to be defined";
#endif
}

constexpr auto passthrough(std::int32_t value) noexcept -> std::int32_t { return value; }

template <std::size_t LhsSize, std::size_t RhsSize>
// NOLINTNEXTLINE(hicpp-avoid-c-arrays)
constexpr auto strings_are_equal(char const (&lhs)[LhsSize], char const (&rhs)[RhsSize]) noexcept -> bool {
  if (LhsSize != RhsSize) {
    return false;
  }
  for (std::size_t index{0}; index < LhsSize; ++index) {
    if (lhs[index] != rhs[index]) {
      return false;
    }
  }
  return true;
}

/// @test Ensure that `ARENE_EXPECT` has the same boolean value as the supplied expression
TEST(Expect, HasValueOfSuppliedExpression) {
  static constexpr std::initializer_list<std::int32_t>
      values{10, 20, 30, 1, 1233, 2004566999, -2004566999, -123, -4567, 0};

  for (auto value : values) {
    ASSERT_EQ(ARENE_EXPECT(value), value != 0);
    ASSERT_EQ(ARENE_EXPECT(passthrough(value)), value != 0);
  }

  static_assert(ARENE_EXPECT(-42), "Correct value");
  static_assert(ARENE_EXPECT(123), "Correct value");
  static_assert(!ARENE_EXPECT(0), "Correct value");
}

/// @test Ensure that `ARENE_EXPECT` works even if the expression contains commas
TEST(Expect, WorksWithExpressionsWihtCommas) { ASSERT_TRUE(ARENE_EXPECT(std::is_convertible<int, double>::value)); }

/// @test On QNX, `ARENE_EXPECT` just expands to a call to check the expression
TEST(Expect, ExpandsToArgOnQNX) {
#ifdef __QNX__
  static_assert(
      strings_are_equal(
          ARENE_STRINGIZE(ARENE_EXPECT(some(expression))),
          "(::arene::base::expect_detail::check((some(expression))) != 0)"
      ),
      "Must be equal"
  );
#else
  GTEST_SKIP() << "Not on QNX";
#endif
}

/// @test If `__builtin_expect` is available, `ARENE_EXPECT` expands to a call to `__builtin_expect` wrapping the call
/// to check the expression
TEST(Expect, ExpandsToBuiltinIfAvailable) {
#if ARENE_HAS_BUILTIN(__builtin_expect)
  ASSERT_THAT(
      ARENE_STRINGIZE(ARENE_EXPECT(some(expression))),
      ::testing::MatchesRegex("[[:space:]]*\\([[:space:]]*__builtin_expect\\([[:space:]]*::arene::base::expect_detail::"
                              "check\\([[:space:]]*\\([[:space:]]*"
                              "some\\(expression\\)"
                              "[[:space:]]*\\)[[:space:]]*\\)[[:space:]]*,[[:space:]]*1\\)[[:space:]]*!=[[:space:]]*0[["
                              ":space:]]*\\)[[:space:]]*")
  );
#else
  GTEST_SKIP() << "No builtin";
#endif
}
}  // namespace

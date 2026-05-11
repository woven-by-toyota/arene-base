// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "testlibs/minitest/minitest.hpp"

// Float comparisons are all checking identical values
#if defined(__clang__)
_Pragma("clang diagnostic ignored \"-Wfloat-equal\"") static_assert(true, "");
#elif defined(__GNUC__)
_Pragma("GCC diagnostic ignored \"-Wfloat-equal\"") static_assert(true, "");
#endif

namespace test {

using size_t = decltype(sizeof(int));

template <class T, size_t N>
struct array {
  static_assert(N != 0, "'N' must be non-zero.");

  T values[N]{};

  constexpr auto begin() -> T* { return values; }
  constexpr auto begin() const -> T const* { return values; }
  constexpr auto end() -> T* { return begin() + N; }
  constexpr auto end() const -> T const* { return begin() + N; }
};

MATCHER_P(Equals, n, "equal to n") { return arg == n; }
MATCHER_P2(EqualsEither, a, b, "equal to a or b") { return (arg == a) || (arg == b); }

}  // namespace test

namespace {

CONSTEXPR_TEST(CustomMatcherEquals, Passes) {
  TEST_MACRO(1, ::test::Equals(1));
  TEST_MACRO(1.0, ::test::Equals(1));
  TEST_MACRO(1.0, ::test::Equals(1.0));
  TEST_MACRO(1, ::test::Equals(1.0));
}

TEST(CustomMatcherEquals, Fails) { TEST_MACRO(1, ::test::Equals(2)); }

CONSTEXPR_TEST(CustomMatcherEqualsEither, Passes) {
  TEST_MACRO(1, ::test::EqualsEither(1, 2));
  TEST_MACRO(2, ::test::EqualsEither(1, 2));
}

TEST(CustomMatcherEqualsEither, Fails) { TEST_MACRO(3, ::test::EqualsEither(1, 2)); }

CONSTEXPR_TEST(EqMatcher, Passes) { TEST_MACRO(3, ::testing::Eq(3)); }

TEST(EqMatcher, Fails) { TEST_MACRO(4, ::testing::Eq(3)); }

CONSTEXPR_TEST(EachMatcher, Passes) {
  auto values = test::array<int, 3>{1, 1, 1};

  TEST_MACRO(values, ::testing::Each(1));
  TEST_MACRO(values, ::testing::Each(::testing::Eq(1)));
}

TEST(EachMatcher, Fails) {
  auto values = test::array<int, 3>{1, 2, 1};

  TEST_MACRO(values, ::testing::Each(1));
}

CONSTEXPR_TEST(ElementsAreArrayMatcher, Passes) {
  auto values = test::array<int, 3>{1, 2, 3};
  int arr[3] = {1, 2, 3};

  TEST_MACRO(values, ::testing::ElementsAreArray({1, 2, 3}));
  TEST_MACRO(values, ::testing::ElementsAreArray(values));
  TEST_MACRO(values, ::testing::ElementsAreArray(values.begin(), values.end()));
  TEST_MACRO(values, ::testing::ElementsAreArray(arr));
}

TEST(ElementsAreArrayMatcher, Fails) {
  auto values = test::array<int, 3>{1, 2, 4};

  TEST_MACRO(values, ::testing::ElementsAreArray({1, 2, 3}));
}

MATCHER_P(calls_function, function, "") { return function(arg); }

constexpr auto is_three(int arg) -> bool { return arg == 3; }

CONSTEXPR_TEST(CustomMatcherWithFunctionArg, Passes) { TEST_MACRO(3, calls_function(is_three)); }

TEST(CustomMatcherWithFunctionArg, Fails) { TEST_MACRO(4, calls_function(is_three)); }

}  // namespace

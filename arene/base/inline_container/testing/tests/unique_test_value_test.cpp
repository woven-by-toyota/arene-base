// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/inline_container/testing/unique_test_value.hpp"

#include <type_traits>
#include <utility>

#include <gtest/gtest.h>

#include "arene/base/array.hpp"
#include "arene/base/inline_container/testing/customization.hpp"
#include "arene/base/testing/gtest.hpp"

// constexpr case
template <>
constexpr auto ::arene::base::testing::test_value_array<int> = arene::base::to_array({1, 2});

namespace {
struct int_wrapper {
  int value;

  int_wrapper() noexcept
      : value{} {}

  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  int_wrapper(int arg) noexcept
      : value{arg} {}

  friend auto operator==(int_wrapper lhs, int_wrapper rhs) noexcept -> bool { return lhs.value == rhs.value; }
};

}  // namespace

// non constexpr case
// NOLINTNEXTLINE(fuchsia-statically-constructed-objects)
template <>
auto const ::arene::base::testing::test_value_array<int_wrapper> =
    arene::base::to_array({int_wrapper{1}, int_wrapper{2}});

namespace {

using ::arene::base::testing::test_value;
using ::arene::base::testing::unique_test_value;

/// @test unique_test_value returns the same value as test_value if test_value is constexpr invocable
CONSTEXPR_TEST(UniqueTestValue, ConstexprCase) {
  using T = int;

  CONSTEXPR_ASSERT(unique_test_value<T, 0>() == test_value<T>(0));
  CONSTEXPR_ASSERT(unique_test_value<T, 1>() == test_value<T>(1));

  CONSTEXPR_ASSERT(std::is_same<decltype(unique_test_value<T, 0>()), decltype(test_value<T>(0))>::value);
  CONSTEXPR_ASSERT(noexcept(unique_test_value<T, 0>()) == noexcept(test_value<T>(0)));
}

/// @test unique_test_value returns the same value as test_value if test_value is not constexpr invocable
TEST(UniqueTestValue, NonConstexprCase) {
  using T = int_wrapper;

  ASSERT_TRUE((unique_test_value<T, 0>() == test_value<T>(0)));
  ASSERT_TRUE((unique_test_value<T, 1>() == test_value<T>(1)));

  ASSERT_TRUE((std::is_same<decltype(unique_test_value<T, 0>()), decltype(test_value<T>(0))>::value));
  ASSERT_TRUE((noexcept(unique_test_value<T, 0>()) == noexcept(test_value<T>(0))));
}

}  // namespace

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/math/lcm.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

namespace {

/// @test lcm of positive integers
TEST(LCMTest, PositiveIntegers) {
  STATIC_ASSERT_EQ(::arene::base::lcm(6, 8), 24);
  STATIC_ASSERT_EQ(::arene::base::lcm(12U, 18U), 36U);
  STATIC_ASSERT_EQ(::arene::base::lcm(15L, 20L), 60L);
  STATIC_ASSERT_EQ(::arene::base::lcm(7LL, 11LL), 77LL);  // Coprime numbers
}

/// @test lcm with zero
TEST(LCMTest, WithZero) {
  STATIC_ASSERT_EQ(::arene::base::lcm(0, 5), 0);
  STATIC_ASSERT_EQ(::arene::base::lcm(5, 0), 0);
  STATIC_ASSERT_EQ(::arene::base::lcm(0, 0), 0);

  // Need to make sure this branch is covered for each template instantiation.
  STATIC_ASSERT_EQ(::arene::base::lcm(0L, 5L), 0);
  STATIC_ASSERT_EQ(::arene::base::lcm(5L, 0L), 0);

  STATIC_ASSERT_EQ(::arene::base::lcm(0LL, 5LL), 0);
  STATIC_ASSERT_EQ(::arene::base::lcm(5LL, 0LL), 0);

  STATIC_ASSERT_EQ(::arene::base::lcm(0U, 5U), 0);
  STATIC_ASSERT_EQ(::arene::base::lcm(5U, 0U), 0);
}

/// @test lcm with one
TEST(LCMTest, WithOne) {
  STATIC_ASSERT_EQ(::arene::base::lcm(1, 5), 5);
  STATIC_ASSERT_EQ(::arene::base::lcm(5, 1), 5);
  STATIC_ASSERT_EQ(::arene::base::lcm(1, 1), 1);
}

/// @test lcm with same numbers
TEST(LCMTest, SameNumbers) {
  STATIC_ASSERT_EQ(::arene::base::lcm(5, 5), 5);
  STATIC_ASSERT_EQ(::arene::base::lcm(12, 12), 12);
}

/// @test lcm with negative integers
TEST(LCMTest, NegativeIntegers) {
  STATIC_ASSERT_EQ(::arene::base::lcm(-6, 8), 24);
  STATIC_ASSERT_EQ(::arene::base::lcm(6, -8), 24);
  STATIC_ASSERT_EQ(::arene::base::lcm(-6, -8), 24);
}

/// @test lcm with large values
TEST(LCMTest, LargeValues) {
  STATIC_ASSERT_EQ(
      ::arene::base::lcm(std::numeric_limits<std::int64_t>::max(), std::int64_t{1}),
      std::numeric_limits<std::int64_t>::max()
  );
  STATIC_ASSERT_EQ(
      ::arene::base::lcm(std::numeric_limits<std::int64_t>::max(), std::numeric_limits<std::int64_t>::max()),
      std::numeric_limits<std::int64_t>::max()
  );
  STATIC_ASSERT_EQ(
      ::arene::base::lcm(-std::numeric_limits<std::int64_t>::max(), std::numeric_limits<std::int64_t>::max()),
      std::numeric_limits<std::int64_t>::max()
  );

  // lcm(max/2, 2) = (max/2) * 2 / gcd(max/2, 2) = (max/2) * 2 = max - 1
  STATIC_ASSERT_EQ(
      ::arene::base::lcm(std::numeric_limits<std::int64_t>::max() / 2, std::int64_t{2}),
      std::numeric_limits<std::int64_t>::max() - 1
  );
}

/// @test lcm function is noexcept
TEST(LCMTest, NoExcept) {
  static_assert(noexcept(::arene::base::lcm(5, 10)), "lcm should be noexcept");
  static_assert(noexcept(::arene::base::lcm(0U, 5U)), "lcm should be noexcept");
  static_assert(noexcept(::arene::base::lcm(-5LL, 10LL)), "lcm should be noexcept");
}

template <typename T>
class NonIntegralTests : public testing::Test {};

struct CustomType {
  int x;
};

// NOLINTNEXTLINE(hicpp-avoid-c-arrays)
using non_integral_types = ::testing::Types<float, double, long double, int*, void*, CustomType, int[5]>;

TYPED_TEST_SUITE(NonIntegralTests, non_integral_types, );

/// @test lcm should not be invocable with non-integral types
TYPED_TEST(NonIntegralTests, NotInvocableWithNonIntegralTypes) {
  auto test_lcm = [](auto lhs, auto rhs) -> decltype(::arene::base::lcm(lhs, rhs)) {
    return ::arene::base::lcm(lhs, rhs);
  };
  static_assert(
      !::arene::base::is_invocable_v<decltype(test_lcm), TypeParam, TypeParam>,
      "lcm should not work with non-integral types"
  );
}

}  // namespace

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/math/detail/abs_if_needed.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"

namespace {

using ::arene::base::math_detail::abs_if_needed;

template <class T>
class AbsIfNeededSignedDeathTest : public testing::Test {};

// NOLINTBEGIN(google-runtime-int)
using signed_integer_types = ::testing::Types<signed char, short, int, long, long long>;
// NOLINTEND(google-runtime-int)

TYPED_TEST_SUITE(AbsIfNeededSignedDeathTest, signed_integer_types, );

/// @test Calling @c abs_if_needed on the @c lowest value of a signed integer type crashes with a precondition violation
TYPED_TEST(AbsIfNeededSignedDeathTest, AbsOfMaximallyNegativeIntegerCrashes) {
  ASSERT_DEATH(abs_if_needed(std::numeric_limits<TypeParam>::lowest()), "Precondition");
}

}  // namespace

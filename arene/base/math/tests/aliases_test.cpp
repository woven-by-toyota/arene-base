// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/math/aliases.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

/// Type used to make tests more clearly indicate the correct overload is invoked.
enum class overload { f32_t, f64_t };

constexpr auto func(arene::base::f32_t) -> overload { return overload::f32_t; }
constexpr auto func(arene::base::f64_t) -> overload { return overload::f64_t; }

/// @test `f32_t` and `f64_t` behave identically to `float` and `double`, respectively.
TEST(Aliases, Basic) {
  STATIC_ASSERT_EQ(func(arene::base::f32_t{}), overload::f32_t);
  STATIC_ASSERT_EQ(func(float{}), overload::f32_t);
  STATIC_ASSERT_TRUE(std::is_same<float, arene::base::f32_t>::value);
  STATIC_ASSERT_EQ(func(arene::base::f64_t{}), overload::f64_t);
  STATIC_ASSERT_EQ(func(double{}), overload::f64_t);
  STATIC_ASSERT_TRUE(std::is_same<double, arene::base::f64_t>::value);
}

}  // namespace

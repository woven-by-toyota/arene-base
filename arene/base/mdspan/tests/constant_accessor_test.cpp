// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/constant_accessor.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/mdspan/is_accessor_policy.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

/// @test @c constant_accessor is a valid accessor
TEST(ConstantAccessor, IsAValidAccessor) {
  STATIC_ASSERT_TRUE(arene::base::is_accessor_policy_v<arene::base::constant_accessor<int>>);
  STATIC_ASSERT_TRUE(arene::base::is_accessor_policy_v<arene::base::constant_accessor<float>>);
  STATIC_ASSERT_TRUE(arene::base::is_accessor_policy_v<arene::base::constant_accessor<double>>);
}

/// @test Invoking @c access always returns the data handle
TEST(ConstantAccessor, AccessAlwaysReturnsDataHandle) {
  arene::base::constant_accessor<int> const int_accessor{};
  arene::base::constant_accessor<float> const float_accessor{};
  arene::base::constant_accessor<double> const double_accessor{};

  constexpr arene::base::array<std::size_t, 5> indices{{0, 10, 100, 4000000, 23}};
  constexpr arene::base::array<int, 5> int_values{{-99, 123, -42000, 100000, 3}};
  constexpr arene::base::array<float, 5> float_values{{-99.123F, 123.456F, -42.234E15F, 1.00000678E-10F, 3.141F}};
  constexpr arene::base::array<double, 5> double_values{
      {-99.123, 123.456, -42.234E27, 1.00000678E-36, 3.141823481479874982374923479}
  };

  for (auto& index : indices) {
    for (auto& value : int_values) {
      ASSERT_EQ(int_accessor.access(value, index), value);
    }
    for (auto& value : float_values) {
      ARENE_IGNORE_START();
      ARENE_IGNORE_ALL(
          "-Wfloat-equal",
          "These tests don't perform arithmetic, so equality is OK even for floating point"
      );
      ASSERT_EQ(float_accessor.access(value, index), value);
      ARENE_IGNORE_END();
    }
    for (auto& value : double_values) {
      ARENE_IGNORE_START();
      ARENE_IGNORE_ALL(
          "-Wfloat-equal",
          "These tests don't perform arithmetic, so equality is OK even for floating point"
      );
      ASSERT_EQ(double_accessor.access(value, index), value);
      ARENE_IGNORE_END();
    }
  }
}

/// @test Invoking @c offset always returns the data handle
TEST(ConstantAccessor, OffsetAlwaysReturnsDataHandle) {
  arene::base::constant_accessor<int> const int_accessor{};
  arene::base::constant_accessor<float> const float_accessor{};
  arene::base::constant_accessor<double> const double_accessor{};

  constexpr arene::base::array<std::size_t, 5> indices{{0, 10, 100, 4000000, 23}};
  constexpr arene::base::array<int, 5> int_values{{-99, 123, -42000, 100000, 3}};
  constexpr arene::base::array<float, 5> float_values{{-99.123F, 123.456F, -42.234E15F, 1.00000678E-10F, 3.141F}};
  constexpr arene::base::array<double, 5> double_values{
      {-99.123, 123.456, -42.234E27, 1.00000678E-36, 3.141823481479874982374923479}
  };

  for (auto& index : indices) {
    for (auto& value : int_values) {
      ASSERT_EQ(int_accessor.offset(value, index), value);
    }
    for (auto& value : float_values) {
      ARENE_IGNORE_START();
      ARENE_IGNORE_ALL(
          "-Wfloat-equal",
          "These tests don't perform arithmetic, so equality is OK even for floating point"
      );
      ASSERT_EQ(float_accessor.offset(value, index), value);
      ARENE_IGNORE_END();
    }
    for (auto& value : double_values) {
      ARENE_IGNORE_START();
      ARENE_IGNORE_ALL(
          "-Wfloat-equal",
          "These tests don't perform arithmetic, so equality is OK even for floating point"
      );
      ASSERT_EQ(double_accessor.offset(value, index), value);
      ARENE_IGNORE_END();
    }
  }
}

}  // namespace

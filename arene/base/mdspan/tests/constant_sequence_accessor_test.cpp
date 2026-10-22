// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/constant_sequence_accessor.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/mdspan/is_accessor_policy.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

template <typename Element, Element... Values>
struct custom_sequence {
  using value_type = Element;
  static constexpr auto size() noexcept -> std::size_t { return sizeof...(Values); }
};

template <typename Element>
struct custom_sequence_with_static_values {
  using value_type = Element;
  static constexpr auto values() noexcept -> arene::base::array<Element, 5> { return {10, 20, 30, 50, 100}; }
  static constexpr auto size() noexcept -> std::size_t { return values().size(); }
};

/// @test @c constant_sequence_accessor is a valid accessor
TEST(ConstantSequenceAccessor, IsAValidAccessor) {
  STATIC_ASSERT_TRUE(arene::base::is_accessor_policy_v<
                     arene::base::constant_sequence_accessor<std::integer_sequence<int, 1, 2, 3>, int>>);
  STATIC_ASSERT_TRUE(arene::base::is_accessor_policy_v<
                     arene::base::constant_sequence_accessor<custom_sequence<int, 1, 2, 3>, int>>);
  STATIC_ASSERT_TRUE(arene::base::is_accessor_policy_v<
                     arene::base::constant_sequence_accessor<custom_sequence_with_static_values<int>, int>>);
  STATIC_ASSERT_TRUE(arene::base::is_accessor_policy_v<
                     arene::base::constant_sequence_accessor<std::integer_sequence<int, -10, 20, 42>, int>>);
  STATIC_ASSERT_TRUE(arene::base::is_accessor_policy_v<
                     arene::base::constant_sequence_accessor<std::integer_sequence<int, 1, 2, 3>, float>>);
  STATIC_ASSERT_TRUE(arene::base::is_accessor_policy_v<
                     arene::base::constant_sequence_accessor<std::integer_sequence<int, 1, 2, 3>, double>>);
  STATIC_ASSERT_TRUE(arene::base::is_accessor_policy_v<
                     arene::base::constant_sequence_accessor<custom_sequence_with_static_values<double>, double>>);
}

/// @test Invoking @c access always returns the value from the sequence at the index given by the sum of the specified
/// base and index
TEST(ConstantSequenceAccessor, AccessAlwaysReturnsDataHandle) {
  constexpr int value1 = 123;
  constexpr int value2 = -99;
  constexpr int value3 = 123456;
  using basic_sequence = std::integer_sequence<int, value1, value2, value3>;
  arene::base::constant_sequence_accessor<basic_sequence, int> const int_accessor;
  arene::base::constant_sequence_accessor<basic_sequence, float> const float_accessor;
  arene::base::constant_sequence_accessor<basic_sequence, double> const double_accessor;
  arene::base::constant_sequence_accessor<custom_sequence<int, value1, value2, value3>> const custom_accessor;
  arene::base::constant_sequence_accessor<custom_sequence_with_static_values<unsigned>> const custom_array_accessor;

  ASSERT_EQ(int_accessor.access(0, 0), value1);
  ASSERT_EQ(int_accessor.access(0, 1), value2);
  ASSERT_EQ(int_accessor.access(0, 2), value3);
  ASSERT_EQ(int_accessor.access(1, 0), value2);
  ASSERT_EQ(int_accessor.access(1, 1), value3);
  ASSERT_EQ(int_accessor.access(1, std::size_t{} - 1U), value1);
  ASSERT_EQ(int_accessor.access(2, 0), value3);
  ASSERT_EQ(int_accessor.access(2, std::size_t{} - 2U), value1);
  ASSERT_EQ(int_accessor.access(2, std::size_t{} - 1U), value2);
  ASSERT_EQ(custom_accessor.access(0, 0), value1);
  ASSERT_EQ(custom_accessor.access(0, 1), value2);
  ASSERT_EQ(custom_accessor.access(0, 2), value3);
  ASSERT_EQ(custom_accessor.access(1, 0), value2);
  ASSERT_EQ(custom_accessor.access(1, 1), value3);
  ASSERT_EQ(custom_accessor.access(1, std::size_t{} - 1U), value1);
  ASSERT_EQ(custom_accessor.access(2, 0), value3);
  ASSERT_EQ(custom_accessor.access(2, std::size_t{} - 2U), value1);
  ASSERT_EQ(custom_accessor.access(2, std::size_t{} - 1U), value2);
  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "These tests don't perform arithmetic, so equality is OK even for floating point");
  ASSERT_EQ(float_accessor.access(0, 0), static_cast<float>(value1));
  ASSERT_EQ(float_accessor.access(0, 1), static_cast<float>(value2));
  ASSERT_EQ(float_accessor.access(0, 2), static_cast<float>(value3));
  ASSERT_EQ(float_accessor.access(1, 0), static_cast<float>(value2));
  ASSERT_EQ(float_accessor.access(1, 1), static_cast<float>(value3));
  ASSERT_EQ(float_accessor.access(1, std::size_t{} - 1U), static_cast<float>(value1));
  ASSERT_EQ(float_accessor.access(2, 0), static_cast<float>(value3));
  ASSERT_EQ(float_accessor.access(2, std::size_t{} - 2U), static_cast<float>(value1));
  ASSERT_EQ(float_accessor.access(2, std::size_t{} - 1U), static_cast<float>(value2));
  ASSERT_EQ(double_accessor.access(0, 0), static_cast<double>(value1));
  ASSERT_EQ(double_accessor.access(0, 1), static_cast<double>(value2));
  ASSERT_EQ(double_accessor.access(0, 2), static_cast<double>(value3));
  ASSERT_EQ(double_accessor.access(1, 0), static_cast<double>(value2));
  ASSERT_EQ(double_accessor.access(1, 1), static_cast<double>(value3));
  ASSERT_EQ(double_accessor.access(1, std::size_t{} - 1U), static_cast<double>(value1));
  ASSERT_EQ(double_accessor.access(2, 0), static_cast<double>(value3));
  ASSERT_EQ(double_accessor.access(2, std::size_t{} - 2U), static_cast<double>(value1));
  ASSERT_EQ(double_accessor.access(2, std::size_t{} - 1U), static_cast<double>(value2));
  ARENE_IGNORE_END();
  ASSERT_EQ(custom_array_accessor.access(0, 0), 10);
  ASSERT_EQ(custom_array_accessor.access(0, 1), 20);
  ASSERT_EQ(custom_array_accessor.access(0, 2), 30);
  ASSERT_EQ(custom_array_accessor.access(0, 3), 50);
  ASSERT_EQ(custom_array_accessor.access(0, 4), 100);
  ASSERT_EQ(custom_array_accessor.access(1, 0), 20);
  ASSERT_EQ(custom_array_accessor.access(1, 1), 30);
  ASSERT_EQ(custom_array_accessor.access(1, 2), 50);
  ASSERT_EQ(custom_array_accessor.access(1, 3), 100);
  ASSERT_EQ(custom_array_accessor.access(1, std::size_t{} - 1U), 10);
  ASSERT_EQ(custom_array_accessor.access(2, 0), 30);
  ASSERT_EQ(custom_array_accessor.access(2, 1), 50);
  ASSERT_EQ(custom_array_accessor.access(2, 2), 100);
  ASSERT_EQ(custom_array_accessor.access(2, std::size_t{} - 2U), 10);
  ASSERT_EQ(custom_array_accessor.access(2, std::size_t{} - 1U), 20);
}

/// @test Invoking @c offset always returns the sum of the supplied base and index
TEST(ConstantSequenceAccessor, AccessAlwaysReturnsDataHandleAdjustedByIndex) {
  constexpr int value1 = 123;
  constexpr int value2 = -99;
  constexpr int value3 = 123456;
  using basic_sequence = std::integer_sequence<int, value1, value2, value3>;
  arene::base::constant_sequence_accessor<basic_sequence, int> const int_accessor;
  arene::base::constant_sequence_accessor<basic_sequence, float> const float_accessor;
  arene::base::constant_sequence_accessor<basic_sequence, double> const double_accessor;
  arene::base::constant_sequence_accessor<custom_sequence<int, value1, value2, value3>> const custom_accessor;
  arene::base::constant_sequence_accessor<custom_sequence_with_static_values<unsigned>> const custom_array_accessor;

  ASSERT_EQ(int_accessor.offset(0, 0), 0);
  ASSERT_EQ(int_accessor.offset(0, 1), 1);
  ASSERT_EQ(int_accessor.offset(0, 2), 2);
  ASSERT_EQ(int_accessor.offset(0, 27), 27);
  ASSERT_EQ(int_accessor.offset(1, 0), 1);
  ASSERT_EQ(int_accessor.offset(1, 1), 2);
  ASSERT_EQ(int_accessor.offset(1, 27), 28);
  ASSERT_EQ(int_accessor.offset(2, 0), 2);
  ASSERT_EQ(int_accessor.offset(2, 27), 29);
  ASSERT_EQ(int_accessor.offset(27, 3), 30);
  ASSERT_EQ(int_accessor.offset(242, std::size_t{} - 200U), 42);
  ASSERT_EQ(custom_accessor.offset(0, 0), 0);
  ASSERT_EQ(custom_accessor.offset(0, 1), 1);
  ASSERT_EQ(custom_accessor.offset(0, 2), 2);
  ASSERT_EQ(custom_accessor.offset(0, 27), 27);
  ASSERT_EQ(custom_accessor.offset(1, 0), 1);
  ASSERT_EQ(custom_accessor.offset(1, 1), 2);
  ASSERT_EQ(custom_accessor.offset(1, 27), 28);
  ASSERT_EQ(custom_accessor.offset(2, 0), 2);
  ASSERT_EQ(custom_accessor.offset(2, 27), 29);
  ASSERT_EQ(custom_accessor.offset(27, 3), 30);
  ASSERT_EQ(custom_accessor.offset(242, std::size_t{} - 200U), 42);
  ASSERT_EQ(float_accessor.offset(0, 0), 0);
  ASSERT_EQ(float_accessor.offset(0, 1), 1);
  ASSERT_EQ(float_accessor.offset(0, 2), 2);
  ASSERT_EQ(float_accessor.offset(0, 27), 27);
  ASSERT_EQ(float_accessor.offset(1, 0), 1);
  ASSERT_EQ(float_accessor.offset(1, 1), 2);
  ASSERT_EQ(float_accessor.offset(1, 27), 28);
  ASSERT_EQ(float_accessor.offset(2, 0), 2);
  ASSERT_EQ(float_accessor.offset(2, 27), 29);
  ASSERT_EQ(float_accessor.offset(27, 3), 30);
  ASSERT_EQ(float_accessor.offset(242, std::size_t{} - 200U), 42);
  ASSERT_EQ(double_accessor.offset(0, 0), 0);
  ASSERT_EQ(double_accessor.offset(0, 1), 1);
  ASSERT_EQ(double_accessor.offset(0, 2), 2);
  ASSERT_EQ(double_accessor.offset(0, 27), 27);
  ASSERT_EQ(double_accessor.offset(1, 0), 1);
  ASSERT_EQ(double_accessor.offset(1, 1), 2);
  ASSERT_EQ(double_accessor.offset(1, 27), 28);
  ASSERT_EQ(double_accessor.offset(2, 0), 2);
  ASSERT_EQ(double_accessor.offset(2, 27), 29);
  ASSERT_EQ(double_accessor.offset(27, 3), 30);
  ASSERT_EQ(double_accessor.offset(242, std::size_t{} - 200U), 42);
  ASSERT_EQ(custom_array_accessor.offset(0, 0), 0);
  ASSERT_EQ(custom_array_accessor.offset(0, 1), 1);
  ASSERT_EQ(custom_array_accessor.offset(0, 2), 2);
  ASSERT_EQ(custom_array_accessor.offset(0, 27), 27);
  ASSERT_EQ(custom_array_accessor.offset(1, 0), 1);
  ASSERT_EQ(custom_array_accessor.offset(1, 1), 2);
  ASSERT_EQ(custom_array_accessor.offset(1, 27), 28);
  ASSERT_EQ(custom_array_accessor.offset(2, 0), 2);
  ASSERT_EQ(custom_array_accessor.offset(2, 27), 29);
  ASSERT_EQ(custom_array_accessor.offset(27, 3), 30);
  ASSERT_EQ(custom_array_accessor.offset(242, std::size_t{} - 200U), 42);
}

}  // namespace

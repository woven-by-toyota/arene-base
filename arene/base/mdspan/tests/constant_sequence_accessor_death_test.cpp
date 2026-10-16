// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/mdspan/constant_sequence_accessor.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"

namespace {

template <typename Element, Element... Values>
struct custom_sequence {
  using value_type = Element;
  static constexpr auto size() noexcept -> std::size_t { return sizeof...(Values); }
};

/// @test Invoking @c access with out of range values is precondition violation
TEST(ConstantSequenceAccessorDeathTest, OutOfRangeIndexIsPreconditionViolation) {
  constexpr int value1 = 123;
  constexpr int value2 = -99;
  constexpr int value3 = 123456;
  using basic_sequence = std::integer_sequence<int, value1, value2, value3>;
  arene::base::constant_sequence_accessor<basic_sequence, int> const int_accessor;
  arene::base::constant_sequence_accessor<basic_sequence, float> const float_accessor;
  arene::base::constant_sequence_accessor<basic_sequence, double> const double_accessor;
  arene::base::constant_sequence_accessor<custom_sequence<int, value1, value2, value3>> const custom_accessor;

  ASSERT_DEATH(int_accessor.access(0, 3), "Precondition");
  ASSERT_DEATH(int_accessor.access(0, 99), "Precondition");
  ASSERT_DEATH(float_accessor.access(0, 3), "Precondition");
  ASSERT_DEATH(float_accessor.access(0, 99), "Precondition");
  ASSERT_DEATH(double_accessor.access(0, 3), "Precondition");
  ASSERT_DEATH(double_accessor.access(0, 99), "Precondition");
  ASSERT_DEATH(custom_accessor.access(0, 3), "Precondition");
  ASSERT_DEATH(custom_accessor.access(0, 99), "Precondition");

  ASSERT_DEATH(int_accessor.access(3, 0), "Precondition");
  ASSERT_DEATH(int_accessor.access(99, 0), "Precondition");
  ASSERT_DEATH(float_accessor.access(3, 0), "Precondition");
  ASSERT_DEATH(float_accessor.access(99, 0), "Precondition");
  ASSERT_DEATH(double_accessor.access(3, 0), "Precondition");
  ASSERT_DEATH(double_accessor.access(99, 0), "Precondition");
  ASSERT_DEATH(custom_accessor.access(3, 0), "Precondition");
  ASSERT_DEATH(custom_accessor.access(99, 0), "Precondition");

  ASSERT_DEATH(int_accessor.access(1, 2), "Precondition");
  ASSERT_DEATH(float_accessor.access(1, 2), "Precondition");
  ASSERT_DEATH(double_accessor.access(1, 2), "Precondition");
  ASSERT_DEATH(custom_accessor.access(1, 2), "Precondition");
}

}  // namespace

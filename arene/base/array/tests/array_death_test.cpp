// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"

namespace {

struct user_type {};

/// @test Attempting to access any of the elements of a zero-sized array terminates the program
TEST(ArrayDeathTests, ZeroSizedArrayElementAccessIsAPreconditionViolation) {
  using arr_t = arene::base::array<user_type, 0>;
  arr_t array{};
  arr_t const& const_array = array;

  ASSERT_DEATH(array.front(), "Precondition");
  ASSERT_DEATH(array.back(), "Precondition");
  ASSERT_DEATH(array[0], "Precondition");
  ASSERT_DEATH(const_array.front(), "Precondition");
  ASSERT_DEATH(const_array.back(), "Precondition");
  ASSERT_DEATH(const_array[0], "Precondition");
}

}  // namespace

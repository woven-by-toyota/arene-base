// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/optional/optional.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/move.hpp"

namespace {

using ::arene::base::optional;

/// @test Accessing the contents of an empty optional using @c operator* crashes with a precondition violation
TEST(OptionalDeathTest, PreconditionViolationOnStarOperator) {
  optional<int> opt_int;
  ASSERT_DEATH(*opt_int, "Precondition");
  ASSERT_DEATH(*std::move(opt_int), "Precondition");

  optional<int> const const_opt_int;
  ASSERT_DEATH(*const_opt_int, "Precondition");
  ASSERT_DEATH(*std::move(const_opt_int), "Precondition");  // NOLINT(hicpp-move-const-arg) this is testing const move
}

/// @test Accessing the contents of an empty optional using @c operator-> crashes with a precondition violation
TEST(OptionalDeathTest, PreconditionViolationOnArrowOperator) {
  struct wrapped_int {
    int value{0};
  };

  optional<wrapped_int> opt_wint;
  ASSERT_DEATH(std::ignore = opt_wint->value, "Precondition");

  optional<wrapped_int> const const_opt_wint;
  ASSERT_DEATH(std::ignore = const_opt_wint->value, "Precondition");
}

}  // namespace

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file non_owning_ptr_death_test.cpp
/// @brief Unit tests for arene/base/pointer/non_owning_ptr.hpp
///

#include <gtest/gtest.h>

#include "arene/base/pointer.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"

namespace {

using ::arene::base::non_owning_ptr;

/// @test Deferencing a NULL `non_owning_ptr` terminates the program with a precondition violation
TEST(NonOwningPtrDeathTest, DereferenceOperatorOnNullptrIsPreconditionViolation) {
  non_owning_ptr<int> const null{nullptr};
  ASSERT_DEATH(*null = 10, "Precondition");
}

/// @test Using the arrow operator on a NULL `non_owning_ptr` terminates the program with a precondition violation
TEST(NonOwningPtrDeathTest, ArrowOperatorOnNullptrIsPreconditionViolation) {
  non_owning_ptr<std::tuple<int>> const null{nullptr};
  std::tuple<int> swap_with{10};
  ASSERT_DEATH(null->swap(swap_with), "Precondition");
}

}  // namespace

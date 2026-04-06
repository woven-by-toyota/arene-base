// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/strings/string_view.hpp"

using ::arene::base::string_view;

namespace {

/// @test Passing an index greater than or equal to the length of the string terminates the program with a precondition
/// violation
TEST(StringViewDeathTest, OutOfRangeIndexing) {
  string_view const str_view{"hello"};
  ASSERT_DEATH(str_view[str_view.size()], "pos < size()");
  ASSERT_DEATH(str_view[str_view.size() + 1], "pos < size()");
}

/// @test Invoking `front` when the `string_view` is empty terminates the program with a precondition violation
TEST(StringViewDeathTest, FrontFailsPreConditionOnEmptyStringView) {
  constexpr string_view str_view;
  ASSERT_DEATH(str_view.front(), "!empty()");
}

/// @test Invoking `back` when the `string_view` is empty terminates the program with a precondition violation
TEST(StringViewDeathTest, BackFailsPreConditionOnEmptyStringView) {
  constexpr string_view str_view;
  ASSERT_DEATH(str_view.back(), "!empty()");
}

}  // namespace

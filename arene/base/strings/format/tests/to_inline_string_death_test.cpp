// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/strings/format/to_inline_string.hpp"

namespace {

using ::arene::base::to_string_detail::digit;

/// @test `digit` requires a value between 0 and 9.
TEST(ToInlineStringDeathTest, DigitRequiresValueBetween0And9) {
  ASSERT_DEATH(digit(10), "Precondition");
  ASSERT_DEATH(digit(static_cast<std::size_t>(-1)), "Precondition");
}

}  // namespace

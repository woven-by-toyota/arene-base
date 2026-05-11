// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/strings/detail/string_length.hpp"

#include <gtest/gtest.h>

#include "arene/base/testing/gtest.hpp"

namespace {

using ::arene::base::detail::string_length;

/// @test The length of an empty string literal is zero
TEST(StringLength, ReturnsZeroForEmptyString) { STATIC_ASSERT_EQ(string_length(""), 0); }

/// @test The length of a string literal is the number of characters, not including the NUL terminator
TEST(StringLength, ReturnsSizeOfStringForNonEmptyStringExcludingNullTerminator) {
  STATIC_ASSERT_EQ(string_length("a"), 1);
  STATIC_ASSERT_EQ(string_length("abc"), 3);
  STATIC_ASSERT_EQ(string_length("A/aasi'onawenw'io屋enf010929232432"), 35);
}

/// @test The length of a string is the number of characters up to the first NUL character.
TEST(StringLength, NullTerminatorEndsSizeCount) { STATIC_ASSERT_EQ(string_length("a\0bc"), 1); }

/// @test Invoking `string_length` with a NULL pointer terminates the program with a precondition violation
TEST(StringLengthDeathTest, StrLengthAgainstNullptrIsPreconditionViolation) {
  EXPECT_DEATH(string_length(nullptr), "value != nullptr");
}

}  // namespace

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "testlibs/minitest/minitest.hpp"

TEST(Test, EqualStrings) {
  ASSERT_STREQ("hello", "hello");
  ASSERT_STRNE("hello", "hello");
}

TEST(Test, UnequalStringsFinalLetter) {
  ASSERT_STREQ("hello", "hellp");
  ASSERT_STRNE("hello", "hellp");
}

TEST(Test, UnequalStringsDifferentLengths) {
  ASSERT_STREQ("hello", "hello world");
  ASSERT_STRNE("hello", "hello world");
}

TEST(Test, UnequalStringsInitialLetter) {
  ASSERT_STREQ("hello", "jello");
  ASSERT_STRNE("hello", "jello");
}

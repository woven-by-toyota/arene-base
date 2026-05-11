// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "testlibs/minitest/minitest.hpp"

constexpr void foo(int) {}

TEST(Test, Assertions) {
  ASSERT_TRUE(true) << "x" << 3;
  ASSERT_FALSE(false) << 3 << "x";
  ASSERT_EQ(1, 1) << 0.;
  ASSERT_NE(1, 2) << "wibble";
  ASSERT_LT(1, 2) << 0. << "x" << 3;
  ASSERT_GT(2, 1) << "x" << 3;
  ASSERT_LE(1, 2) << 0. << "x" << 3;
  ASSERT_GE(2, 1) << "x" << 3;
  EXPECT_TRUE(true) << "x" << 3;
  EXPECT_FALSE(false) << 3 << "x";
  EXPECT_NO_FATAL_FAILURE(foo(23);) << 0.;
}

CONSTEXPR_TEST(Test, ConstexprTests) {
  ASSERT_TRUE(true) << "x" << 3;
  ASSERT_FALSE(false) << 3 << "x";
  ASSERT_EQ(1, 1) << 0.;
  ASSERT_NE(1, 2) << "wibble";
  ASSERT_LT(1, 2) << 0. << "x" << 3;
  ASSERT_GT(2, 1) << "x" << 3;
  ASSERT_LE(1, 2) << 0. << "x" << 3;
  ASSERT_GE(2, 1) << "x" << 3;
  EXPECT_TRUE(true) << "x" << 3;
  EXPECT_FALSE(false) << 3 << "x";
  EXPECT_EQ(1, 1) << 0.;
  EXPECT_NO_FATAL_FAILURE(foo(23);) << 0.;
}

TEST(Test, Failure) { ASSERT_EQ(1, 2) << "message"; }

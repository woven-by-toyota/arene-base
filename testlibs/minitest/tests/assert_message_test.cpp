// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "testlibs/minitest/minitest.hpp"

TEST(Messages, LessThan) { ASSERT_LT(1, 0); }

TEST(Messages, GreaterThan) { ASSERT_GT(1, 2); }

TEST(Messages, LessThanOrEqual) { ASSERT_LE(1, 0); }

TEST(Messages, GreaterThanOrEqual) { ASSERT_GE(1, 2); }

TEST(Messages, Equal) { ASSERT_EQ(1, 2); }

TEST(Messages, NotEqual) { ASSERT_NE(1, 1); }

TEST(Messages, False) { ASSERT_FALSE(true); }

TEST(Messages, True) { ASSERT_TRUE(false); }

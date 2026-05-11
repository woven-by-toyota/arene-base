// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "testlibs/minitest/minitest.hpp"

TEST(Test, TestOne) { ASSERT_TRUE(TEST_ONE_PASSES); }

TEST(Test, TestTwo) { ASSERT_TRUE(TEST_TWO_PASSES); }

TEST(Test, TestThree) { ASSERT_TRUE(TEST_THREE_PASSES); }

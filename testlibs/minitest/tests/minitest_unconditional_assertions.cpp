// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "testlibs/minitest/minitest.hpp"

TEST(Test, Succeed) { SUCCEED() << "should"; }

TEST(Test, Skip) { GTEST_SKIP() << "not"; }

TEST(Test, Fail) { FAIL() << "appear"; }

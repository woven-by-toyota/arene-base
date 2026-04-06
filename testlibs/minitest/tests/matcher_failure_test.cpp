// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "testlibs/minitest/minitest.hpp"

namespace {

TEST(CustomMatcherEquals, Fails) {
    TEST_MACRO(1, ::testing::Eq(2));
    TEST_MACRO(1, ::testing::Eq(3));
}

}  // namespace

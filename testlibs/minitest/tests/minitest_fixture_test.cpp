// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "testlibs/minitest/minitest.hpp"

class TestFixture : public testing::Test {
  protected:
   bool fixture_data = true;
};

TEST_F(TestFixture, CanAccessFixtureData) { ASSERT_TRUE(fixture_data); }

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/exception"
#include "testlibs/minitest/minitest.hpp"

namespace {

/// @test Calling terminate causes the process to be killed
TEST(TerminateDeathTest, TerminateKillsProcess) { ASSERT_DEATH(std::terminate(), ""); }

}  // namespace

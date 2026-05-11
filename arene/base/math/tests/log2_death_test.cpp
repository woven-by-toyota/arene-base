// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/math/log2.hpp"

namespace {
/// @test `log2` crashes with a precondition violation when given a zero operand.
TEST(Log2DeathTest, Log2Of0Crashes) { ASSERT_DEATH(arene::base::log2(0), "Precondition"); }
}  // namespace

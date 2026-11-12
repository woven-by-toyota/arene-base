// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>  // IWYU pragma: keep

#include "arene/base/testing/property_config.hpp"

namespace {

/// @test Values set on the test target's @c env propagate through to @c read_property_config on every platform,
/// including the Cortex-R52 bare-metal port where @c ::getenv is replaced at link time by @c
/// //toolchains/none_cortex-r52:fake_env.
TEST(ReadPropertyConfigEnv, ReadsStaticEnv) {
  auto const config = arene::base::testing::read_property_config();

  EXPECT_EQ(config.required_successes, 7U);
  EXPECT_EQ(config.max_discards, 3U);
  EXPECT_EQ(config.seed, 42U);
}

}  // namespace

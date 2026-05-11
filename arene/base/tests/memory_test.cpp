// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/memory.hpp"

#include <gtest/gtest.h>

#include "arene/base/type_traits.hpp"

namespace {

/// @test `construct_at` is declared
TEST(Memory, ConstructAt) {
  EXPECT_TRUE((arene::base::is_invocable_v<decltype(::arene::base::construct_at), int*, int>));
}

/// @test `destroy_at` is declared
TEST(Memory, DestroyAt) { EXPECT_TRUE((arene::base::is_invocable_v<decltype(::arene::base::destroy_at), int*>)); }

}  // namespace

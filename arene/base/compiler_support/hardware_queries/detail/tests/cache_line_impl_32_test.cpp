// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file cache_line_impl_32_test.cpp
/// @brief Unit tests for cache_line_impl_32.hpp
///
#include "arene/base/compiler_support/hardware_queries/detail/cache_line_impl_32.hpp"

#include <gtest/gtest.h>

#include "arene/base/testing/gtest.hpp"

namespace {
/// @test The cache line size is 32-bytes
TEST(CacheLineImpl32, L1CacheLineSizeIs32) {
  STATIC_ASSERT_EQ(arene::base::compiler_support::detail::l1_cache_line_size, 32U);
}
}  // namespace

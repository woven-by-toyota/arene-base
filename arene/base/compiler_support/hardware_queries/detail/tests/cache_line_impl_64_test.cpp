// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file cache_line_impl_64_test.cpp
/// @brief Unit tests for cache_line_impl_64.hpp
///
#include "arene/base/compiler_support/hardware_queries/detail/cache_line_impl_64.hpp"

#include <gtest/gtest.h>

#include "arene/base/testing/gtest.hpp"

namespace {
/// @test The cache line size is 64-bytes
TEST(CacheLineImpl64, L1CacheLineSizeIs64) {
  STATIC_ASSERT_EQ(arene::base::compiler_support::detail::l1_cache_line_size, 64U);
}
}  // namespace

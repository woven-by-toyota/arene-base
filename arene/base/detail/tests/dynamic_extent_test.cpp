// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/detail/dynamic_extent.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

using ::arene::base::dynamic_extent;

/// @test `dynamic_extent` is of type `std::size_t`
TEST(DynamicExtent, IsSizeT) { ::testing::StaticAssertTypeEq<decltype(dynamic_extent), std::size_t const>(); }

/// @test `dynamic_extent`'s value is `std::numeric_limits<std::size_t>::max()`
TEST(DynamicExtent, ValueIsMax) { STATIC_ASSERT_EQ(dynamic_extent, std::numeric_limits<std::size_t>::max()); }

}  // namespace

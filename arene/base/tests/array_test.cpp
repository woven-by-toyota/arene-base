// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/array.hpp"

#include <gtest/gtest.h>

namespace {

/// @test Ensure that `array` is declared
TEST(Array, ArrayIsExported) { using ::arene::base::array; }

/// @test Ensure that `to_array` is declared
TEST(Array, ToArrayIsExported) { using ::arene::base::to_array; }

}  // namespace

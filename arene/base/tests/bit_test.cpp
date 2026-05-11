// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file bit_test.cpp
/// @brief Provides unit tests to validate export of content in bit.hpp
///

#include "arene/base/bit.hpp"

#include <gtest/gtest.h>

namespace {

/// @test Ensure `bit_cast` is exported
TEST(Bit, BitCast) { using ::arene::base::bit_cast; }

}  // namespace

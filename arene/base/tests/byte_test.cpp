// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file byte_test.cpp
/// @brief Provides unit tests to validate export of content in byte.hpp
///

#include "arene/base/byte.hpp"

#include <gtest/gtest.h>

namespace {

/// @test Ensure `byte` is exported
TEST(Byte, Byte) { using arene::base::byte; }

/// @test Ensure `to_integer` is exported
TEST(Byte, ToInteger) { using arene::base::to_integer; }

}  // namespace

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file inline_string_test.cpp
/// @brief Validates that expected symbols are exported from inline_string.hpp
///

#include "arene/base/inline_string.hpp"

#include <gtest/gtest.h>

namespace {
/// @test Ensure that `inline_string` is declared
TEST(InlineString, InlineString) { using ::arene::base::inline_string; }
/// @test Ensure that `to_inline_string` is declared
TEST(InlineString, ToInlineString) { using ::arene::base::to_inline_string; }
/// @test Ensure that `make_inline_string` is declared
TEST(InlineString, MakeInlineString) { using ::arene::base::make_inline_string; }
/// @test Ensure that `is_inline_string` is declared
TEST(InlineString, IsInlineString) { using ::arene::base::is_inline_string; }
/// @test Ensure that `is_inline_string_v` is declared
TEST(InlineString, IsInlineStringV) { using ::arene::base::is_inline_string_v; }
/// @test Ensure that `is_inline_buffer_string` is declared
TEST(InlineString, IsInlineBufferString) { using ::arene::base::is_inline_buffer_string; }
/// @test Ensure that `is_inline_buffer_string_v` is declared
TEST(InlineString, IsInlineBufferStringV) { using ::arene::base::is_inline_buffer_string_v; }

}  // namespace

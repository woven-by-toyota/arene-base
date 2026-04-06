// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file string_view_test.cpp
/// @brief Validates that expected symbols are exported from string_view.hpp
///

#include "arene/base/string_view.hpp"

#include <gtest/gtest.h>

namespace {
/// @test `string_view` is declared
TEST(Strings, StringView) { using ::arene::base::string_view; }
/// @test `null_terminated_string_view` is declared
TEST(Strings, NullTerminatedStringView) { using ::arene::base::null_terminated_string_view; }

}  // namespace

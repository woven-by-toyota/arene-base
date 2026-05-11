// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/strings/algorithm/length.hpp"  // IWYU pragma: keep

#include <gtest/gtest.h>
// IWYU pragma: no_include "arene/base/strings/detail/string_length.hpp"

namespace {

/// @test Ensure `string_length` is declared
TEST(Length, StringLength) { using ::arene::base::string_length; }

}  // namespace

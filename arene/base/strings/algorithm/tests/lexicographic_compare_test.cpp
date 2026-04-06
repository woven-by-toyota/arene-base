// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/strings/algorithm/lexicographic_compare.hpp"  // IWYU pragma: keep

#include <gtest/gtest.h>
// IWYU pragma: no_include "arene/base/strings/detail/lexicographic_string_compare.hpp"

namespace {

/// @test Ensure `lexicographic_string_compare` is declared
TEST(lexicographicCompare, lexicographicStringCompare) { using ::arene::base::lexicographic_string_compare; }

}  // namespace

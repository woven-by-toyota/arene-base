// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file string_algorithms_test.cpp
/// @brief Validates that expected symbols are exported from string_algorithms.hpp
///

#include "arene/base/string_algorithms.hpp"

#include <gtest/gtest.h>

namespace {
/// @test `string_length` is declared
TEST(StringAlgorithms, Length) { using ::arene::base::string_length; }
/// @test `lexicographic_string_compare` is declared
TEST(StringAlgorithms, LexicographicalCompare) { using ::arene::base::lexicographic_string_compare; }
/// @test `trim`, `ltrim` and `rtrim` are declared
TEST(StringAlgorithms, Trim) {
  using ::arene::base::ltrim;
  using ::arene::base::rtrim;
  using ::arene::base::trim;
}

}  // namespace

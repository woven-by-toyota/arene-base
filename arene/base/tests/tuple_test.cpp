// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/tuple.hpp"

#include <gtest/gtest.h>

namespace {

/// @test `for_each` is declared
TEST(Tuple, ForEachIsExported) { using ::arene::base::for_each; }

/// @test `for_each_index` is declared
TEST(Tuple, ForEachIndexIsExported) { using ::arene::base::for_each_index; }

/// @test `apply` is declared
TEST(Tuple, ApplyIsExported) { using ::arene::base::apply; }

}  // namespace

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/detail/raw_c_string.hpp"

#include <gtest/gtest.h>

namespace {
using ::arene::base::detail::character;
using ::arene::base::detail::raw_c_string;

/// @test `character` is an alias for `char`
TEST(Character, IsAliasForChar) { ::testing::StaticAssertTypeEq<character, char>(); }

/// @test `raw_c_string` is an alias for `character const*`
TEST(RawCString, IsAliasForConstPtrToCharacter) { ::testing::StaticAssertTypeEq<raw_c_string, character const*>(); }
}  // namespace

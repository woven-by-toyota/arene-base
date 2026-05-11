// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_manipulation/non_constructible_dummy.hpp"

#include <type_traits>

#include <gtest/gtest.h>

#include "arene/base/testing/gtest.hpp"

namespace {

using ::arene::base::non_constructible_dummy;

/// @test `non_constructible_dummy` is not default-constructible
TEST(NonConstructibleDummy, IsNotDefaultConstructible) {
  STATIC_ASSERT_FALSE((std::is_constructible<non_constructible_dummy>::value));
}

/// @test `non_constructible_dummy` is not copy-constructible or copy-assignable
TEST(NonConstructibleDummy, IsNotCopyable) {
  STATIC_ASSERT_FALSE((std::is_copy_constructible<non_constructible_dummy>::value));
  STATIC_ASSERT_FALSE((std::is_copy_assignable<non_constructible_dummy>::value));
}

/// @test `non_constructible_dummy` is not move-constructible or move-assignable
TEST(NonConstructibleDummy, IsNotMoveable) {
  STATIC_ASSERT_FALSE((std::is_move_constructible<non_constructible_dummy>::value));
  STATIC_ASSERT_FALSE((std::is_move_assignable<non_constructible_dummy>::value));
}

}  // namespace

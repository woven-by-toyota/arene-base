// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_traits/all_are_same.hpp"

#include <gtest/gtest.h>

namespace {

using arene::base::all_are_same_v;

// Types used for testing
struct a;
struct b;

/// @test Check that `all_are_same_v` evaluates to `true` if and only if all the template parameters are the same type,
/// for up to 4 parameters
TEST(AllAreSame, AllAreSameForFewerThan5Parameters) {
  // 0 arg case
  EXPECT_TRUE((all_are_same_v<>));

  // 1 arg case
  EXPECT_TRUE((all_are_same_v<a>));

  // 2 arg cases
  EXPECT_TRUE((all_are_same_v<a, a>));
  EXPECT_FALSE((all_are_same_v<a, b>));

  // 3 arg cases
  EXPECT_TRUE((all_are_same_v<a, a, a>));
  EXPECT_FALSE((all_are_same_v<a, a, b>));
  EXPECT_FALSE((all_are_same_v<a, b, a>));
  EXPECT_FALSE((all_are_same_v<a, b, b>));

  // 4 arg cases
  EXPECT_TRUE((all_are_same_v<a, a, a, a>));
  EXPECT_FALSE((all_are_same_v<a, a, a, b>));
  EXPECT_FALSE((all_are_same_v<a, a, b, a>));
  EXPECT_FALSE((all_are_same_v<a, a, b, b>));
  EXPECT_FALSE((all_are_same_v<a, b, a, a>));
  EXPECT_FALSE((all_are_same_v<a, b, a, b>));
  EXPECT_FALSE((all_are_same_v<a, b, b, a>));
  EXPECT_FALSE((all_are_same_v<a, b, b, b>));
}

/// @test Check that `all_are_same_v` evaluates to `true` if and only if all the template parameters are the same type,
/// for up to 5 and 6 parameters
TEST(AllAreSame, AllAreSameFor5Or6Parameters) {
  // 5 arg cases
  EXPECT_TRUE((all_are_same_v<a, a, a, a, a>));
  EXPECT_FALSE((all_are_same_v<a, a, a, a, b>));
  EXPECT_FALSE((all_are_same_v<a, a, a, b, a>));
  EXPECT_FALSE((all_are_same_v<a, a, a, b, b>));
  EXPECT_FALSE((all_are_same_v<a, a, b, a, a>));
  EXPECT_FALSE((all_are_same_v<a, a, b, a, b>));
  EXPECT_FALSE((all_are_same_v<a, a, b, b, a>));
  EXPECT_FALSE((all_are_same_v<a, a, b, b, b>));
  EXPECT_FALSE((all_are_same_v<a, b, a, a, a>));
  EXPECT_FALSE((all_are_same_v<a, b, a, a, b>));
  EXPECT_FALSE((all_are_same_v<a, b, a, b, a>));
  EXPECT_FALSE((all_are_same_v<a, b, a, b, b>));
  EXPECT_FALSE((all_are_same_v<a, b, b, a, a>));
  EXPECT_FALSE((all_are_same_v<a, b, b, a, b>));
  EXPECT_FALSE((all_are_same_v<a, b, b, b, a>));
  EXPECT_FALSE((all_are_same_v<a, b, b, b, b>));

  // 6 arg cases
  EXPECT_TRUE((all_are_same_v<a, a, a, a, a, a>));
  EXPECT_FALSE((all_are_same_v<a, a, a, a, a, b>));
  EXPECT_FALSE((all_are_same_v<a, a, a, a, b, a>));
  EXPECT_FALSE((all_are_same_v<a, a, a, a, b, b>));
  EXPECT_FALSE((all_are_same_v<a, a, a, b, a, a>));
  EXPECT_FALSE((all_are_same_v<a, a, a, b, a, b>));
  EXPECT_FALSE((all_are_same_v<a, a, a, b, b, a>));
  EXPECT_FALSE((all_are_same_v<a, a, a, b, b, b>));
  EXPECT_FALSE((all_are_same_v<a, a, b, a, a, a>));
  EXPECT_FALSE((all_are_same_v<a, a, b, a, a, b>));
  EXPECT_FALSE((all_are_same_v<a, a, b, a, b, a>));
  EXPECT_FALSE((all_are_same_v<a, a, b, a, b, b>));
  EXPECT_FALSE((all_are_same_v<a, a, b, b, a, a>));
  EXPECT_FALSE((all_are_same_v<a, a, b, b, a, b>));
  EXPECT_FALSE((all_are_same_v<a, a, b, b, b, a>));
  EXPECT_FALSE((all_are_same_v<a, a, b, b, b, b>));
  EXPECT_FALSE((all_are_same_v<a, b, a, a, a, a>));
  EXPECT_FALSE((all_are_same_v<a, b, a, a, a, b>));
  EXPECT_FALSE((all_are_same_v<a, b, a, a, b, a>));
  EXPECT_FALSE((all_are_same_v<a, b, a, a, b, b>));
  EXPECT_FALSE((all_are_same_v<a, b, a, b, a, a>));
  EXPECT_FALSE((all_are_same_v<a, b, a, b, a, b>));
  EXPECT_FALSE((all_are_same_v<a, b, a, b, b, a>));
  EXPECT_FALSE((all_are_same_v<a, b, a, b, b, b>));
  EXPECT_FALSE((all_are_same_v<a, b, b, a, a, a>));
  EXPECT_FALSE((all_are_same_v<a, b, b, a, a, b>));
  EXPECT_FALSE((all_are_same_v<a, b, b, a, b, a>));
  EXPECT_FALSE((all_are_same_v<a, b, b, a, b, b>));
  EXPECT_FALSE((all_are_same_v<a, b, b, b, a, a>));
  EXPECT_FALSE((all_are_same_v<a, b, b, b, a, b>));
  EXPECT_FALSE((all_are_same_v<a, b, b, b, b, a>));
  EXPECT_FALSE((all_are_same_v<a, b, b, b, b, b>));
}

}  // namespace

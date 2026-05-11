// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_traits/is_one_of.hpp"

#include <gtest/gtest.h>

namespace {
using arene::base::is_one_of;
using arene::base::is_one_of_v;

/// @test `is_one_of_v` evaluates to `true` if and only if the first template parameter is identical to one or more of
/// the subsequent template parameters
TEST(IsOneOf, Correctness) {
  static_assert(!is_one_of_v<void>, "");
  static_assert(!is_one_of_v<void, int>, "");
  static_assert(!is_one_of_v<void, int, char, int>, "");

  static_assert(!is_one_of_v<int>, "");
  static_assert(!is_one_of_v<int, int const, char, bool>, "");
  static_assert(!is_one_of_v<int, int&, char, bool>, "");
  static_assert(!is_one_of_v<int, int&&, char, bool>, "");

  static_assert(is_one_of_v<int, int>, "");
  static_assert(is_one_of_v<int, int, char, bool>, "");
  static_assert(is_one_of_v<int, char, int, bool>, "");
  static_assert(is_one_of_v<int, char, bool, int>, "");

  static_assert(is_one_of<int, int>::value, "");
  static_assert(is_one_of<int, int, char>::value, "");
  static_assert(is_one_of<int, bool, int>::value, "");
  static_assert(is_one_of<int, void, bool, int, char>::value, "");
  static_assert(!is_one_of<int, char, unsigned int, bool, void>::value, "");
}
}  // namespace

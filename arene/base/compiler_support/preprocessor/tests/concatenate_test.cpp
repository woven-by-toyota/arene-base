// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file concatenate_test.cpp
/// @brief Unit tests for concatenate.hpp
///

#include <gtest/gtest.h>

#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

/// @test The result of applying `ARENE_CONCATENATE` to normal tokens is concatenating them like with ##
TEST(Concatenate, NormalTokensCanBeConcatenated) {
  constexpr bool value = true;
  constexpr bool value2 = ARENE_CONCATENATE(va, lue);
  STATIC_ASSERT_EQ(value, value2);
}

/// @test `ARENE_CONCATENATE` applied to `__LINE__` concatenates the line number rather than the token `__LINE__`
TEST(Concatenate, LineCanBeConcatenated) {
  constexpr auto value = ARENE_STRINGIZE(ARENE_CONCATENATE(testing_, __LINE__));
  STATIC_ASSERT_STREQ(value, "testing_26");
}

}  // namespace

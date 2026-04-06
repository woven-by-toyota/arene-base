// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/tests/cpp14_inline_test.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/tests/cpp14_inline_test_tu1.hpp"
#include "arene/base/compiler_support/tests/cpp14_inline_test_tu2.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

/// @test Given a function object which has been made inline via `ARENE_CPP14_INLINE_VARIABLE`, then the result is
/// equivalent to a static instance marked `inline` in C++17.
TEST(AreneCpp14InlineVariable, CreatesAnInvocableInstanceOfTheInputFunctionObject) {
  STATIC_ASSERT_EQ(cpp14_inline_test::example_add(1, 2), 3);
}

/// @test Given a function object which has been made inline via `ARENE_CPP14_INLINE_VARIABLE`, then it can be ODR-used,
/// which means it has a stable address across multiple translation units.
TEST(AreneCpp14InlineVariable, InstanceCanBeODRUsed) {
  EXPECT_EQ(cpp14_inline_test::tu1::example_add_addr(), cpp14_inline_test::tu2::example_add_addr());
  EXPECT_EQ(
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast) Need a reinterpret cast here, and is safe.
      reinterpret_cast<std::uintptr_t>(&cpp14_inline_test::example_add),
      cpp14_inline_test::tu2::example_add_addr()
  );
}

}  // namespace

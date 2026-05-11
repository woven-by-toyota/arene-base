// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_traits/is_invocable.hpp"
#include "stdlib/include/new"
#include "stdlib/include/type_traits"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"

namespace {

/// @test @c std::nothrow_t is an empty class type that is default constructible.
CONSTEXPR_TEST(NoThrowTTest, NoThrowTProperties) {
  ASSERT_TRUE(std::is_empty_v<std::nothrow_t>);
  ASSERT_TRUE(std::is_default_constructible_v<std::nothrow_t>);
}

/// @test @c std::nothrow is an object of type @c std::nothrow_t.
CONSTEXPR_TEST(NoThrowTTest, NoThrowIsCorrectType) {
  ::testing::StaticAssertTypeEq<decltype(std::nothrow), std::nothrow_t const>();
}

}  // namespace

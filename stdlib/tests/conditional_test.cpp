// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"

namespace {

struct udt1 {};
struct udt2 {};

/// @test @c conditional<true,T1,T2>::type is an alias for the first type
TEST(Conditional, IfConditionIsTrueTypeIsFirst) {
  ::testing::StaticAssertTypeEq<std::conditional<true, udt1, udt2>::type, udt1>();
}

/// @test @c conditional<false,T1,T2>::type is an alias for the second type
TEST(Conditional, IfConditionIsFalseTypeIsSecond) {
  ::testing::StaticAssertTypeEq<std::conditional<false, udt1, udt2>::type, udt2>();
}

/// @test @c conditional_t<true,T1,T2> is an alias for the first type
TEST(ConditionalT, IfConditionIsTrueTypeIsFirst) {
  ::testing::StaticAssertTypeEq<std::conditional_t<true, udt1, udt2>, udt1>();
}

/// @test @c conditional_t<false,T1,T2> is an alias for the second type
TEST(ConditionalT, IfConditionIsFalesTypeIsSecond) {
  ::testing::StaticAssertTypeEq<std::conditional_t<false, udt1, udt2>, udt2>();
}
}  // namespace

// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_manipulation/static_if.hpp"

#include <type_traits>

#include <gtest/gtest.h>

namespace {

using arene::base::static_if;

template <class T, class U>
constexpr bool is_same_v = std::is_same<T, U>::value;

// Template used when testing the true branches
template <class...>
struct on_true;

// Template used when testing the false branches
template <class...>
struct on_false;

// Types used for testing
struct a;
struct b;
struct c;

/// @test Test `static_if<Cond>::then_else` is an alias for the "then" type if the condition is true, and for the "else"
/// type otherwise.
TEST(StaticIfTest, ThenElse) {
  // With true condition
  EXPECT_TRUE((is_same_v<static_if<true>::then_else<on_true<>, on_false<>>, on_true<>>));

  // With false condition
  EXPECT_TRUE((is_same_v<static_if<false>::then_else<on_true<>, on_false<>>, on_false<>>));
}

/// @test Test `static_if<Cond>::then_apply_else` is an alias for the "then" template applied with the argument list if
/// the condition is true, and for the "else" type otherwise.
TEST(StaticIfTest, ThenApplyElse) {
  // With true condition
  EXPECT_TRUE((is_same_v<static_if<true>::then_apply_else<on_true, on_false<>, a, b, c>, on_true<a, b, c>>));

  // With false condition
  EXPECT_TRUE((is_same_v<static_if<false>::then_apply_else<on_true, on_false<>, a, b, c>, on_false<>>));
}

/// @test Test `static_if<Cond>::then_else_apply` is an alias for the "then" type if the condition is true, and for the
/// "else" template applied with the argument list otherwise
TEST(StaticIfTest, ThenElseApply) {
  // With true condition
  EXPECT_TRUE((is_same_v<static_if<true>::then_else_apply<on_true<>, on_false, a, b, c>, on_true<>>));

  // With false condition
  EXPECT_TRUE((is_same_v<static_if<false>::then_else_apply<on_true<>, on_false, a, b, c>, on_false<a, b, c>>));
}

/// @test Test `static_if<Cond>::then_apply_else_apply` is an alias for the "then" template applied with the argument
/// list if the condition is true, and for the "else" template applied with the argument list otherwise
TEST(StaticIfTest, ThenApplyElseApply) {
  // With true condition
  EXPECT_TRUE((is_same_v<static_if<true>::then_apply_else_apply<on_true, on_false, a, b, c>, on_true<a, b, c>>));

  // With false condition
  EXPECT_TRUE((is_same_v<static_if<false>::then_apply_else_apply<on_true, on_false, a, b, c>, on_false<a, b, c>>));
}

}  // namespace

// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_traits/arithmetic_traits.hpp"

#include <string>

#include <gtest/gtest.h>

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

ARENE_IGNORE_START();
ARENE_IGNORE_CLANG(
    "-Wunneeded-internal-declaration",
    "Trait detection on declaration is possible, but triggers this warning. You cannot ARENE_MAYBE_UNUSED "
    "a declaration. And we cannot provide a definition without getting uncovered lines as a definition "
    "would never be invoked. Therefore we have to suppress this warning via diagnostics."
);

struct non_addable {};

struct addable {
  auto operator+(addable const&) noexcept -> addable;
  auto operator+(int) const noexcept -> addable;

  friend auto operator+(std::string const&, addable const&) noexcept -> addable;
};

struct throwing_addable {
  auto operator+(throwing_addable const&) -> bool;
  auto operator+(int) const -> bool;

  friend auto operator+(std::string const&, throwing_addable const&) -> bool;
};

struct non_subtractable {};

struct subtractable {
  auto operator-(subtractable const&) noexcept -> subtractable;
  auto operator-(int) const noexcept -> subtractable;

  friend auto operator-(std::string const&, subtractable const&) noexcept -> subtractable;
};

struct throwing_subtractable {
  auto operator-(throwing_subtractable const&) -> bool;
  auto operator-(int) const -> bool;

  friend auto operator-(std::string const&, throwing_subtractable const&) -> bool;
};

ARENE_IGNORE_END();

/// @test `is_addable_v<T,U>` evaluates to `false` when the operation `T + U` is not well formed.
TEST(IsAddable, FalseForTypesWithoutAddOperatorBetweenTypes) {
  STATIC_ASSERT_FALSE(arene::base::is_addable_v<non_addable>);
  STATIC_ASSERT_FALSE(arene::base::is_addable_v<addable, non_addable>);
  STATIC_ASSERT_FALSE(arene::base::is_addable_v<non_addable, addable>);
  STATIC_ASSERT_FALSE(arene::base::is_addable_v<int, addable>);
  STATIC_ASSERT_FALSE(arene::base::is_addable_v<non_addable, throwing_addable>);
  STATIC_ASSERT_FALSE(arene::base::is_addable_v<addable, throwing_addable>);
  STATIC_ASSERT_FALSE(arene::base::is_addable_v<addable const, addable>);
  STATIC_ASSERT_FALSE(arene::base::is_addable_v<int, std::string>);
}

/// @test `is_addable_v<T,U>` evaluates to `true` when the operation `T + U` is well formed.
TEST(IsAddable, TrueForTypesWithAddOperatorBetweenTypes) {
  STATIC_ASSERT_TRUE(arene::base::is_addable_v<addable>);
  STATIC_ASSERT_TRUE(arene::base::is_addable_v<addable, addable const>);
  STATIC_ASSERT_TRUE(arene::base::is_addable_v<addable, int>);
  STATIC_ASSERT_TRUE(arene::base::is_addable_v<addable const, int>);
  STATIC_ASSERT_TRUE(arene::base::is_addable_v<std::string, addable>);
  STATIC_ASSERT_TRUE(arene::base::is_addable_v<throwing_addable>);
  STATIC_ASSERT_TRUE(arene::base::is_addable_v<throwing_addable const, int>);
  STATIC_ASSERT_TRUE(arene::base::is_addable_v<std::string, throwing_addable>);
  STATIC_ASSERT_TRUE(arene::base::is_addable_v<int>);
  STATIC_ASSERT_TRUE(arene::base::is_addable_v<int, double>);
  STATIC_ASSERT_TRUE(arene::base::is_addable_v<std::string>);
}

/// @test `is_nothrow_addable_v<T,U>` evaluates to `false` when the operation `T + U` is not well formed.
TEST(IsNothrowAddable, FalseForTypesWithoutAddOperatorBetweenTypes) {
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_addable_v<non_addable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_addable_v<addable, non_addable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_addable_v<non_addable, addable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_addable_v<int, addable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_addable_v<non_addable, throwing_addable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_addable_v<addable, throwing_addable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_addable_v<addable const, addable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_addable_v<int, std::string>);
}

/// @test `is_nothrow_addable_v<T,U>` evaluates to `false` when the operation `T + U` is well formed but
/// `noexcept(T + U)` returns `false`.
TEST(IsNothrowAddable, FalseForTypesWithAddOperatorBetweenTypesThatThrow) {
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_addable_v<throwing_addable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_addable_v<throwing_addable const, int>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_addable_v<std::string, throwing_addable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_addable_v<std::string>);
}

/// @test `is_nothrow_addable_v<T,U>` evaluates to `true` when the operation `T + U` is well formed and
/// `noexcept(T + U)` returns `true`.
TEST(IsNothrowAddable, TrueForTypesWithAddOperatorBetweenTypesThatDoNotThrow) {
  STATIC_ASSERT_TRUE(arene::base::is_nothrow_addable_v<addable>);
  STATIC_ASSERT_TRUE(arene::base::is_nothrow_addable_v<addable, addable const>);
  STATIC_ASSERT_TRUE(arene::base::is_nothrow_addable_v<addable, int>);
  STATIC_ASSERT_TRUE(arene::base::is_nothrow_addable_v<addable const, int>);
  STATIC_ASSERT_TRUE(arene::base::is_nothrow_addable_v<std::string, addable>);
  STATIC_ASSERT_TRUE(arene::base::is_nothrow_addable_v<int>);
  STATIC_ASSERT_TRUE(arene::base::is_nothrow_addable_v<int, double>);
}

/// @test `is_subtractable_v<T,U>` evaluates to `false` when the operation `T - U` is not well formed.
TEST(IsSubtractable, FalseForTypesWithoutSubtractionOperatorBetweenTypes) {
  STATIC_ASSERT_FALSE(arene::base::is_subtractable_v<non_subtractable>);
  STATIC_ASSERT_FALSE(arene::base::is_subtractable_v<subtractable, non_subtractable>);
  STATIC_ASSERT_FALSE(arene::base::is_subtractable_v<non_subtractable, subtractable>);
  STATIC_ASSERT_FALSE(arene::base::is_subtractable_v<int, subtractable>);
  STATIC_ASSERT_FALSE(arene::base::is_subtractable_v<non_subtractable, throwing_subtractable>);
  STATIC_ASSERT_FALSE(arene::base::is_subtractable_v<subtractable, throwing_subtractable>);
  STATIC_ASSERT_FALSE(arene::base::is_subtractable_v<subtractable const, subtractable>);
  STATIC_ASSERT_FALSE(arene::base::is_subtractable_v<int, std::string>);
  STATIC_ASSERT_FALSE(arene::base::is_subtractable_v<std::string>);
}

/// @test `is_subtractable_v<T,U>` evaluates to `true` when the operation `T - U` is well formed.
TEST(IsSubtractable, TrueForTypesWithSubtractionOperatorBetweenTypes) {
  STATIC_ASSERT_TRUE(arene::base::is_subtractable_v<subtractable>);
  STATIC_ASSERT_TRUE(arene::base::is_subtractable_v<subtractable, subtractable const>);
  STATIC_ASSERT_TRUE(arene::base::is_subtractable_v<subtractable, int>);
  STATIC_ASSERT_TRUE(arene::base::is_subtractable_v<subtractable const, int>);
  STATIC_ASSERT_TRUE(arene::base::is_subtractable_v<std::string, subtractable>);
  STATIC_ASSERT_TRUE(arene::base::is_subtractable_v<throwing_subtractable>);
  STATIC_ASSERT_TRUE(arene::base::is_subtractable_v<throwing_subtractable const, int>);
  STATIC_ASSERT_TRUE(arene::base::is_subtractable_v<std::string, throwing_subtractable>);
  STATIC_ASSERT_TRUE(arene::base::is_subtractable_v<int>);
  STATIC_ASSERT_TRUE(arene::base::is_subtractable_v<int, double>);
}

/// @test `is_nothrow_subtractable_v<T,U>` evaluates to `false` when the operation `T - U` is not well formed.
TEST(IsNothrowSubtractable, FalseForTypesWithoutSubtractionOperatorBetweenTypes) {
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_subtractable_v<non_subtractable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_subtractable_v<subtractable, non_subtractable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_subtractable_v<non_subtractable, subtractable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_subtractable_v<int, subtractable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_subtractable_v<non_subtractable, throwing_subtractable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_subtractable_v<subtractable, throwing_subtractable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_subtractable_v<subtractable const, subtractable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_subtractable_v<int, std::string>);
}

/// @test `is_nothrow_subtractable_v<T,U>` evaluates to `false` when the operation `T - U` is well formed but
/// `noexcept(T - U)` returns `false`.
TEST(IsNothrowSubtractable, FalseForTypesWithSubtractionOperatorBetweenTypesThatThrow) {
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_subtractable_v<throwing_subtractable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_subtractable_v<throwing_subtractable const, int>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_subtractable_v<std::string, throwing_subtractable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_subtractable_v<std::string>);
}

/// @test `is_nothrow_subtractable_v<T,U>` evaluates to `true` when the operation `T - U` is well formed and
/// `noexcept(T - U)` returns `true`.
TEST(IsNothrowSubtractable, TrueForTypesWithSubtractionOperatorBetweenTypesThatDoNotThrow) {
  STATIC_ASSERT_TRUE(arene::base::is_nothrow_subtractable_v<subtractable>);
  STATIC_ASSERT_TRUE(arene::base::is_nothrow_subtractable_v<subtractable, subtractable const>);
  STATIC_ASSERT_TRUE(arene::base::is_nothrow_subtractable_v<subtractable, int>);
  STATIC_ASSERT_TRUE(arene::base::is_nothrow_subtractable_v<subtractable const, int>);
  STATIC_ASSERT_TRUE(arene::base::is_nothrow_subtractable_v<std::string, subtractable>);
  STATIC_ASSERT_TRUE(arene::base::is_nothrow_subtractable_v<int>);
  STATIC_ASSERT_TRUE(arene::base::is_nothrow_subtractable_v<int, double>);
}

}  // namespace

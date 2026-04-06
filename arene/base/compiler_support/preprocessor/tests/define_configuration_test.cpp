// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

/// @test `a ARENE_ON b` evaluates to `true` if either `a` or `b` is `true`
TEST(AreneOn, IsOr) {
  STATIC_ASSERT_TRUE((true ARENE_ON true));
  STATIC_ASSERT_TRUE((false ARENE_ON true));
  STATIC_ASSERT_TRUE((true ARENE_ON false));
  STATIC_ASSERT_FALSE((false ARENE_ON false));
}

/// @test `a ARENE_OFF b` evaluates to `true` if one of `a` or `b` is `true`
TEST(AreneOff, IsXOr) {
  STATIC_ASSERT_FALSE((true ARENE_OFF true));
  STATIC_ASSERT_TRUE((false ARENE_OFF true));
  STATIC_ASSERT_TRUE((true ARENE_OFF false));
  STATIC_ASSERT_FALSE((false ARENE_OFF false));
}

/// @test `a ARENE_ON_BY_DEFAULT b` evaluates the same as `a+b`
TEST(AreneOnByDefault, IsPlus) { STATIC_ASSERT_EQ((3 ARENE_ON_BY_DEFAULT 3), 6); }
/// @test `a ARENE_OFF_BY_DEFAULT b` evaluates the same as `a-b`
TEST(AreneOffByDefault, IsMinus) { STATIC_ASSERT_EQ((3 ARENE_OFF_BY_DEFAULT 3), 0); }

/// @test `ARENE_IS_ON(arg)` yields an expression that evaluates to `true` if the corresponding `arg_I_` symbol is
/// defined to `ARENE_ON`
TEST(AreneIsOn, TrueForISymbolDefinedAsAreneOn) {
#define SOME_CONFIG_VAR_TEST_I_ ARENE_ON
  STATIC_ASSERT_TRUE(ARENE_IS_ON(SOME_CONFIG_VAR_TEST));
#undef SOME_CONFIG_VAR_TEST_I_
}

/// @test `ARENE_IS_ON(arg)` yields an expression that evaluates to `true` if the corresponding `arg_I_` symbol is
/// defined to `ARENE_ON_BY_DEFAULT`
TEST(AreneIsOn, TrueForISymbolDefinedAsAreneOnByDefault) {
#define SOME_CONFIG_VAR_TEST_I_ ARENE_ON_BY_DEFAULT
  STATIC_ASSERT_TRUE(ARENE_IS_ON(SOME_CONFIG_VAR_TEST));
#undef SOME_CONFIG_VAR_TEST_I_
}

/// @test `ARENE_IS_ON(arg)` yields an expression that evaluates to `false` if the corresponding `arg_I_` symbol is
/// defined to `ARENE_OFF`
TEST(AreneIsOn, FalseForISymbolDefinedAsAreneOff) {
#define SOME_CONFIG_VAR_TEST_I_ ARENE_OFF
  STATIC_ASSERT_FALSE(ARENE_IS_ON(SOME_CONFIG_VAR_TEST));
#undef SOME_CONFIG_VAR_TEST_I_
}

/// @test `ARENE_IS_ON(arg)` yields an expression that evaluates to `false` if the corresponding `arg_I_` symbol is
/// defined to `ARENE_OFF_BY_DEFAULT`
TEST(AreneIsOn, FalseForISymbolDefinedAsAreneOffByDefault) {
#define SOME_CONFIG_VAR_TEST_I_ ARENE_OFF_BY_DEFAULT
  STATIC_ASSERT_FALSE(ARENE_IS_ON(SOME_CONFIG_VAR_TEST));
#undef SOME_CONFIG_VAR_TEST_I_
}

/// @test `ARENE_IS_OFF(arg)` yields an expression that evaluates to `false` if the corresponding `arg_I_` symbol is
/// defined to `ARENE_ON`
TEST(AreneIsOff, FalseForISymbolDefinedAsAreneOn) {
#define SOME_CONFIG_VAR_TEST_I_ ARENE_ON
  STATIC_ASSERT_FALSE(ARENE_IS_OFF(SOME_CONFIG_VAR_TEST));
#undef SOME_CONFIG_VAR_TEST_I_
}

/// @test `ARENE_IS_OFF(arg)` yields an expression that evaluates to `false` if the corresponding `arg_I_` symbol is
/// defined to `ARENE_ON_BY_DEFAULT`
TEST(AreneIsOff, FalseForISymbolDefinedAsAreneOnByDefault) {
#define SOME_CONFIG_VAR_TEST_I_ ARENE_ON_BY_DEFAULT
  STATIC_ASSERT_FALSE(ARENE_IS_OFF(SOME_CONFIG_VAR_TEST));
#undef SOME_CONFIG_VAR_TEST_I_
}

/// @test `ARENE_IS_OFF(arg)` yields an expression that evaluates to `true` if the corresponding `arg_I_` symbol is
/// defined to `ARENE_OFF`
TEST(AreneIsOff, TrueForISymbolDefinedAsAreneOff) {
#define SOME_CONFIG_VAR_TEST_I_ ARENE_OFF
  STATIC_ASSERT_TRUE(ARENE_IS_OFF(SOME_CONFIG_VAR_TEST));
#undef SOME_CONFIG_VAR_TEST_I_
}

/// @test `ARENE_IS_OFF(arg)` yields an expression that evaluates to `true` if the corresponding `arg_I_` symbol is
/// defined to `ARENE_OFF_BY_DEFAULT`
TEST(AreneIsOff, TrueForISymbolDefinedAsAreneOffByDefault) {
#define SOME_CONFIG_VAR_TEST_I_ ARENE_OFF_BY_DEFAULT
  STATIC_ASSERT_TRUE(ARENE_IS_OFF(SOME_CONFIG_VAR_TEST));
#undef SOME_CONFIG_VAR_TEST_I_
}

/// @test `ARENE_IS_ON_BY_DEFAULT(arg)` yields an expression that evaluates to `false` if the corresponding `arg_I_`
/// symbol is defined to `ARENE_ON`
TEST(AreneIsOnByDefault, FalseForISymbolDefinedAsAreneOn) {
#define SOME_CONFIG_VAR_TEST_I_ ARENE_ON
  STATIC_ASSERT_FALSE(ARENE_IS_ON_BY_DEFAULT(SOME_CONFIG_VAR_TEST));
#undef SOME_CONFIG_VAR_TEST_I_
}

/// @test `ARENE_IS_ON_BY_DEFAULT(arg)` yields an expression that evaluates to `true` if the corresponding `arg_I_`
/// symbol is defined to `ARENE_ON_BY_DEFAULT`
TEST(AreneIsOnByDefault, TrueForISymbolDefinedAsAreneOnByDefault) {
#define SOME_CONFIG_VAR_TEST_I_ ARENE_ON_BY_DEFAULT
  STATIC_ASSERT_TRUE(ARENE_IS_ON_BY_DEFAULT(SOME_CONFIG_VAR_TEST));
#undef SOME_CONFIG_VAR_TEST_I_
}

/// @test `ARENE_IS_ON_BY_DEFAULT(arg)` yields an expression that evaluates to `false` if the corresponding `arg_I_`
/// symbol is defined to `ARENE_OFF`
TEST(AreneIsOnByDefault, FalseForISymbolDefinedAsAreneOff) {
#define SOME_CONFIG_VAR_TEST_I_ ARENE_OFF
  STATIC_ASSERT_FALSE(ARENE_IS_ON_BY_DEFAULT(SOME_CONFIG_VAR_TEST));
#undef SOME_CONFIG_VAR_TEST_I_
}

/// @test `ARENE_IS_ON_BY_DEFAULT(arg)` yields an expression that evaluates to `false` if the corresponding `arg_I_`
/// symbol is defined to `ARENE_OFF_BY_DEFAULT`
TEST(AreneIsOnByDefault, FalseForISymbolDefinedAsAreneOffByDefault) {
#define SOME_CONFIG_VAR_TEST_I_ ARENE_OFF_BY_DEFAULT
  STATIC_ASSERT_FALSE(ARENE_IS_ON_BY_DEFAULT(SOME_CONFIG_VAR_TEST));
#undef SOME_CONFIG_VAR_TEST_I_
}

/// @test `ARENE_IS_OFF_BY_DEFAULT(arg)` yields an expression that evaluates to `false` if the corresponding `arg_I_`
/// symbol is defined to `ARENE_ON`
TEST(AreneIsOffByDefault, FalseForISymbolDefinedAsAreneOn) {
#define SOME_CONFIG_VAR_TEST_I_ ARENE_ON
  STATIC_ASSERT_FALSE(ARENE_IS_OFF_BY_DEFAULT(SOME_CONFIG_VAR_TEST));
#undef SOME_CONFIG_VAR_TEST_I_
}

/// @test `ARENE_IS_OFF_BY_DEFAULT(arg)` yields an expression that evaluates to `false` if the corresponding `arg_I_`
/// symbol is defined to `ARENE_ON_BY_DEFAULT`
TEST(AreneIsOffByDefault, FalseForISymbolDefinedAsAreneOnByDefault) {
#define SOME_CONFIG_VAR_TEST_I_ ARENE_ON_BY_DEFAULT
  STATIC_ASSERT_FALSE(ARENE_IS_OFF_BY_DEFAULT(SOME_CONFIG_VAR_TEST));
#undef SOME_CONFIG_VAR_TEST_I_
}

/// @test `ARENE_IS_OFF_BY_DEFAULT(arg)` yields an expression that evaluates to `false` if the corresponding `arg_I_`
/// symbol is defined to `ARENE_OFF`
TEST(AreneIsOffByDefault, FalseForISymbolDefinedAsAreneOff) {
#define SOME_CONFIG_VAR_TEST_I_ ARENE_OFF
  STATIC_ASSERT_FALSE(ARENE_IS_OFF_BY_DEFAULT(SOME_CONFIG_VAR_TEST));
#undef SOME_CONFIG_VAR_TEST_I_
}

/// @test `ARENE_IS_OFF_BY_DEFAULT(arg)` yields an expression that evaluates to `true` if the corresponding `arg_I_`
/// symbol is defined to `ARENE_OFF_BY_DEFAULT`
TEST(AreneIsOffByDefault, TrueForISymbolDefinedAsAreneOffByDefault) {
#define SOME_CONFIG_VAR_TEST_I_ ARENE_OFF_BY_DEFAULT
  STATIC_ASSERT_TRUE(ARENE_IS_OFF_BY_DEFAULT(SOME_CONFIG_VAR_TEST));
#undef SOME_CONFIG_VAR_TEST_I_
}

}  // namespace

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file decays_to_test.cpp
/// @brief Unit tests for decays_to.hpp
///
///

#include "arene/base/type_traits/decays_to.hpp"

#include <gtest/gtest.h>

#include "arene/base/testing/gtest.hpp"

namespace {

using ::arene::base::decays_to_v;

class Base {};

class Unexpected : public Base {};

class Expected : public Base {};

class Derived : public Expected {};

template <typename T>
struct DecaysToVForSameTypeTest : ::testing::Test {};

using same_types = ::testing::Types<Expected, Expected const&, Expected&&>;

TYPED_TEST_SUITE(DecaysToVForSameTypeTest, same_types, );

/// @test `decays_to_v` evaluates to `true` if the arguments are `cv-ref T` and `T`
TYPED_TEST(DecaysToVForSameTypeTest, IsTrueForTypesWhichAreTheSameStrippedOfQualifiers) {
  STATIC_ASSERT_TRUE((decays_to_v<TypeParam, Expected>));
}

template <typename T>
struct DecaysToVForNotSameTypeTest : ::testing::Test {};

using not_same_types = ::testing::Types<
    Expected*,
    Unexpected*,
    Unexpected,
    Unexpected const&,
    Unexpected&&,
    Derived*,
    Derived,
    Derived const&,
    Derived&&,
    Base*,
    Base,
    Base const&,
    Base&&>;

TYPED_TEST_SUITE(DecaysToVForNotSameTypeTest, not_same_types, );

/// @test `decays_to_v` evaluates to `false` if the arguments are unrelated
TYPED_TEST(DecaysToVForNotSameTypeTest, IsFalseForTypesWhichAreNotTheSameStrippedOfQualifiers) {
  STATIC_ASSERT_FALSE((decays_to_v<TypeParam, Expected>));
}

/// @test `decays_to_v` evaluates to `true` if the arguments are array-of-`T` and `T*`
TEST(DecaysToV, ArrayTypeTest) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  STATIC_ASSERT_TRUE((decays_to_v<char[1], char*>));
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  STATIC_ASSERT_FALSE((decays_to_v<char[1], char[1]>));
}

/// @test `decays_to_v` evaluates to `true` if the arguments are `T` and `T*`, where `T` is a function type
TEST(DecaysToV, FunctionTypeTest) {
  STATIC_ASSERT_TRUE((decays_to_v<void(), void (*)()>));
  STATIC_ASSERT_FALSE((decays_to_v<void(), void()>));
}

}  // namespace

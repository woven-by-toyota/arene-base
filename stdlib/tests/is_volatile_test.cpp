// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"

namespace {

using testing::is_unambiguously_publicly_derived_from_v;

/// @brief A simple user-defined type
class some_user_type {};

using non_volatile_types = ::testing::Types<
    char,
    signed char,
    unsigned char,
    short,           // NOLINT
    unsigned short,  // NOLINT
    int,
    unsigned,
    long,                // NOLINT
    unsigned long,       // NOLINT
    long long,           // NOLINT
    unsigned long long,  // NOLINT
    bool,
    wchar_t,
    std::nullptr_t,
    float,
    double,
    long double,
    some_user_type,
    void (*)(int, double),
    int*,
    int const*,
    int volatile*,
    int const volatile*,
    void*,
    void const*,
    void volatile*,
    void const volatile*,
    int some_user_type::*,
    int (some_user_type::*)()>;

template <typename T>
class IsVolatileTest : public testing::Test {};

TYPED_TEST_SUITE(IsVolatileTest, non_volatile_types, );

/// @test @c is_volatile is derived from @c false_type and @c is_volatile_v is @c false for types that are not @c
/// volatile types
TYPED_TEST(IsVolatileTest, NonVolatileTypesYieldFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_volatile<TypeParam>, std::false_type>);
  ASSERT_FALSE(std::is_volatile_v<TypeParam>);
}

/// @test @c is_volatile is derived from @c false_type and @c is_volatile_v is @c false for references
TYPED_TEST(IsVolatileTest, ReferenceTypesYieldFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_volatile<TypeParam&>, std::false_type>);
  ASSERT_FALSE(std::is_volatile_v<TypeParam&>);
}

/// @test @c is_volatile is derived from @c false_type and @c is_volatile_v is @c false for @c volatile references
TYPED_TEST(IsVolatileTest, VolatileReferenceTypesYieldFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_volatile<TypeParam volatile&>, std::false_type>);
  ASSERT_FALSE(std::is_volatile_v<TypeParam volatile&>);
}

/// @test @c is_volatile is derived from @c true_type and @c is_volatile_v is @c true for types that are @c volatile
/// types
TYPED_TEST(IsVolatileTest, VolatileTypesYieldTrue) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_volatile<TypeParam volatile>, std::true_type>);
  ASSERT_TRUE(std::is_volatile_v<TypeParam volatile>);
}

}  // namespace

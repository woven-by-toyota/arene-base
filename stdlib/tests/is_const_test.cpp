// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"

namespace {

using testing::is_unambiguously_publicly_derived_from_v;

/// @brief A simple user-defined type
class some_user_type {};

using non_const_types = ::testing::Types<
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
class IsConstTest : public testing::Test {};

TYPED_TEST_SUITE(IsConstTest, non_const_types, );

/// @test @c is_const is derived from @c false_type and @c is_const_v is @c false for types that are not @c const types
TYPED_TEST(IsConstTest, NonConstTypesYieldFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_const<TypeParam>, std::false_type>);
  ASSERT_FALSE(std::is_const_v<TypeParam>);
}

/// @test @c is_const is derived from @c false_type and @c is_const_v is @c false for references
TYPED_TEST(IsConstTest, ReferenceTypesYieldFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_const<TypeParam&>, std::false_type>);
  ASSERT_FALSE(std::is_const_v<TypeParam&>);
}

/// @test @c is_const is derived from @c false_type and @c is_const_v is @c false for @c const references
TYPED_TEST(IsConstTest, ConstReferenceTypesYieldFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_const<TypeParam const&>, std::false_type>);
  ASSERT_FALSE(std::is_const_v<TypeParam const&>);
}

/// @test @c is_const is derived from @c true_type and @c is_const_v is @c true for types that are @c const types
TYPED_TEST(IsConstTest, ConstTypesYieldTrue) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_const<TypeParam const>, std::true_type>);
  ASSERT_TRUE(std::is_const_v<TypeParam const>);
}

}  // namespace

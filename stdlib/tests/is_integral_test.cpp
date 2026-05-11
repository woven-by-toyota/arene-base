// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/cstddef"
#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"

namespace {

using testing::is_unambiguously_publicly_derived_from_v;

/// @brief A simple user-defined type
class some_user_type {};

// See https://eel.is/c++draft/basic.fundamental#11
using integral_types = ::testing::Types<
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
    char16_t,
    char32_t>;

using non_integral_types = ::testing::Types<
    std::nullptr_t,
    void,
    int&,
    unsigned&,
    int const&,
    unsigned const&,
    int&&,
    unsigned&&,
    int const&&,
    unsigned const&&,
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
    int some_user_type::*,
    int (some_user_type::*)()>;

template <typename T>
class IsIntegralTest : public testing::Test {};

TYPED_TEST_SUITE(IsIntegralTest, integral_types, );

/// @test @c is_integral is derived from @c true_type and @c is_integral_v is @c true for integral types
TYPED_TEST(IsIntegralTest, IntegralTypesYieldTrue) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_integral<TypeParam>, std::true_type>);
  ASSERT_TRUE(std::is_integral_v<TypeParam>);
}

/// @test @c is_integral is derived from @c true_type and @c is_integral_v is @c true for @c const integral types
TYPED_TEST(IsIntegralTest, ConstIntegralTypesYieldTrue) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_integral<TypeParam const>, std::true_type>);
  ASSERT_TRUE(std::is_integral_v<TypeParam const>);
}

/// @test @c is_integral is derived from @c true_type and @c is_integral_v is @c true for @c volatile integral types
TYPED_TEST(IsIntegralTest, VolatileIntegralTypesYieldTrue) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_integral<TypeParam volatile>, std::true_type>);
  ASSERT_TRUE(std::is_integral_v<TypeParam volatile>);
}

/// @test @c is_integral is derived from @c true_type and @c is_integral_v is @c true for @c const @c volatile integral
/// types
TYPED_TEST(IsIntegralTest, ConstVolatileIntegralTypesYieldTrue) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_integral<TypeParam const volatile>, std::true_type>);
  ASSERT_TRUE(std::is_integral_v<TypeParam const volatile>);
}

template <typename T>
class IsNonIntegralTest : public testing::Test {};

TYPED_TEST_SUITE(IsNonIntegralTest, non_integral_types, );

/// @test @c is_integral is derived from @c false_type and @c is_integral_v is @c false for types that are not integral
/// types
TYPED_TEST(IsNonIntegralTest, NonIntegralTypesYieldFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_integral<TypeParam>, std::false_type>);
  ASSERT_FALSE(std::is_integral_v<TypeParam>);
}

/// @test @c is_integral is derived from @c false_type and @c is_integral_v is @c false for @c const types that are not
/// integral types
TYPED_TEST(IsNonIntegralTest, ConstNonIntegralTypesYieldFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_integral<TypeParam const>, std::false_type>);
  ASSERT_FALSE(std::is_integral_v<TypeParam const>);
}

/// @test @c is_integral is derived from @c false_type and @c is_integral_v is @c false for @c volatile types that are
/// not integral types
TYPED_TEST(IsNonIntegralTest, VolatileNonIntegralTypesYieldFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_integral<TypeParam volatile>, std::false_type>);
  ASSERT_FALSE(std::is_integral_v<TypeParam volatile>);
}

/// @test @c is_integral is derived from @c false_type and @c is_integral_v is @c false for @c const @c volatile types
/// that are not integral types
TYPED_TEST(IsNonIntegralTest, ConstVolatileNonIntegralTypesYieldFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_integral<TypeParam const volatile>, std::false_type>);
  ASSERT_FALSE(std::is_integral_v<TypeParam const volatile>);
}

}  // namespace

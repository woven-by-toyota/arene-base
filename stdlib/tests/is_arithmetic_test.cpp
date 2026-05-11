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
using arithmetic_types = ::testing::Types<
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
    float,
    double,
    long double>;

using non_arithmetic_types = ::testing::Types<
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
class IsArithmeticTest : public testing::Test {};

TYPED_TEST_SUITE(IsArithmeticTest, arithmetic_types, );

/// @test @c is_arithmetic is derived from @c true_type and @c is_arithmetic_v is @c true for arithmetic types
TYPED_TEST(IsArithmeticTest, ArithmeticTypesYieldTrue) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_arithmetic<TypeParam>, std::true_type>);
  ASSERT_TRUE(std::is_arithmetic_v<TypeParam>);
}

/// @test @c is_arithmetic is derived from @c true_type and @c is_arithmetic_v is @c true for @c const arithmetic types
TYPED_TEST(IsArithmeticTest, ConstArithmeticTypesYieldTrue) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_arithmetic<TypeParam const>, std::true_type>);
  ASSERT_TRUE(std::is_arithmetic_v<TypeParam const>);
}

/// @test @c is_arithmetic is derived from @c true_type and @c is_arithmetic_v is @c true for @c volatile arithmetic
/// types
TYPED_TEST(IsArithmeticTest, VolatileArithmeticTypesYieldTrue) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_arithmetic<TypeParam volatile>, std::true_type>);
  ASSERT_TRUE(std::is_arithmetic_v<TypeParam volatile>);
}

/// @test @c is_arithmetic is derived from @c true_type and @c is_arithmetic_v is @c true for @c const @c volatile
/// arithmetic types
TYPED_TEST(IsArithmeticTest, ConstVolatileArithmeticTypesYieldTrue) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_arithmetic<TypeParam const volatile>, std::true_type>);
  ASSERT_TRUE(std::is_arithmetic_v<TypeParam const volatile>);
}

template <typename T>
class IsNonArithmeticTest : public testing::Test {};

TYPED_TEST_SUITE(IsNonArithmeticTest, non_arithmetic_types, );

/// @test @c is_arithmetic is derived from @c false_type and @c is_arithmetic_v is @c false for types that are not
/// arithmetic types
TYPED_TEST(IsNonArithmeticTest, NonArithmeticTypesYieldFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_arithmetic<TypeParam>, std::false_type>);
  ASSERT_FALSE(std::is_arithmetic_v<TypeParam>);
}

/// @test @c is_arithmetic is derived from @c false_type and @c is_arithmetic_v is @c false for @c const types that are
/// not arithmetic types
TYPED_TEST(IsNonArithmeticTest, ConstNonArithmeticTypesYieldFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_arithmetic<TypeParam const>, std::false_type>);
  ASSERT_FALSE(std::is_arithmetic_v<TypeParam const>);
}

/// @test @c is_arithmetic is derived from @c false_type and @c is_arithmetic_v is @c false for @c volatile types that
/// are not arithmetic types
TYPED_TEST(IsNonArithmeticTest, VolatileNonArithmeticTypesYieldFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_arithmetic<TypeParam volatile>, std::false_type>);
  ASSERT_FALSE(std::is_arithmetic_v<TypeParam volatile>);
}

/// @test @c is_arithmetic is derived from @c false_type and @c is_arithmetic_v is @c false for @c const @c volatile
/// types that are not arithmetic types
TYPED_TEST(IsNonArithmeticTest, ConstVolatileNonArithmeticTypesYieldFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_arithmetic<TypeParam const volatile>, std::false_type>);
  ASSERT_FALSE(std::is_arithmetic_v<TypeParam const volatile>);
}

}  // namespace

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

// See https://eel.is/c++draft/basic.fundamental#12
using floating_point_types = ::testing::Types<float, double, long double>;

using non_floating_point_types = ::testing::Types<
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
    int (some_user_type::*)(),
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
    wchar_t>;

template <typename T>
class IsFloatingPointTest : public testing::Test {};

TYPED_TEST_SUITE(IsFloatingPointTest, floating_point_types, );

/// @test @c is_floating point is derived from @c true_type and @c is_floating point_v is @c true for floating point
/// types
TYPED_TEST(IsFloatingPointTest, FloatingPointTypesYieldTrue) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_floating_point<TypeParam>, std::true_type>);
  ASSERT_TRUE(std::is_floating_point_v<TypeParam>);
}

/// @test @c is_floating point is derived from @c true_type and @c is_floating point_v is @c true for @c const floating
/// point types
TYPED_TEST(IsFloatingPointTest, ConstFloatingPointTypesYieldTrue) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_floating_point<TypeParam const>, std::true_type>);
  ASSERT_TRUE(std::is_floating_point_v<TypeParam const>);
}

/// @test @c is_floating point is derived from @c true_type and @c is_floating point_v is @c true for @c volatile
/// floating point types
TYPED_TEST(IsFloatingPointTest, VolatileFloatingPointTypesYieldTrue) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_floating_point<TypeParam volatile>, std::true_type>);
  ASSERT_TRUE(std::is_floating_point_v<TypeParam volatile>);
}

/// @test @c is_floating point is derived from @c true_type and @c is_floating point_v is @c true for @c const @c
/// volatile floating point types
TYPED_TEST(IsFloatingPointTest, ConstVolatileFloatingPointTypesYieldTrue) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<
              std::is_floating_point<TypeParam const volatile>,
              std::true_type>);
  ASSERT_TRUE(std::is_floating_point_v<TypeParam const volatile>);
}

template <typename T>
class IsNonFloatingPointTest : public testing::Test {};

TYPED_TEST_SUITE(IsNonFloatingPointTest, non_floating_point_types, );

/// @test @c is_floating point is derived from @c false_type and @c is_floating point_v is @c false for types that are
/// not floating point types
TYPED_TEST(IsNonFloatingPointTest, NonFloatingPointTypesYieldFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_floating_point<TypeParam>, std::false_type>);
  ASSERT_FALSE(std::is_floating_point_v<TypeParam>);
}

/// @test @c is_floating point is derived from @c false_type and @c is_floating point_v is @c false for @c const types
/// that are not floating point types
TYPED_TEST(IsNonFloatingPointTest, ConstNonFloatingPointTypesYieldFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_floating_point<TypeParam const>, std::false_type>);
  ASSERT_FALSE(std::is_floating_point_v<TypeParam const>);
}

/// @test @c is_floating point is derived from @c false_type and @c is_floating point_v is @c false for @c volatile
/// types that are not floating point types
TYPED_TEST(IsNonFloatingPointTest, VolatileNonFloatingPointTypesYieldFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_floating_point<TypeParam volatile>, std::false_type>);
  ASSERT_FALSE(std::is_floating_point_v<TypeParam volatile>);
}

/// @test @c is_floating point is derived from @c false_type and @c is_floating point_v is @c false for @c const @c
/// volatile types that are not floating point types
TYPED_TEST(IsNonFloatingPointTest, ConstVolatileNonFloatingPointTypesYieldFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<
              std::is_floating_point<TypeParam const volatile>,
              std::false_type>);
  ASSERT_FALSE(std::is_floating_point_v<TypeParam const volatile>);
}

}  // namespace

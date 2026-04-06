// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"

namespace {

using testing::is_unambiguously_publicly_derived_from_v;

/// @brief A simple user-defined type
class some_user_type {};

using non_void_types = ::testing::Types<
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
    void const*,
    void volatile*,
    void const volatile*,
    int some_user_type::*,
    int (some_user_type::*)()>;

template <typename T>
class IsVoidTest : public testing::Test {};

TYPED_TEST_SUITE(IsVoidTest, non_void_types, );

/// @test @c is_void is derived from @c false_type and @c is_void_v is @c false for types that are not void types
TYPED_TEST(IsVoidTest, NonVoidTypesYieldFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_void<TypeParam>, std::false_type>);
  ASSERT_FALSE(std::is_void_v<TypeParam>);
}

/// @test @c is_void is derived from @c true_type and @c is_void_v is @c true for void types
TEST(IsVoidTest, VoidTypesYieldTrue) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_void<void>, std::true_type>);
  ASSERT_TRUE(std::is_void_v<void>);

  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_void<void const>, std::true_type>);
  ASSERT_TRUE(std::is_void_v<void const>);

  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_void<void volatile>, std::true_type>);
  ASSERT_TRUE(std::is_void_v<void volatile>);

  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_void<void const volatile>, std::true_type>);
  ASSERT_TRUE(std::is_void_v<void const volatile>);
}

}  // namespace

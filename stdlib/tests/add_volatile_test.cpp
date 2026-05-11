// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"

namespace {

/// @brief A simple user-defined type
class some_user_type {};

using add_volatile_types = ::testing::Types<
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
    float,
    double,
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
class AddVolatileTest : public testing::Test {};

TYPED_TEST_SUITE(AddVolatileTest, add_volatile_types, );

/// @test The @c type member of @c add_volatile is the provided type with a @c volatile qualifier
TYPED_TEST(AddVolatileTest, VolatileQualifierAdded) {
  ::testing::StaticAssertTypeEq<typename std::add_volatile<TypeParam>::type, TypeParam volatile>();
  ::testing::StaticAssertTypeEq<typename std::add_volatile<TypeParam const>::type, TypeParam const volatile>();
}

/// @test The @c type member of @c add_volatile is the provided type if it already has a @c volatile qualifier
TYPED_TEST(AddVolatileTest, VolatileQualifierKept) {
  ::testing::StaticAssertTypeEq<typename std::add_volatile<TypeParam volatile>::type, TypeParam volatile>();
  ::testing::StaticAssertTypeEq<typename std::add_volatile<TypeParam const volatile>::type, TypeParam const volatile>();
}

/// @test The @c type member of @c add_volatile is the provided type if it is a reference
TYPED_TEST(AddVolatileTest, ReferencesLeftUntouched) {
  ::testing::StaticAssertTypeEq<typename std::add_volatile<TypeParam&>::type, TypeParam&>();
  ::testing::StaticAssertTypeEq<typename std::add_volatile<TypeParam const&>::type, TypeParam const&>();
  ::testing::StaticAssertTypeEq<typename std::add_volatile<TypeParam volatile&>::type, TypeParam volatile&>();
  ::testing::StaticAssertTypeEq<typename std::add_volatile<TypeParam const volatile&>::type, TypeParam const volatile&>(
  );
  ::testing::StaticAssertTypeEq<typename std::add_volatile<TypeParam&&>::type, TypeParam&&>();
  ::testing::StaticAssertTypeEq<typename std::add_volatile<TypeParam const&&>::type, TypeParam const&&>();
  ::testing::StaticAssertTypeEq<typename std::add_volatile<TypeParam volatile&&>::type, TypeParam volatile&&>();
  ::testing::
      StaticAssertTypeEq<typename std::add_volatile<TypeParam const volatile&&>::type, TypeParam const volatile&&>();
}

/// @test The @c type member of @c add_volatile is the provided type if it is a function type
TYPED_TEST(AddVolatileTest, FunctionTypesLeftUntouched) {
  ::testing::StaticAssertTypeEq<typename std::add_volatile<TypeParam(void)>::type, TypeParam(void)>();
}

/// @test @c add_volatile_t is an alias for the provided type with a @c volatile qualifier
TYPED_TEST(AddVolatileTest, VolatileQualifierAddedWithAddVolatileT) {
  ::testing::StaticAssertTypeEq<std::add_volatile_t<TypeParam>, TypeParam volatile>();
  ::testing::StaticAssertTypeEq<std::add_volatile_t<TypeParam const>, TypeParam const volatile>();
}

/// @test @c add_volatile_t is an alias for the provided type if it already has a @c volatile qualifier
TYPED_TEST(AddVolatileTest, VolatileQualifierKeptWithAddVolatileT) {
  ::testing::StaticAssertTypeEq<std::add_volatile_t<TypeParam volatile>, TypeParam volatile>();
  ::testing::StaticAssertTypeEq<std::add_volatile_t<TypeParam const volatile>, TypeParam const volatile>();
}

/// @test @c add_volatile_t is an alias for the provided type if it is a reference
TYPED_TEST(AddVolatileTest, ReferencesLeftUntouchedWithAddVolatileT) {
  ::testing::StaticAssertTypeEq<std::add_volatile_t<TypeParam&>, TypeParam&>();
  ::testing::StaticAssertTypeEq<std::add_volatile_t<TypeParam const&>, TypeParam const&>();
  ::testing::StaticAssertTypeEq<std::add_volatile_t<TypeParam volatile&>, TypeParam volatile&>();
  ::testing::StaticAssertTypeEq<std::add_volatile_t<TypeParam const volatile&>, TypeParam const volatile&>();
  ::testing::StaticAssertTypeEq<std::add_volatile_t<TypeParam&&>, TypeParam&&>();
  ::testing::StaticAssertTypeEq<std::add_volatile_t<TypeParam const&&>, TypeParam const&&>();
  ::testing::StaticAssertTypeEq<std::add_volatile_t<TypeParam volatile&&>, TypeParam volatile&&>();
  ::testing::StaticAssertTypeEq<std::add_volatile_t<TypeParam const volatile&&>, TypeParam const volatile&&>();
}

/// @test @c add_volatile_t is an alias for the provided type if it is a function type
TYPED_TEST(AddVolatileTest, FunctionTypesLeftUntouchedWithAddVolatileT) {
  ::testing::StaticAssertTypeEq<std::add_volatile_t<TypeParam(void)>, TypeParam(void)>();
}

}  // namespace

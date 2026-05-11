// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"

namespace {

/// @brief A simple user-defined type
class some_user_type {};

using add_const_types = ::testing::Types<
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
class AddConstTest : public testing::Test {};

TYPED_TEST_SUITE(AddConstTest, add_const_types, );

/// @test The @c type member of @c add_const is the provided type with a @c const qualifier
TYPED_TEST(AddConstTest, ConstQualifierAdded) {
  ::testing::StaticAssertTypeEq<typename std::add_const<TypeParam>::type, TypeParam const>();
  ::testing::StaticAssertTypeEq<typename std::add_const<TypeParam volatile>::type, TypeParam const volatile>();
}

/// @test The @c type member of @c add_const is the provided type if it already has a @c const qualifier
TYPED_TEST(AddConstTest, ConstQualifierKept) {
  ::testing::StaticAssertTypeEq<typename std::add_const<TypeParam const>::type, TypeParam const>();
  ::testing::StaticAssertTypeEq<typename std::add_const<TypeParam const volatile>::type, TypeParam const volatile>();
}

/// @test The @c type member of @c add_const is the provided type if it is a reference
TYPED_TEST(AddConstTest, ReferencesLeftUntouched) {
  ::testing::StaticAssertTypeEq<typename std::add_const<TypeParam&>::type, TypeParam&>();
  ::testing::StaticAssertTypeEq<typename std::add_const<TypeParam const&>::type, TypeParam const&>();
  ::testing::StaticAssertTypeEq<typename std::add_const<TypeParam volatile&>::type, TypeParam volatile&>();
  ::testing::StaticAssertTypeEq<typename std::add_const<TypeParam const volatile&>::type, TypeParam const volatile&>();
  ::testing::StaticAssertTypeEq<typename std::add_const<TypeParam&&>::type, TypeParam&&>();
  ::testing::StaticAssertTypeEq<typename std::add_const<TypeParam const&&>::type, TypeParam const&&>();
  ::testing::StaticAssertTypeEq<typename std::add_const<TypeParam volatile&&>::type, TypeParam volatile&&>();
  ::testing::StaticAssertTypeEq<typename std::add_const<TypeParam const volatile&&>::type, TypeParam const volatile&&>(
  );
}

/// @test The @c type member of @c add_const is the provided type if it is a function type
TYPED_TEST(AddConstTest, FunctionTypesLeftUntouched) {
  ::testing::StaticAssertTypeEq<typename std::add_const<TypeParam(void)>::type, TypeParam(void)>();
}

/// @test @c add_const_t is an alias for the provided type with a @c const qualifier
TYPED_TEST(AddConstTest, ConstQualifierAddedWithAddConstT) {
  ::testing::StaticAssertTypeEq<std::add_const_t<TypeParam>, TypeParam const>();
  ::testing::StaticAssertTypeEq<std::add_const_t<TypeParam volatile>, TypeParam const volatile>();
}

/// @test @c add_const_t is an alias for the provided type if it already has a @c const qualifier
TYPED_TEST(AddConstTest, ConstQualifierKeptWithAddConstT) {
  ::testing::StaticAssertTypeEq<std::add_const_t<TypeParam const>, TypeParam const>();
  ::testing::StaticAssertTypeEq<std::add_const_t<TypeParam const volatile>, TypeParam const volatile>();
}

/// @test @c add_const_t is an alias for the provided type if it is a reference
TYPED_TEST(AddConstTest, ReferencesLeftUntouchedWithAddConstT) {
  ::testing::StaticAssertTypeEq<std::add_const_t<TypeParam&>, TypeParam&>();
  ::testing::StaticAssertTypeEq<std::add_const_t<TypeParam const&>, TypeParam const&>();
  ::testing::StaticAssertTypeEq<std::add_const_t<TypeParam volatile&>, TypeParam volatile&>();
  ::testing::StaticAssertTypeEq<std::add_const_t<TypeParam const volatile&>, TypeParam const volatile&>();
  ::testing::StaticAssertTypeEq<std::add_const_t<TypeParam&&>, TypeParam&&>();
  ::testing::StaticAssertTypeEq<std::add_const_t<TypeParam const&&>, TypeParam const&&>();
  ::testing::StaticAssertTypeEq<std::add_const_t<TypeParam volatile&&>, TypeParam volatile&&>();
  ::testing::StaticAssertTypeEq<std::add_const_t<TypeParam const volatile&&>, TypeParam const volatile&&>();
}

/// @test @c add_const_t is an alias for the provided type if it is a function type
TYPED_TEST(AddConstTest, FunctionTypesLeftUntouchedWithAddConstT) {
  ::testing::StaticAssertTypeEq<std::add_const_t<TypeParam(void)>, TypeParam(void)>();
}

}  // namespace

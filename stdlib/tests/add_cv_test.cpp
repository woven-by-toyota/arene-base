// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace {

/// @brief A simple user-defined type
class some_user_type {};

using add_cv_types = ::testing::non_array_object_types;

template <typename T>
class AddCvTest : public testing::Test {};

TYPED_TEST_SUITE(AddCvTest, add_cv_types, );

/// @test The @c type member of @c add_cv is the provided type with the @c const and @c volatile qualifiers
TYPED_TEST(AddCvTest, ConstVolatileQualifierAdded) {
  ::testing::StaticAssertTypeEq<typename std::add_cv<TypeParam>::type, TypeParam const volatile>();
  ::testing::StaticAssertTypeEq<typename std::add_cv<TypeParam const>::type, TypeParam const volatile>();
  ::testing::StaticAssertTypeEq<typename std::add_cv<TypeParam volatile>::type, TypeParam const volatile>();
}

/// @test The @c type member of @c add_cv is the provided type if it already has the @c const and @c volatile qualifiers
TYPED_TEST(AddCvTest, ConstVolatileQualifierKept) {
  ::testing::StaticAssertTypeEq<typename std::add_cv<TypeParam const volatile>::type, TypeParam const volatile>();
}

/// @test The @c type member of @c add_cv is the provided type if it is a reference
TYPED_TEST(AddCvTest, ReferencesLeftUntouched) {
  ::testing::StaticAssertTypeEq<typename std::add_cv<TypeParam&>::type, TypeParam&>();
  ::testing::StaticAssertTypeEq<typename std::add_cv<TypeParam const&>::type, TypeParam const&>();
  ::testing::StaticAssertTypeEq<typename std::add_cv<TypeParam volatile&>::type, TypeParam volatile&>();
  ::testing::StaticAssertTypeEq<typename std::add_cv<TypeParam const volatile&>::type, TypeParam const volatile&>();
  ::testing::StaticAssertTypeEq<typename std::add_cv<TypeParam&&>::type, TypeParam&&>();
  ::testing::StaticAssertTypeEq<typename std::add_cv<TypeParam const&&>::type, TypeParam const&&>();
  ::testing::StaticAssertTypeEq<typename std::add_cv<TypeParam volatile&&>::type, TypeParam volatile&&>();
  ::testing::StaticAssertTypeEq<typename std::add_cv<TypeParam const volatile&&>::type, TypeParam const volatile&&>();
}

/// @test The @c type member of @c add_cv is the provided type if it is a function type
TYPED_TEST(AddCvTest, FunctionTypesLeftUntouched) {
  ::testing::StaticAssertTypeEq<typename std::add_cv<TypeParam(void)>::type, TypeParam(void)>();
}

/// @test @c add_cv_t is an alias for the provided type with the @c const and @c volatile qualifiers
TYPED_TEST(AddCvTest, ConstVolatileQualifierAddedWithAddCvT) {
  ::testing::StaticAssertTypeEq<std::add_cv_t<TypeParam>, TypeParam const volatile>();
  ::testing::StaticAssertTypeEq<std::add_cv_t<TypeParam const>, TypeParam const volatile>();
  ::testing::StaticAssertTypeEq<std::add_cv_t<TypeParam volatile>, TypeParam const volatile>();
}

/// @test @c add_cv_t is an alias for the provided type if it already has the @c const and @c volatile qualifiers
TYPED_TEST(AddCvTest, VolatileQualifierKeptWithAddVolatileT) {
  ::testing::StaticAssertTypeEq<std::add_cv_t<TypeParam const volatile>, TypeParam const volatile>();
}

/// @test @c add_cv_t is an alias for the provided type if it is a reference
TYPED_TEST(AddCvTest, ReferencesLeftUntouchedWithAddCvT) {
  ::testing::StaticAssertTypeEq<std::add_cv_t<TypeParam&>, TypeParam&>();
  ::testing::StaticAssertTypeEq<std::add_cv_t<TypeParam const&>, TypeParam const&>();
  ::testing::StaticAssertTypeEq<std::add_cv_t<TypeParam volatile&>, TypeParam volatile&>();
  ::testing::StaticAssertTypeEq<std::add_cv_t<TypeParam const volatile&>, TypeParam const volatile&>();
  ::testing::StaticAssertTypeEq<std::add_cv_t<TypeParam&&>, TypeParam&&>();
  ::testing::StaticAssertTypeEq<std::add_cv_t<TypeParam const&&>, TypeParam const&&>();
  ::testing::StaticAssertTypeEq<std::add_cv_t<TypeParam volatile&&>, TypeParam volatile&&>();
  ::testing::StaticAssertTypeEq<std::add_cv_t<TypeParam const volatile&&>, TypeParam const volatile&&>();
}

/// @test @c add_cv_t is an alias for the provided type if it is a function type
TYPED_TEST(AddCvTest, FunctionTypesLeftUntouchedWithAddCvT) {
  ::testing::StaticAssertTypeEq<std::add_cv_t<TypeParam(void)>, TypeParam(void)>();
}

}  // namespace

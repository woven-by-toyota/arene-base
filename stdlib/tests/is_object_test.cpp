// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"

namespace {

using testing::is_unambiguously_publicly_derived_from_v;

/// @brief A simple user-defined type
class some_user_type {
 public:
  explicit some_user_type(int) noexcept(false) {}
  // NOLINTNEXTLINE(hicpp-use-equals-default)
  some_user_type(some_user_type const&) noexcept(false) {}
  // NOLINTNEXTLINE(hicpp-use-equals-default)
  some_user_type(some_user_type&&) noexcept(false) {}

  auto operator=(some_user_type const&) noexcept -> some_user_type& = default;
  auto operator=(some_user_type&&) noexcept -> some_user_type& = default;

  ~some_user_type() = default;
};

using is_object_types = ::testing::Types<
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
class IsObjectTest : public testing::Test {};

TYPED_TEST_SUITE(IsObjectTest, is_object_types, );

/// @test a scalar or pointer or class is an object
TYPED_TEST(IsObjectTest, NonReferenceYieldsTrue) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_object<TypeParam>, std::true_type>);
  ASSERT_TRUE(std::is_object_v<TypeParam>);
}

/// @test @c void is not an object
TEST(IsObjectTest, VoidYieldsFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_object<void>, std::false_type>);
  ASSERT_FALSE(std::is_object_v<void>);
}

/// @test A function type is not an object
TEST(IsObjectTest, FunctionYieldsFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_object<void()>, std::false_type>);
  ASSERT_FALSE(std::is_object_v<void()>);
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_object<void(int)>, std::false_type>);
  ASSERT_FALSE(std::is_object_v<void(int)>);
}

/// @test a non-const lvalue reference is not an object
TYPED_TEST(IsObjectTest, LvalueReferenceYieldsFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_object<TypeParam&>, std::false_type>);
  ASSERT_FALSE(std::is_object_v<TypeParam&>);
}

/// @test a const lvalue reference is not an object
TYPED_TEST(IsObjectTest, ConstLvalueReferenceYieldsFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_object<TypeParam const&>, std::false_type>);
  ASSERT_FALSE(std::is_object_v<TypeParam const&>);
}

/// @test a volatile lvalue reference is not an object
TYPED_TEST(IsObjectTest, VolatileLvalueReferenceYieldsFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_object<TypeParam volatile&>, std::false_type>);
  ASSERT_FALSE(std::is_object_v<TypeParam volatile&>);
}

/// @test a const volatile lvalue reference is not an object
TYPED_TEST(IsObjectTest, ConstVolatileLvalueReferenceYieldsFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_object<TypeParam const volatile&>, std::false_type>);
  ASSERT_FALSE(std::is_object_v<TypeParam const volatile&>);
}

/// @test a non-const rvalue reference is not an object
TYPED_TEST(IsObjectTest, RvalueReferenceYieldsFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_object<TypeParam&&>, std::false_type>);
  ASSERT_FALSE(std::is_object_v<TypeParam&&>);
}

/// @test a const rvalue reference is not an object
TYPED_TEST(IsObjectTest, ConstRvalueReferenceYieldsFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_object<TypeParam const&&>, std::false_type>);
  ASSERT_FALSE(std::is_object_v<TypeParam const&&>);
}

/// @test a volatile rvalue reference is not an object
TYPED_TEST(IsObjectTest, VolatileRvalueReferenceYieldsFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_object<TypeParam volatile&&>, std::false_type>);
  ASSERT_FALSE(std::is_object_v<TypeParam volatile&&>);
}

/// @test a const volatile rvalue reference is not an object
TYPED_TEST(IsObjectTest, ConstVolatileRvalueReferenceYieldsFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_object<TypeParam const volatile&&>, std::false_type>);
  ASSERT_FALSE(std::is_object_v<TypeParam const volatile&&>);
}

}  // namespace

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

using is_lvalue_reference_types = ::testing::Types<
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
class IsLvalueReferenceTest : public testing::Test {};

TYPED_TEST_SUITE(IsLvalueReferenceTest, is_lvalue_reference_types, );

/// @test an object or function is not an lvalue reference
TYPED_TEST(IsLvalueReferenceTest, NonReferenceYieldsFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_lvalue_reference<TypeParam>, std::false_type>);
  ASSERT_FALSE(std::is_lvalue_reference_v<TypeParam>);
}

/// @test a non-const lvalue reference is an lvalue reference
TYPED_TEST(IsLvalueReferenceTest, LvalueReferenceYieldsTrue) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_lvalue_reference<TypeParam&>, std::true_type>);
  ASSERT_TRUE(std::is_lvalue_reference_v<TypeParam&>);
}

/// @test a const lvalue reference is an lvalue reference
TYPED_TEST(IsLvalueReferenceTest, ConstLvalueReferenceYieldsTrue) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_lvalue_reference<TypeParam const&>, std::true_type>);
  ASSERT_TRUE(std::is_lvalue_reference_v<TypeParam const&>);
}

/// @test a volatile lvalue reference is an lvalue reference
TYPED_TEST(IsLvalueReferenceTest, VolatileLvalueReferenceYieldsTrue) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_lvalue_reference<TypeParam volatile&>, std::true_type>);
  ASSERT_TRUE(std::is_lvalue_reference_v<TypeParam volatile&>);
}

/// @test a const volatile lvalue reference is an lvalue reference
TYPED_TEST(IsLvalueReferenceTest, ConstVolatileLvalueReferenceYieldsTrue) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<
              std::is_lvalue_reference<TypeParam const volatile&>,
              std::true_type>);
  ASSERT_TRUE(std::is_lvalue_reference_v<TypeParam const volatile&>);
}

/// @test a non-const rvalue reference is not an lvalue reference
TYPED_TEST(IsLvalueReferenceTest, RvalueReferenceYieldsFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_lvalue_reference<TypeParam&&>, std::false_type>);
  ASSERT_FALSE(std::is_lvalue_reference_v<TypeParam&&>);
}

/// @test a const rvalue reference is not an lvalue reference
TYPED_TEST(IsLvalueReferenceTest, ConstRvalueReferenceYieldsFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_lvalue_reference<TypeParam const&&>, std::false_type>);
  ASSERT_FALSE(std::is_lvalue_reference_v<TypeParam const&&>);
}

/// @test a volatile rvalue reference is not an lvalue reference
TYPED_TEST(IsLvalueReferenceTest, VolatileRvalueReferenceYieldsFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<
              std::is_lvalue_reference<TypeParam volatile&&>,
              std::false_type>);
  ASSERT_FALSE(std::is_lvalue_reference_v<TypeParam volatile&&>);
}

/// @test a const volatile rvalue reference is not an lvalue reference
TYPED_TEST(IsLvalueReferenceTest, ConstVolatileRvalueReferenceYieldsFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<
              std::is_lvalue_reference<TypeParam const volatile&&>,
              std::false_type>);
  ASSERT_FALSE(std::is_lvalue_reference_v<TypeParam const volatile&&>);
}

}  // namespace

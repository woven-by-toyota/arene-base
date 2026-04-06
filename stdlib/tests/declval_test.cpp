// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"

namespace {
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

using declval_types = ::testing::Types<
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
class DeclvalTest : public testing::Test {};

TYPED_TEST_SUITE(DeclvalTest, declval_types, );

/// @test The return type of @c declval is @c T&&
TYPED_TEST(DeclvalTest, HasRightReturnType) {
  testing::StaticAssertTypeEq<decltype(std::declval<TypeParam>()), TypeParam&&>();
  testing::StaticAssertTypeEq<decltype(std::declval<TypeParam&>()), TypeParam&>();
  testing::StaticAssertTypeEq<decltype(std::declval<TypeParam&&>()), TypeParam&&>();

  testing::StaticAssertTypeEq<decltype(std::declval<TypeParam const>()), TypeParam const&&>();
  testing::StaticAssertTypeEq<decltype(std::declval<TypeParam const&>()), TypeParam const&>();
  testing::StaticAssertTypeEq<decltype(std::declval<TypeParam const&&>()), TypeParam const&&>();

  testing::StaticAssertTypeEq<decltype(std::declval<TypeParam volatile>()), TypeParam volatile&&>();
  testing::StaticAssertTypeEq<decltype(std::declval<TypeParam volatile&>()), TypeParam volatile&>();
  testing::StaticAssertTypeEq<decltype(std::declval<TypeParam volatile&&>()), TypeParam volatile&&>();

  testing::StaticAssertTypeEq<decltype(std::declval<TypeParam const volatile>()), TypeParam const volatile&&>();
  testing::StaticAssertTypeEq<decltype(std::declval<TypeParam const volatile&>()), TypeParam const volatile&>();
  testing::StaticAssertTypeEq<decltype(std::declval<TypeParam const volatile&&>()), TypeParam const volatile&&>();
}

/// @test @c declval is @c noexcept
TYPED_TEST(DeclvalTest, IsNoexcept) {
  ASSERT_TRUE(noexcept(std::declval<TypeParam>()));
  ASSERT_TRUE(noexcept(std::declval<TypeParam&>()));
  ASSERT_TRUE(noexcept(std::declval<TypeParam&&>()));

  ASSERT_TRUE(noexcept(std::declval<TypeParam const>()));
  ASSERT_TRUE(noexcept(std::declval<TypeParam const&>()));
  ASSERT_TRUE(noexcept(std::declval<TypeParam const&&>()));

  ASSERT_TRUE(noexcept(std::declval<TypeParam volatile>()));
  ASSERT_TRUE(noexcept(std::declval<TypeParam volatile&>()));
  ASSERT_TRUE(noexcept(std::declval<TypeParam volatile&&>()));

  ASSERT_TRUE(noexcept(std::declval<TypeParam const volatile>()));
  ASSERT_TRUE(noexcept(std::declval<TypeParam const volatile&>()));
  ASSERT_TRUE(noexcept(std::declval<TypeParam const volatile&&>()));
}

}  // namespace

// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/concat_unique.hpp"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace {

using move_types = arene::base::type_lists::concat_unique_t<
    ::testing::object_types,
    ::testing::Types<
        ::testing::not_noexcept_user_type,
        int ::testing::not_noexcept_user_type::*,
        int (::testing::not_noexcept_user_type::*)()>>;

template <typename T>
class MoveTest : public testing::Test {};

TYPED_TEST_SUITE(MoveTest, move_types, );

/// @test The return type of @c move is @c T&&
TYPED_TEST(MoveTest, HasRightReturnType) {
  testing::StaticAssertTypeEq<decltype(std::move(std::declval<TypeParam>())), TypeParam&&>();
  testing::StaticAssertTypeEq<decltype(std::move(std::declval<TypeParam&>())), TypeParam&&>();
  testing::StaticAssertTypeEq<decltype(std::move(std::declval<TypeParam&&>())), TypeParam&&>();

  testing::StaticAssertTypeEq<decltype(std::move(std::declval<TypeParam const>())), TypeParam const&&>();
  testing::StaticAssertTypeEq<decltype(std::move(std::declval<TypeParam const&>())), TypeParam const&&>();
  testing::StaticAssertTypeEq<decltype(std::move(std::declval<TypeParam const&&>())), TypeParam const&&>();

  testing::StaticAssertTypeEq<decltype(std::move(std::declval<TypeParam volatile>())), TypeParam volatile&&>();
  testing::StaticAssertTypeEq<decltype(std::move(std::declval<TypeParam volatile&>())), TypeParam volatile&&>();
  testing::StaticAssertTypeEq<decltype(std::move(std::declval<TypeParam volatile&&>())), TypeParam volatile&&>();

  testing::
      StaticAssertTypeEq<decltype(std::move(std::declval<TypeParam const volatile>())), TypeParam const volatile&&>();
  testing::
      StaticAssertTypeEq<decltype(std::move(std::declval<TypeParam const volatile&>())), TypeParam const volatile&&>();
  testing::
      StaticAssertTypeEq<decltype(std::move(std::declval<TypeParam const volatile&&>())), TypeParam const volatile&&>();
}

/// @test @c move is @c noexcept
TYPED_TEST(MoveTest, IsNoexcept) {
  ASSERT_TRUE(noexcept(std::move(std::declval<TypeParam>())));
  ASSERT_TRUE(noexcept(std::move(std::declval<TypeParam&>())));
  ASSERT_TRUE(noexcept(std::move(std::declval<TypeParam&&>())));

  ASSERT_TRUE(noexcept(std::move(std::declval<TypeParam const>())));
  ASSERT_TRUE(noexcept(std::move(std::declval<TypeParam const&>())));
  ASSERT_TRUE(noexcept(std::move(std::declval<TypeParam const&&>())));

  ASSERT_TRUE(noexcept(std::move(std::declval<TypeParam volatile>())));
  ASSERT_TRUE(noexcept(std::move(std::declval<TypeParam volatile&>())));
  ASSERT_TRUE(noexcept(std::move(std::declval<TypeParam volatile&&>())));

  ASSERT_TRUE(noexcept(std::move(std::declval<TypeParam const volatile>())));
  ASSERT_TRUE(noexcept(std::move(std::declval<TypeParam const volatile&>())));
  ASSERT_TRUE(noexcept(std::move(std::declval<TypeParam const volatile&&>())));
}

/// @test The return value of @c std::move is a reference to the supplied argument
CONSTEXPR_TYPED_TEST(MoveTest, TheReturnValueIsARefToArgument) {
  TypeParam value{};

  auto&& result = std::move(value);
  ASSERT_EQ(&result, &value);
}

}  // namespace

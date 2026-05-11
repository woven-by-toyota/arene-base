// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/concat_unique.hpp"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace {

using forward_types = arene::base::type_lists::concat_unique_t<
    ::testing::object_types,
    ::testing::Types<
        ::testing::not_noexcept_user_type,
        int ::testing::not_noexcept_user_type::*,
        int (::testing::not_noexcept_user_type::*)()>>;

template <typename T>
class ForwardTest : public testing::Test {};

TYPED_TEST_SUITE(ForwardTest, forward_types, );

/// @test The return type of @c forward is @c T&&
TYPED_TEST(ForwardTest, HasRightReturnType) {
  testing::StaticAssertTypeEq<decltype(std::forward<TypeParam>(std::declval<TypeParam>())), TypeParam&&>();
  testing::StaticAssertTypeEq<decltype(std::forward<TypeParam>(std::declval<TypeParam&>())), TypeParam&&>();
  testing::StaticAssertTypeEq<decltype(std::forward<TypeParam>(std::declval<TypeParam&&>())), TypeParam&&>();
  testing::StaticAssertTypeEq<decltype(std::forward<TypeParam&>(std::declval<TypeParam&>())), TypeParam&>();
  testing::StaticAssertTypeEq<decltype(std::forward<TypeParam&&>(std::declval<TypeParam&&>())), TypeParam&&>();

  testing::
      StaticAssertTypeEq<decltype(std::forward<TypeParam const>(std::declval<TypeParam const>())), TypeParam const&&>();
  testing::
      StaticAssertTypeEq<decltype(std::forward<TypeParam const&>(std::declval<TypeParam const&>())), TypeParam const&>(
      );
  testing::StaticAssertTypeEq<
      decltype(std::forward<TypeParam const&&>(std::declval<TypeParam const&&>())),
      TypeParam const&&>();

  testing::StaticAssertTypeEq<
      decltype(std::forward<TypeParam volatile>(std::declval<TypeParam volatile>())),
      TypeParam volatile&&>();
  testing::StaticAssertTypeEq<
      decltype(std::forward<TypeParam volatile&>(std::declval<TypeParam volatile&>())),
      TypeParam volatile&>();
  testing::StaticAssertTypeEq<
      decltype(std::forward<TypeParam volatile&&>(std::declval<TypeParam volatile&&>())),
      TypeParam volatile&&>();

  testing::StaticAssertTypeEq<
      decltype(std::forward<TypeParam const volatile>(std::declval<TypeParam const volatile>())),
      TypeParam const volatile&&>();
  testing::StaticAssertTypeEq<
      decltype(std::forward<TypeParam const volatile&>(std::declval<TypeParam const volatile&>())),
      TypeParam const volatile&>();
  testing::StaticAssertTypeEq<
      decltype(std::forward<TypeParam const volatile&&>(std::declval<TypeParam const volatile&&>())),
      TypeParam const volatile&&>();
}

/// @test @c forward is @c noexcept
TYPED_TEST(ForwardTest, IsNoexcept) {
  ASSERT_TRUE(noexcept(std::forward<TypeParam>(std::declval<TypeParam>())));
  ASSERT_TRUE(noexcept(std::forward<TypeParam&>(std::declval<TypeParam&>())));
  ASSERT_TRUE(noexcept(std::forward<TypeParam&&>(std::declval<TypeParam&&>())));

  ASSERT_TRUE(noexcept(std::forward<TypeParam const>(std::declval<TypeParam const>())));
  ASSERT_TRUE(noexcept(std::forward<TypeParam const&>(std::declval<TypeParam const&>())));
  ASSERT_TRUE(noexcept(std::forward<TypeParam const&&>(std::declval<TypeParam const&&>())));

  ASSERT_TRUE(noexcept(std::forward<TypeParam volatile>(std::declval<TypeParam volatile>())));
  ASSERT_TRUE(noexcept(std::forward<TypeParam volatile&>(std::declval<TypeParam volatile&>())));
  ASSERT_TRUE(noexcept(std::forward<TypeParam volatile&&>(std::declval<TypeParam volatile&&>())));

  ASSERT_TRUE(noexcept(std::forward<TypeParam const volatile>(std::declval<TypeParam const volatile>())));
  ASSERT_TRUE(noexcept(std::forward<TypeParam const volatile&>(std::declval<TypeParam const volatile&>())));
  ASSERT_TRUE(noexcept(std::forward<TypeParam const volatile&&>(std::declval<TypeParam const volatile&&>())));
}

/// @test The return value of @c std::forward is a reference to the supplied argument
CONSTEXPR_TYPED_TEST(ForwardTest, TheReturnValueIsARefToArgument) {
  TypeParam value{};

  auto&& rvalue_result = std::forward<TypeParam>(static_cast<TypeParam&&>(value));
  ASSERT_EQ(&rvalue_result, &value);

  auto&& lvalue_result = std::forward<TypeParam>(value);
  ASSERT_EQ(&lvalue_result, &value);
}

}  // namespace

// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace {

using testing::is_unambiguously_publicly_derived_from_v;

using is_fundamental_types = ::testing::fundamental_types;
using not_fundamental_types = ::testing::compound_types;

template <typename T>
class IsFundamentalTest : public testing::Test {};

TYPED_TEST_SUITE(IsFundamentalTest, is_fundamental_types, );

/// @test an integral, floating point, boolean, character, void or nullptr type is a fundamental type
TYPED_TEST(IsFundamentalTest, FundamentalTypeYieldsTrue) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_fundamental<TypeParam>, std::true_type>);
  ASSERT_TRUE(std::is_fundamental_v<TypeParam>);
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_fundamental<TypeParam const>, std::true_type>);
  ASSERT_TRUE(std::is_fundamental_v<TypeParam const>);
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_fundamental<TypeParam volatile>, std::true_type>);
  ASSERT_TRUE(std::is_fundamental_v<TypeParam volatile>);
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_fundamental<TypeParam const volatile>, std::true_type>);
  ASSERT_TRUE(std::is_fundamental_v<TypeParam const volatile>);
}

template <typename T>
class IsNotFundamentalTest : public testing::Test {};

TYPED_TEST_SUITE(IsNotFundamentalTest, not_fundamental_types, );

/// @test Not fundamental types are not a fundamental type
TYPED_TEST(IsNotFundamentalTest, NotFundamentalTypesYieldFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_fundamental<TypeParam>, std::false_type>);
  ASSERT_FALSE(std::is_fundamental_v<TypeParam>);
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_fundamental<TypeParam const>, std::false_type>);
  ASSERT_FALSE(std::is_fundamental_v<TypeParam const>);
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_fundamental<TypeParam volatile>, std::false_type>);
  ASSERT_FALSE(std::is_fundamental_v<TypeParam volatile>);
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_fundamental<TypeParam const volatile>, std::false_type>);
  ASSERT_FALSE(std::is_fundamental_v<TypeParam const volatile>);
}

}  // namespace

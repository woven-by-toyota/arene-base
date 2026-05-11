// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace {

template <typename T>
class VoidTTest : public testing::Test {};

TYPED_TEST_SUITE(VoidTTest, testing::object_types, );

/// @test @c void_t is an alias for void
TYPED_TEST(VoidTTest, VoidTIsAliasForVoid) { testing::StaticAssertTypeEq<std::void_t<TypeParam>, void>(); }

namespace using_void_t_to_test_sfinae {
// primary template handles types that do not support pre-increment:
template <class, class = void>
struct has_pre_increment_member : std::false_type {};

// specialization recognizes types that do support pre-increment:
template <class T>
struct has_pre_increment_member<T, std::void_t<decltype(++std::declval<T&>())>> : std::true_type {};
}  // namespace using_void_t_to_test_sfinae

/// @test @c void_t can be used for SFINAE checks
CONSTEXPR_TEST(VoidTTest, VoidTUsedToTestSfinae) {
  struct empty {};

  ASSERT_FALSE(using_void_t_to_test_sfinae::has_pre_increment_member<empty>::value);
  ASSERT_TRUE(using_void_t_to_test_sfinae::has_pre_increment_member<int>::value);
}

}  // namespace

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/iterator/detail/cond_value_type.hpp"

#include <gtest/gtest.h>

#include "arene/base/iterator/detail/has_member_value_type.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

using arene::base::iterator_detail::cond_value_type;
using arene::base::iterator_detail::has_member_value_type_v;

template <typename T>
class CondValueTypeNonObjectTest : public ::testing::Test {};

using non_object_types = ::testing::Types<int&, int const&, int&&, int const&&, void, int (&)(), int (&&)()>;

TYPED_TEST_SUITE(CondValueTypeNonObjectTest, non_object_types, );

/// @test Given a type `T` which does not satisfy `std::is_object<T>`, then `cond_value_type` does not have a member
/// typedef `value_type`.
TYPED_TEST(CondValueTypeNonObjectTest, DoesNotHaveMemberValueTypedef) {
  STATIC_ASSERT_FALSE(has_member_value_type_v<cond_value_type<TypeParam>>);
}

template <typename T>
class CondValueTypeObjectTest : public ::testing::Test {};

struct udt_struct {};
// NOLINTNEXTLINE(hicpp-avoid-c-arrays) explicitly testing for c-array behavior
using object_types = ::testing::Types<int, int const, int[], int[3], int*, int* const, udt_struct>;

TYPED_TEST_SUITE(CondValueTypeObjectTest, object_types, );

/// @test Given a type `T` which satisfies `std::is_object<T>`, then `cond_value_type` has a member typedef `value_type`
/// equal to `std::remove_cv_t<T>`.
TYPED_TEST(CondValueTypeObjectTest, HasMemberValueType) {
  STATIC_ASSERT_TRUE(has_member_value_type_v<cond_value_type<TypeParam>>);
  ::testing::StaticAssertTypeEq<typename cond_value_type<TypeParam>::value_type, std::remove_cv_t<TypeParam>>();
}

}  // namespace

// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/concat_unique.hpp"
#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace {

/// @brief Assert @c add_rvalue_reference<Input>::type is the same as @c ExpectedResult
/// @tparam Input The input type to add an rvalue reference to
/// @tparam ExpectedResult The expected result of adding an rvalue reference to the @c Input type
template <typename Input, typename ExpectedResult>
constexpr auto static_assert_add_rvalue_reference() noexcept -> void {
  testing::StaticAssertTypeEq<typename std::add_rvalue_reference<Input>::type, ExpectedResult>();
  testing::StaticAssertTypeEq<std::add_rvalue_reference_t<Input>, ExpectedResult>();
}

template <typename T>
class AddRvalueReferenceSuite : public testing::Test {};

using add_rvalue_reference_types =
    arene::base::type_lists::concat_unique_t<::testing::object_types, ::testing::function_types>;
TYPED_TEST_SUITE(AddRvalueReferenceSuite, add_rvalue_reference_types, );

/// @test @c add_rvalue_reference works correctly with object types
TYPED_TEST(AddRvalueReferenceSuite, WithObjectTypes) {
  static_assert_add_rvalue_reference<TypeParam, TypeParam&&>();
  static_assert_add_rvalue_reference<TypeParam const, TypeParam const&&>();
  static_assert_add_rvalue_reference<TypeParam volatile, TypeParam volatile&&>();
  static_assert_add_rvalue_reference<TypeParam const volatile, TypeParam const volatile&&>();
}

/// @test @c add_rvalue_reference works correctly with reference types
TYPED_TEST(AddRvalueReferenceSuite, WithReferenceTypes) {
  testing::StaticAssertTypeEq<typename std::add_rvalue_reference<TypeParam&>::type, TypeParam&>();
  testing::StaticAssertTypeEq<std::add_rvalue_reference_t<TypeParam&>, TypeParam&>();
  testing::StaticAssertTypeEq<typename std::add_rvalue_reference<TypeParam const&>::type, TypeParam const&>();
  testing::StaticAssertTypeEq<std::add_rvalue_reference_t<TypeParam const&>, TypeParam const&>();
  testing::StaticAssertTypeEq<typename std::add_rvalue_reference<TypeParam volatile&>::type, TypeParam volatile&>();
  testing::StaticAssertTypeEq<std::add_rvalue_reference_t<TypeParam volatile&>, TypeParam volatile&>();
  testing::StaticAssertTypeEq<
      typename std::add_rvalue_reference<TypeParam const volatile&>::type,
      TypeParam const volatile&>();
  testing::StaticAssertTypeEq<std::add_rvalue_reference_t<TypeParam const volatile&>, TypeParam const volatile&>();

  testing::StaticAssertTypeEq<typename std::add_rvalue_reference<TypeParam&&>::type, TypeParam&&>();
  testing::StaticAssertTypeEq<std::add_rvalue_reference_t<TypeParam&&>, TypeParam&&>();
  testing::StaticAssertTypeEq<typename std::add_rvalue_reference<TypeParam const&&>::type, TypeParam const&&>();
  testing::StaticAssertTypeEq<std::add_rvalue_reference_t<TypeParam const&&>, TypeParam const&&>();
  testing::StaticAssertTypeEq<typename std::add_rvalue_reference<TypeParam volatile&&>::type, TypeParam volatile&&>();
  testing::StaticAssertTypeEq<std::add_rvalue_reference_t<TypeParam volatile&&>, TypeParam volatile&&>();
  testing::StaticAssertTypeEq<
      typename std::add_rvalue_reference<TypeParam const volatile&&>::type,
      TypeParam const volatile&&>();
  testing::StaticAssertTypeEq<std::add_rvalue_reference_t<TypeParam const volatile&&>, TypeParam const volatile&&>();
}

template <typename T>
class AddRvalueReferenceNonreferenceableSuite : public testing::Test {};

TYPED_TEST_SUITE(AddRvalueReferenceNonreferenceableSuite, ::testing::non_referenceable_types, );

/// @test @c add_rvalue_reference works correctly with non-referenceable types
TYPED_TEST(AddRvalueReferenceNonreferenceableSuite, NotReferenceableType) {
  static_assert_add_rvalue_reference<TypeParam, TypeParam>();
  static_assert_add_rvalue_reference<TypeParam const, TypeParam const>();
  static_assert_add_rvalue_reference<TypeParam volatile, TypeParam volatile>();
  static_assert_add_rvalue_reference<TypeParam const volatile, TypeParam const volatile>();
}
}  // namespace

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/concat_unique.hpp"
#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace {

/// @brief Assert @c add_pointer<Input>::type is the same as @c ExpectedResult
/// @tparam Input The input type to add a pointer to
/// @tparam ExpectedResult The expected result of adding a pointer to the @c Input type
template <typename Input, typename ExpectedResult>
constexpr auto static_assert_add_pointer() noexcept -> void {
  testing::StaticAssertTypeEq<typename std::add_pointer<Input>::type, ExpectedResult>();
  testing::StaticAssertTypeEq<std::add_pointer_t<Input>, ExpectedResult>();
}

template <typename T>
class AddPointerSuite : public testing::Test {};

using add_pointer_types = arene::base::type_lists::concat_unique_t<::testing::object_types, ::testing::function_types>;
TYPED_TEST_SUITE(AddPointerSuite, add_pointer_types, );

/// @test @c add_pointer works correctly with object types
TYPED_TEST(AddPointerSuite, WithObjectTypes) {
  static_assert_add_pointer<TypeParam, TypeParam*>();
  static_assert_add_pointer<TypeParam const, TypeParam const*>();
  static_assert_add_pointer<TypeParam volatile, TypeParam volatile*>();
  static_assert_add_pointer<TypeParam const volatile, TypeParam const volatile*>();
}

/// @test @c add_pointer works correctly with void types
TEST(AddPointerSuite, WithVoidTypes) {
  static_assert_add_pointer<void, void*>();
  static_assert_add_pointer<void const, void const*>();
  static_assert_add_pointer<void volatile, void volatile*>();
  static_assert_add_pointer<void const volatile, void const volatile*>();
}

/// @test @c add_pointer works correctly with reference types
TYPED_TEST(AddPointerSuite, WithReferenceTypes) {
  static_assert_add_pointer<TypeParam&, TypeParam*>();
  static_assert_add_pointer<TypeParam const&, TypeParam const*>();
  static_assert_add_pointer<TypeParam volatile&, TypeParam volatile*>();
  static_assert_add_pointer<TypeParam const volatile&, TypeParam const volatile*>();
  static_assert_add_pointer<TypeParam&&, TypeParam*>();
  static_assert_add_pointer<TypeParam const&&, TypeParam const*>();
  static_assert_add_pointer<TypeParam volatile&&, TypeParam volatile*>();
  static_assert_add_pointer<TypeParam const volatile&&, TypeParam const volatile*>();
}

template <typename T>
class AddPointerNonreferenceableSuite : public testing::Test {};

TYPED_TEST_SUITE(AddPointerNonreferenceableSuite, ::testing::cv_qualified_function_types, );

/// @test @c add_pointer works correctly with non-referenceable types
TYPED_TEST(AddPointerNonreferenceableSuite, NotReferenceableType) {
  static_assert_add_pointer<TypeParam, TypeParam>();
  static_assert_add_pointer<TypeParam const, TypeParam const>();
  static_assert_add_pointer<TypeParam volatile, TypeParam volatile>();
  static_assert_add_pointer<TypeParam const volatile, TypeParam const volatile>();
}

}  // namespace

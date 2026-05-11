// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/concat_unique.hpp"
#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace {

/// @brief Assert @c remove_pointer<Input>::type is the same as @c ExpectedResult
/// @tparam Input The input type to remove a pointer from
/// @tparam ExpectedResult The expected result of removing a pointer from the @c Input type
template <typename Input, typename ExpectedResult>
constexpr auto static_assert_remove_pointer() noexcept -> void {
  testing::StaticAssertTypeEq<typename std::remove_pointer<Input>::type, ExpectedResult>();
  testing::StaticAssertTypeEq<std::remove_pointer_t<Input>, ExpectedResult>();
}

// This includes all types other than references.
using pointable_types =
    arene::base::type_lists::concat_unique_t<testing::object_types, testing::void_types, testing::function_types>;

template <typename T>
class RemovePointerFromPointerSuite : public testing::Test {};

TYPED_TEST_SUITE(RemovePointerFromPointerSuite, pointable_types, );

/// @test @c remove_pointer correctly returns the base type from a pointer
TYPED_TEST(RemovePointerFromPointerSuite, RemovePointerFromPointerTest) {
  static_assert_remove_pointer<TypeParam*, TypeParam>();

  static_assert_remove_pointer<TypeParam const*, TypeParam const>();
  static_assert_remove_pointer<TypeParam volatile*, TypeParam volatile>();
  static_assert_remove_pointer<TypeParam const volatile*, TypeParam const volatile>();

  static_assert_remove_pointer<TypeParam* const, TypeParam>();
  static_assert_remove_pointer<TypeParam* volatile, TypeParam>();
  static_assert_remove_pointer<TypeParam* const volatile, TypeParam>();

  static_assert_remove_pointer<TypeParam const* const, TypeParam const>();
  static_assert_remove_pointer<TypeParam const* volatile, TypeParam const>();
  static_assert_remove_pointer<TypeParam const* const volatile, TypeParam const>();

  static_assert_remove_pointer<TypeParam volatile* const, TypeParam volatile>();
  static_assert_remove_pointer<TypeParam volatile* volatile, TypeParam volatile>();
  static_assert_remove_pointer<TypeParam volatile* const volatile, TypeParam volatile>();

  static_assert_remove_pointer<TypeParam const volatile* const, TypeParam const volatile>();
  static_assert_remove_pointer<TypeParam const volatile* volatile, TypeParam const volatile>();
  static_assert_remove_pointer<TypeParam const volatile* const volatile, TypeParam const volatile>();
}

template <typename T>
class RemovePointerFromNonPointerSuite : public testing::Test {};

// All types other than pointers, function pointers and member pointers.
using non_pointer_types = arene::base::type_lists::concat_unique_t<
    testing::fundamental_types,
    testing::reference_types,
    testing::enum_types,
    testing::array_types,
    testing::union_types,
    testing::class_types,
    testing::function_types>;

TYPED_TEST_SUITE(RemovePointerFromNonPointerSuite, non_pointer_types, );

/// @test @c remove_pointer correctly returns the input type when not given a pointer
TYPED_TEST(RemovePointerFromNonPointerSuite, RemovePointerFromNonPointerTest) {
  static_assert_remove_pointer<TypeParam, TypeParam>();
  static_assert_remove_pointer<TypeParam const, TypeParam const>();
  static_assert_remove_pointer<TypeParam volatile, TypeParam volatile>();
  static_assert_remove_pointer<TypeParam const volatile, TypeParam const volatile>();
}

}  // namespace

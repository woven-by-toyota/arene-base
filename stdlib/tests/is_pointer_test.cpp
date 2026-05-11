// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/concat_unique.hpp"
#include "arene/base/type_list/remove_duplicates.hpp"
#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"
#include "testlibs/utilities/mutate.hpp"

namespace {

using arene::base::type_lists::concat_unique_t;
using arene::base::type_lists::remove_duplicates_t;
using testing::is_unambiguously_publicly_derived_from_v;

using pointer_capable_types = remove_duplicates_t<::testing::extend_with_cv_qualifiers_t<concat_unique_t<
    ::testing::arithmetic_types,
    ::testing::array_types,
    ::testing::class_types,
    ::testing::enum_types,
    ::testing::function_types,
    ::testing::union_types>>>;
using non_pointer_types =
    concat_unique_t<pointer_capable_types, ::testing::extend_with_cv_qualifiers_t<::testing::reference_types>>;

/// @brief Assert @c is_pointer<Input> derives from @c std::false_type and @c is_pointer_v<Type> is @c false
/// @tparam Input The input type to check if pointer type
template <typename Input>
constexpr auto static_assert_is_pointer_false() noexcept -> void {
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_pointer<Input>, std::false_type>,
      "is_pointer not false_type"
  );
  static_assert(!std::is_pointer_v<Input>, "is_pointer_v not false");
}

/// @brief Assert @c is_pointer<Input> derives from @c std::true_type and @c is_pointer_v<Type> is @c true
/// @tparam Input The input type to check if pointer type
template <typename Input>
constexpr auto static_assert_is_pointer_true() noexcept -> void {
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_pointer<Input>, std::true_type>,
      "is_pointer not true_type"
  );
  static_assert(std::is_pointer_v<Input>, "is_pointer_v not true");
}

template <typename T>
class IsNonPointerTest : public testing::Test {};

TYPED_TEST_SUITE(IsNonPointerTest, non_pointer_types, );

/// @test @c is_pointer is derived from @c false_type and @c is_pointer_v is @c false for types that are not
/// pointer types
TYPED_TEST(IsNonPointerTest, WithNonPointerTypes) { static_assert_is_pointer_false<TypeParam>(); }

template <typename T>
class IsPointerTest : public testing::Test {};

TYPED_TEST_SUITE(IsPointerTest, pointer_capable_types, );

/// @test @c is_pointer is derived from @c true_type and @c is_pointer_v is @c true for types that are pointer
/// types
TYPED_TEST(IsPointerTest, WithPointerTypes) {
  static_assert_is_pointer_true<TypeParam*>();
  static_assert_is_pointer_true<TypeParam const*>();
  static_assert_is_pointer_true<TypeParam volatile*>();
  static_assert_is_pointer_true<TypeParam const volatile*>();
}

}  // namespace

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_traits/is_copyable.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/testing/gtest.hpp"
#include "testlibs/utilities/configurable_value.hpp"

namespace {

using ::arene::base::is_copyable_v;

using copyable_type = testing::configurable_value<int, testing::throws_on::nothing, testing::disable::nothing>;

using copy_only = testing::configurable_value<
    int,
    testing::throws_on::nothing,
    (testing::disable::move_construct | testing::disable::move_assign)>;

using move_only = testing::configurable_value<
    int,
    testing::throws_on::nothing,
    (testing::disable::copy_construct | testing::disable::copy_assign)>;

using not_copy_constructible =
    testing::configurable_value<int, testing::throws_on::nothing, testing::disable::copy_construct>;

using not_copy_assignable =
    testing::configurable_value<int, testing::throws_on::nothing, testing::disable::copy_assign>;

using not_movable = testing::configurable_value<int, testing::throws_on::nothing, testing::disable::move_construct>;

using not_swappable = testing::configurable_value<
    int,
    testing::throws_on::nothing,
    (testing::disable::copy_construct | testing::disable::copy_assign | testing::disable::move_construct |
     testing::disable::move_assign | testing::disable::swap)>;

using throwing_copy = testing::configurable_value<
    int,
    (testing::throws_on::copy_construct | testing::throws_on::copy_assign),
    testing::disable::nothing>;

// Type with custom ADL swap
struct adl_swappable_copyable {
  adl_swappable_copyable() = default;
  ~adl_swappable_copyable() = default;

  adl_swappable_copyable(adl_swappable_copyable const&) = default;
  auto operator=(adl_swappable_copyable const&) -> adl_swappable_copyable& = default;

  adl_swappable_copyable(adl_swappable_copyable&&) = default;
  auto operator=(adl_swappable_copyable&&) -> adl_swappable_copyable& = default;
};

ARENE_MAYBE_UNUSED void swap(adl_swappable_copyable&, adl_swappable_copyable&) noexcept {}

template <typename T>
class CopyableForCopyableTypesTest : public ::testing::Test {};

using copyable_types = ::testing::Types<int, char, float, double, copyable_type, throwing_copy, adl_swappable_copyable>;
TYPED_TEST_SUITE(CopyableForCopyableTypesTest, copyable_types, );

/// @test Given an input type `T`, when all of `std::is_copy_constructible<T>::value`, `arene::base::is_movable_v<T>`,
/// `std::is_assignable<T&, T&>::value`, `std::is_assignable<T&, T const&>::value`, `std::is_assignable<T&, T
/// const>::value` are `true`, then `is_movable_v<T>` is `true`
TYPED_TEST(CopyableForCopyableTypesTest, IsTrue) { STATIC_ASSERT_TRUE(is_copyable_v<TypeParam>); }

/// @test `copyable` returns `false` for reference types
TYPED_TEST(CopyableForCopyableTypesTest, ReferenceTypesAreFalse) {
  STATIC_ASSERT_FALSE(is_copyable_v<TypeParam&>);
  STATIC_ASSERT_FALSE(is_copyable_v<TypeParam const&>);
  STATIC_ASSERT_FALSE(is_copyable_v<TypeParam&&>);
  STATIC_ASSERT_FALSE(is_copyable_v<TypeParam const&&>);
}

template <typename T>
class CopyableForNonCopyableTypesTest : public ::testing::Test {};

using non_copyable_types = ::testing::Types<copy_only, not_copy_constructible, not_copy_assignable, not_movable>;
TYPED_TEST_SUITE(CopyableForNonCopyableTypesTest, non_copyable_types, );

/// @test Given an input type `T`, when one of `std::is_copy_constructible<T>::value`, `arene::base::is_movable_v<T>`,
/// `std::is_assignable<T&, T&>::value`, `std::is_assignable<T&, T const&>::value`, `std::is_assignable<T&, T
/// const>::value` are `false`, then `is_movable_v<T>` is `false`
TYPED_TEST(CopyableForNonCopyableTypesTest, IsFalse) { STATIC_ASSERT_FALSE(is_copyable_v<TypeParam>); }

/// @test `copyable` returns `true` for pointer types (except const pointers)
TEST(CopyableTest, PointerTypes) {
  STATIC_ASSERT_TRUE(is_copyable_v<int*>);
  STATIC_ASSERT_TRUE(is_copyable_v<int const*>);
  STATIC_ASSERT_FALSE(is_copyable_v<int* const>);        // const pointer is not move-assignable
  STATIC_ASSERT_FALSE(is_copyable_v<int const* const>);  // const pointer is not move-assignable

  STATIC_ASSERT_TRUE(is_copyable_v<copyable_type*>);
  STATIC_ASSERT_TRUE(is_copyable_v<move_only*>);
  STATIC_ASSERT_TRUE(is_copyable_v<not_movable*>);
}

/// @test `copyable` returns `false` for array types (arrays are not movable)
TEST(CopyableTest, ArrayTypesAreFalse) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays) - intentionally testing C-style arrays
  STATIC_ASSERT_FALSE(is_copyable_v<int[10]>);
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays) - intentionally testing C-style arrays
  STATIC_ASSERT_FALSE(is_copyable_v<int[]>);
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays) - intentionally testing C-style arrays
  STATIC_ASSERT_FALSE(is_copyable_v<copyable_type[5]>);
}

/// @test `copyable` handles void types correctly
TEST(CopyableTest, VoidTypeHandling) {
  static_assert(!is_copyable_v<void>, "void is not an object type");
  static_assert(!is_copyable_v<void const>, "const void is not an object type");
  static_assert(!is_copyable_v<void volatile>, "volatile void is not an object type");
  static_assert(!is_copyable_v<void const volatile>, "const volatile void is not an object type");
}

/// @test `copyable` returns `false` for function types (functions are not object types)
TEST(CopyableTest, FunctionTypesAreFalse) {
  STATIC_ASSERT_FALSE(is_copyable_v<void()>);
  STATIC_ASSERT_FALSE(is_copyable_v<int(int)>);
  STATIC_ASSERT_FALSE(is_copyable_v<void(int, float)>);
}

/// @test `copyable` returns `false` for const types
TEST(CopyableTest, CVQualifiedTypes) {
  STATIC_ASSERT_TRUE(is_copyable_v<int>);
  STATIC_ASSERT_FALSE(is_copyable_v<int const>);
  STATIC_ASSERT_TRUE(is_copyable_v<int volatile>);
  STATIC_ASSERT_FALSE(is_copyable_v<int const volatile>);

  STATIC_ASSERT_TRUE(is_copyable_v<copyable_type>);
  STATIC_ASSERT_FALSE(is_copyable_v<copyable_type const>);
  STATIC_ASSERT_FALSE(is_copyable_v<copyable_type volatile>);  // volatile is_copyable_v_type doesn't have volatile copy
                                                               // ops
  STATIC_ASSERT_FALSE(is_copyable_v<copyable_type const volatile>);
}

}  // namespace

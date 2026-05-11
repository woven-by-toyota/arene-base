// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_traits/is_movable.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/testing/gtest.hpp"
#include "testlibs/utilities/configurable_value.hpp"

namespace {

using ::arene::base::is_movable_v;

using move_only = testing::configurable_value<
    int,
    testing::throws_on::nothing,
    (testing::disable::copy_construct | testing::disable::copy_assign)>;

using copy_and_movable = testing::configurable_value<int, testing::throws_on::nothing, testing::disable::nothing>;
using not_move_constructible = testing::configurable_value<
    int,
    testing::throws_on::nothing,
    (testing::disable::copy_construct | testing::disable::copy_assign | testing::disable::move_construct)>;

using not_move_assignable = testing::configurable_value<
    int,
    testing::throws_on::nothing,
    (testing::disable::copy_construct | testing::disable::copy_assign | testing::disable::move_construct |
     testing::disable::move_assign)>;

using not_swappable = testing::configurable_value<
    int,
    testing::throws_on::nothing,
    (testing::disable::copy_construct | testing::disable::copy_assign | testing::disable::move_construct |
     testing::disable::move_assign | testing::disable::swap)>;

// Type with custom ADL swap
struct adl_swappable {
  adl_swappable() = default;
  ~adl_swappable() = default;

  adl_swappable(adl_swappable const&) = delete;
  auto operator=(adl_swappable const&) -> adl_swappable& = delete;

  adl_swappable(adl_swappable&&) = delete;
  auto operator=(adl_swappable&&) -> adl_swappable& = delete;
};

ARENE_MAYBE_UNUSED void swap(adl_swappable&, adl_swappable&) noexcept {}

template <typename T>
class MovableForMovableTypesTest : public ::testing::Test {};

using movable_types = ::testing::Types<int, char, float, double, move_only, copy_and_movable>;
TYPED_TEST_SUITE(MovableForMovableTypesTest, movable_types, );

/// @test Given an input type `T`, when all of `std::is_object<T>::value`, `std::is_move_constructible<T>::value`,
/// `std::is_assignable<T&, T>::value`, and `is_swappable_v<T>` are true, then `is_movable_v<T>` is `true`
TYPED_TEST(MovableForMovableTypesTest, IsTrue) { STATIC_ASSERT_TRUE(is_movable_v<TypeParam>); }

template <typename T>
class MovableForNonMovableTypesTest : public ::testing::Test {};

using non_movable_types = ::testing::Types<not_move_constructible, not_move_assignable, not_swappable>;
TYPED_TEST_SUITE(MovableForNonMovableTypesTest, non_movable_types, );

/// @test Given an input type `T`, when one of `std::is_object<T>::value`, `std::is_move_constructible<T>::value`,
/// `std::is_assignable<T&, T>::value`, and `is_swappable_v<T>` are false, then `is_movable_v<T>` is `false`
TYPED_TEST(MovableForNonMovableTypesTest, IsFalse) { STATIC_ASSERT_FALSE(is_movable_v<TypeParam>); }

/// @test `is_movable_v` returns `false` for reference types
TEST(MovableTest, ReferenceTypesAreFalse) {
  STATIC_ASSERT_FALSE(is_movable_v<int&>);
  STATIC_ASSERT_FALSE(is_movable_v<int const&>);
  STATIC_ASSERT_FALSE(is_movable_v<int&&>);
  STATIC_ASSERT_FALSE(is_movable_v<int const&&>);

  STATIC_ASSERT_FALSE(is_movable_v<move_only&>);
  STATIC_ASSERT_FALSE(is_movable_v<move_only const&>);
  STATIC_ASSERT_FALSE(is_movable_v<move_only&&>);
  STATIC_ASSERT_FALSE(is_movable_v<move_only const&&>);
}

/// @test `is_movable_v` returns `true` for pointer types (except const pointers)
TEST(MovableTest, PointerTypesAreTrue) {
  STATIC_ASSERT_TRUE(is_movable_v<int*>);
  STATIC_ASSERT_TRUE(is_movable_v<int const*>);
  STATIC_ASSERT_FALSE(is_movable_v<int* const>);        // const pointer is not move-assignable
  STATIC_ASSERT_FALSE(is_movable_v<int const* const>);  // const pointer is not move-assignable

  STATIC_ASSERT_TRUE(is_movable_v<move_only*>);
  STATIC_ASSERT_TRUE(is_movable_v<not_swappable*>);
}

/// @test `is_movable_v` returns `false` for array types (arrays are not movable)
TEST(MovableTest, ArrayTypesAreFalse) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays) - intentionally testing C-style arrays
  STATIC_ASSERT_FALSE(is_movable_v<int[10]>);
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays) - intentionally testing C-style arrays
  STATIC_ASSERT_FALSE(is_movable_v<int[]>);
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays) - intentionally testing C-style arrays
  STATIC_ASSERT_FALSE(is_movable_v<move_only[5]>);
}

/// @test `is_movable_v` returns false for void types
TEST(MovableTest, VoidTypeHandling) {
  static_assert(!is_movable_v<void>, "void is not an object type");
  static_assert(!is_movable_v<void const>, "const void is not an object type");
  static_assert(!is_movable_v<void volatile>, "volatile void is not an object type");
  static_assert(!is_movable_v<void const volatile>, "const volatile void is not an object type");
}

/// @test `is_movable_v` returns `false` for function types (functions are not object types)
TEST(MovableTest, FunctionTypesAreFalse) {
  STATIC_ASSERT_FALSE(is_movable_v<void()>);
  STATIC_ASSERT_FALSE(is_movable_v<int(int)>);
  STATIC_ASSERT_FALSE(is_movable_v<void(int, float)>);
}

/// @test `is_movable_v` returns `false` for const types
TEST(MovableTest, CVQualifiedTypes) {
  STATIC_ASSERT_TRUE(is_movable_v<int>);
  STATIC_ASSERT_FALSE(is_movable_v<int const>);
  STATIC_ASSERT_TRUE(is_movable_v<int volatile>);
  STATIC_ASSERT_FALSE(is_movable_v<int const volatile>);

  STATIC_ASSERT_TRUE(is_movable_v<move_only>);
  STATIC_ASSERT_FALSE(is_movable_v<move_only const>);
  STATIC_ASSERT_FALSE(is_movable_v<move_only volatile>);
  STATIC_ASSERT_FALSE(is_movable_v<move_only const volatile>);
}

/// @test `is_movable_v` returns `false` for types with ADL swap but no move operations
TEST(MovableTest, ADLSwappableButNotMovable) { STATIC_ASSERT_FALSE(is_movable_v<adl_swappable>); }

}  // namespace

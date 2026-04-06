// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/pointer/detail/type_traits.hpp"

#include <cstddef>
#include <memory>
#include <string>

#include <gtest/gtest.h>

#include "arene/base/testing/gtest.hpp"

namespace {
using ::arene::base::pointer::detail::has_arrow_operator;
using ::arene::base::pointer::detail::has_comparison_to_nullptr;
using ::arene::base::pointer::detail::has_dereference_operator;
using ::arene::base::pointer::detail::has_reset;
using ::arene::base::pointer::detail::is_pointer_like_v;

template <typename T>
struct fancy_pointer {
  auto operator->() const -> T*;
  auto operator*() const -> T&;
  auto operator!=(std::nullptr_t) -> bool;
};

template <typename T>
struct no_arrow_operator {
  auto operator*() const -> T&;
  auto operator!=(std::nullptr_t) -> bool;
};

template <typename T>
struct no_deref_operator {
  auto operator->() const -> T*;
  auto operator!=(std::nullptr_t) -> bool;
};

template <typename T>
struct no_nullptr_comp {
  auto operator->() const -> T*;
  auto operator*() const -> T&;
};

/// @test `has_comparison_to_nullptr` is `true` for types that define `!=` for comparing instances of that type and
/// `nullptr`
TEST(HasComparisonToNullptr, TrueForTypesWithComparisonToNullptr) {
  STATIC_ASSERT_TRUE(has_comparison_to_nullptr<int*>);
  STATIC_ASSERT_TRUE(has_comparison_to_nullptr<int const*>);
  STATIC_ASSERT_TRUE(has_comparison_to_nullptr<std::unique_ptr<int>>);
  STATIC_ASSERT_TRUE(has_comparison_to_nullptr<std::shared_ptr<int>>);
  STATIC_ASSERT_TRUE(has_comparison_to_nullptr<fancy_pointer<int>>);
}

/// @test `has_comparison_to_nullptr` is `false` for types that do not define `!=` for comparing instances of that type
/// and `nullptr`
TEST(HasComparisonToNullptr, FalseForTypesWithoutComparisonToNullptr) {
  STATIC_ASSERT_FALSE(has_comparison_to_nullptr<int>);
  STATIC_ASSERT_FALSE(has_comparison_to_nullptr<int const>);
  STATIC_ASSERT_FALSE(has_comparison_to_nullptr<std::string>);
  STATIC_ASSERT_FALSE(has_comparison_to_nullptr<no_nullptr_comp<int>>);
}

/// @test `has_dereference_operator` is `true` for types that define unary `operator*`
TEST(HasDereferenceOperator, TrueForTypesWithDereferenceOperator) {
  STATIC_ASSERT_TRUE(has_dereference_operator<int*>);
  STATIC_ASSERT_TRUE(has_dereference_operator<int const*>);
  STATIC_ASSERT_TRUE(has_dereference_operator<std::unique_ptr<int>>);
  STATIC_ASSERT_TRUE(has_dereference_operator<std::shared_ptr<int>>);
  STATIC_ASSERT_TRUE(has_dereference_operator<fancy_pointer<int>>);
}

/// @test `has_dereference_operator` is `false` for types that do not define unary `operator*`
TEST(HasDereferenceOperator, FalseForTypesWithoutDereferenceOperator) {
  STATIC_ASSERT_FALSE(has_dereference_operator<int>);
  STATIC_ASSERT_FALSE(has_dereference_operator<int const>);
  STATIC_ASSERT_FALSE(has_dereference_operator<std::string>);
  STATIC_ASSERT_FALSE(has_dereference_operator<no_deref_operator<int>>);
}

/// @test `has_arrow_operator` is `true` for types that define an arrow operator
TEST(HasArrowOperator, TrueForTypesWithArrowOperator) {
  STATIC_ASSERT_TRUE(has_arrow_operator<int*>);
  STATIC_ASSERT_TRUE(has_arrow_operator<int const*>);
  STATIC_ASSERT_TRUE(has_arrow_operator<std::unique_ptr<int>>);
  STATIC_ASSERT_TRUE(has_arrow_operator<std::shared_ptr<int>>);
  STATIC_ASSERT_TRUE(has_arrow_operator<fancy_pointer<int>>);
}

/// @test `has_arrow_operator` is `false` for types that don't define an arrow operator
TEST(HasArrowOperator, FalseForTypesWithoutArrowOperator) {
  STATIC_ASSERT_FALSE(has_arrow_operator<int>);
  STATIC_ASSERT_FALSE(has_arrow_operator<int const>);
  STATIC_ASSERT_FALSE(has_arrow_operator<std::string>);
  STATIC_ASSERT_FALSE(has_arrow_operator<no_arrow_operator<int>>);
}

using pointer_like_types =
    ::testing::Types<int*, int const*, std::shared_ptr<int>, std::unique_ptr<int>, fancy_pointer<int>>;

using not_pointer_like_types =
    ::testing::Types<int, int const, no_arrow_operator<int>, no_deref_operator<int>, no_nullptr_comp<int>>;

template <typename Pointer>
struct IsPointerLikeTest : ::testing::Test {};

template <typename Pointer>
using IsPointerLikeTrueForPointerLikeTypes = IsPointerLikeTest<Pointer>;

TYPED_TEST_SUITE(IsPointerLikeTrueForPointerLikeTypes, pointer_like_types, );

/// @test `is_pointer_like_v` is `true` for raw pointers and smart pointers
/// @tparam TypeParam The pointer type
TYPED_TEST(IsPointerLikeTrueForPointerLikeTypes, IsTrue) { STATIC_ASSERT_TRUE(is_pointer_like_v<TypeParam>); }

template <typename Pointer>
using IsPointerLikeFalseForNotPointerLikeTypes = IsPointerLikeTest<Pointer>;

TYPED_TEST_SUITE(IsPointerLikeFalseForNotPointerLikeTypes, not_pointer_like_types, );

/// @test `is_pointer_like_v` is `false` for non-pointer scalars and classes
/// @tparam TypeParam The non-pointer type
TYPED_TEST(IsPointerLikeFalseForNotPointerLikeTypes, IsFalse) { STATIC_ASSERT_FALSE(is_pointer_like_v<TypeParam>); }

/// @test `has_reset` is true for classes that have a `reset` member function that takes the specified type
TEST(HasReset, IsTrueForTypeWithResetForInput) {
  STATIC_ASSERT_TRUE((has_reset<std::unique_ptr<int const>, int*>));
  STATIC_ASSERT_TRUE((has_reset<std::unique_ptr<int const>, int const*>));

  struct base {};
  struct derived : base {};
  STATIC_ASSERT_TRUE((has_reset<std::unique_ptr<base>, derived*>));
}

/// @test `has_reset` is false for classes that do not have a `reset` member function that takes the specified type
TEST(HasReset, IsFalseForTypeWithResetButNotForInput) {
  STATIC_ASSERT_FALSE((has_reset<std::unique_ptr<int>, int const*>));
  STATIC_ASSERT_FALSE((has_reset<std::unique_ptr<int>, int>));
  STATIC_ASSERT_FALSE((has_reset<std::unique_ptr<int>, std::string>));
}

/// @test `has_reset` is false for raw pointers
TEST(HasReset, IsFalseForTypeWithoutReset) { STATIC_ASSERT_FALSE((has_reset<int*, int*>)); }

}  // namespace

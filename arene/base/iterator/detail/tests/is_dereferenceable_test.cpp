// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/iterator/detail/is_dereferenceable.hpp"

#include <gtest/gtest.h>

#include "arene/base/testing/gtest.hpp"

namespace {

using arene::base::iterator_detail::is_dereferenceable_v;

/// @test Given a type `T` for which the expression `*std::declval<T>()` is not well-formed, then
/// `is_dereferenceable_v<T>` is `false`.
TEST(IsDereferenceableTest, IsFalseIfNotDereferenceable) {
  STATIC_ASSERT_FALSE(is_dereferenceable_v<int&>);
  STATIC_ASSERT_FALSE(is_dereferenceable_v<int const&>);
  STATIC_ASSERT_FALSE(is_dereferenceable_v<int&&>);
  STATIC_ASSERT_FALSE(is_dereferenceable_v<int const&&>);

  struct not_dereferenceable {};
  STATIC_ASSERT_FALSE(is_dereferenceable_v<not_dereferenceable&>);
  STATIC_ASSERT_FALSE(is_dereferenceable_v<not_dereferenceable const&>);
  STATIC_ASSERT_FALSE(is_dereferenceable_v<not_dereferenceable&&>);
  STATIC_ASSERT_FALSE(is_dereferenceable_v<not_dereferenceable const&&>);
}

/// @test Given a type `T` for which the expression `*std::declval<T>()` is well-formed, then
/// `is_dereferenceable_v<T>` is `true`.
TEST(IsDereferenceableTest, IsTrueIfDereferenceable) {
  STATIC_ASSERT_TRUE(is_dereferenceable_v<int*>);
  STATIC_ASSERT_TRUE(is_dereferenceable_v<int const*>);
  STATIC_ASSERT_TRUE(is_dereferenceable_v<int* const>);
  STATIC_ASSERT_TRUE(is_dereferenceable_v<int const* const>);
  struct dereferenceable {
    auto operator*() & -> int&;
    auto operator*() const& -> int const&;
    auto operator*() && -> int&;
    auto operator*() const&& -> int const&;
  };
  STATIC_ASSERT_TRUE(is_dereferenceable_v<dereferenceable&>);
  STATIC_ASSERT_TRUE(is_dereferenceable_v<dereferenceable const&>);
  STATIC_ASSERT_TRUE(is_dereferenceable_v<dereferenceable&&>);
  STATIC_ASSERT_TRUE(is_dereferenceable_v<dereferenceable const&&>);
}

/// @test Given a type `T` for which the expression `*std::declval<T>()` is well-formed and returns a type `U`, and when
/// `U&` is not a well-formed expression, then `is_dereferenceable_v<T>` is `false`.
TEST(IsDereferenceableTest, IsFalseIfDereferencedTypeIsNonReferenceable) {
  struct dereferenceable_non_referenceable {
    auto operator*() & -> void;
    auto operator*() const& -> void;
    auto operator*() && -> void;
    auto operator*() const&& -> void;
  };
  STATIC_ASSERT_FALSE(is_dereferenceable_v<dereferenceable_non_referenceable&>);
  STATIC_ASSERT_FALSE(is_dereferenceable_v<dereferenceable_non_referenceable const&>);
  STATIC_ASSERT_FALSE(is_dereferenceable_v<dereferenceable_non_referenceable&&>);
  STATIC_ASSERT_FALSE(is_dereferenceable_v<dereferenceable_non_referenceable const&&>);
}

}  // namespace

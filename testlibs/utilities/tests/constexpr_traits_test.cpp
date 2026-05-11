// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "testlibs/utilities/constexpr_traits.hpp"

#include <gtest/gtest.h>

#include "arene/base/testing/gtest.hpp"

namespace {

template <class T>
constexpr auto check_is_constexpr_default_constructible() -> bool {
  using ::testing::is_constexpr_default_constructible;
  using ::testing::is_constexpr_default_constructible_v;

  return is_constexpr_default_constructible<T>::value && is_constexpr_default_constructible_v<T>;
}

template <class T>
constexpr auto check_is_constexpr_default_and_copy_constructible() -> bool {
  using ::testing::is_constexpr_default_and_copy_constructible;
  using ::testing::is_constexpr_default_and_copy_constructible_v;

  return is_constexpr_default_and_copy_constructible<T>::value && is_constexpr_default_and_copy_constructible_v<T>;
}

struct constexpr_default_constructible {
  constexpr_default_constructible() = default;
};

struct not_constexpr_default_constructible {
  // NOLINTNEXTLINE(hicpp-use-equals-default)
  not_constexpr_default_constructible() {}
};

struct not_default_constructible {
  explicit not_default_constructible() = delete;
};

using constexpr_default_constructible_types = ::testing::Types<int, double, constexpr_default_constructible>;

template <typename T>
class ConstexprDefaultConstructibleTest : public ::testing::Test {};

TYPED_TEST_SUITE(ConstexprDefaultConstructibleTest, constexpr_default_constructible_types, );

/// @test @c testing::is_constexpr_default_constructible returns true for types that are default constructible in a
/// constant expression
TYPED_TEST(ConstexprDefaultConstructibleTest, IsTrueForConstexprDefaultConstructibleTypes) {
  STATIC_ASSERT_TRUE(check_is_constexpr_default_constructible<TypeParam>());
}

/// @test @c testing::is_constexpr_default_constructible returns false for types that are not default constructible
/// in a constant expression
TEST(ConstexprDefaultConstructibleTest, IsFalseForNotConstexprDefaultConstructibleTypes) {
  STATIC_ASSERT_FALSE(check_is_constexpr_default_constructible<not_default_constructible>());
  STATIC_ASSERT_FALSE(check_is_constexpr_default_constructible<not_constexpr_default_constructible>());
}

template <typename T>
class ConstexprCopyConstructibleTest : public ::testing::Test {};

// NOLINTNEXTLINE(hicpp-special-member-functions)
struct constexpr_default_and_copy_constructible {
  // NOLINTNEXTLINE(hicpp-use-equals-default)
  constexpr_default_and_copy_constructible() = default;
  constexpr_default_and_copy_constructible(constexpr_default_and_copy_constructible const&) = default;
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
struct constexpr_only_copy_constructible {
  explicit constexpr_only_copy_constructible() = delete;
  constexpr_only_copy_constructible(constexpr_only_copy_constructible const&) = default;
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
struct not_constexpr_default_and_copy_constructible {
  // NOLINTNEXTLINE(hicpp-use-equals-default)
  not_constexpr_default_and_copy_constructible() {}
  // NOLINTNEXTLINE(hicpp-use-equals-default)
  not_constexpr_default_and_copy_constructible(not_constexpr_default_and_copy_constructible const&) {}
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
struct constexpr_default_constructible_not_constexpr_copy_constructible {
  constexpr_default_constructible_not_constexpr_copy_constructible() = default;
  // NOLINTNEXTLINE(hicpp-use-equals-default)
  constexpr_default_constructible_not_constexpr_copy_constructible(constexpr_default_constructible_not_constexpr_copy_constructible const&) {
  }
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
struct not_default_or_copy_constructible {
  explicit not_default_or_copy_constructible() = delete;
  not_default_or_copy_constructible(not_default_or_copy_constructible const&) = delete;
};

using constexpr_copy_constructible_types = ::testing::Types<int, double, constexpr_default_and_copy_constructible>;

TYPED_TEST_SUITE(ConstexprCopyConstructibleTest, constexpr_copy_constructible_types, );

/// @test @c testing::is_constexpr_copy_constructible returns true for types that are copy constructible in a
/// constant expression
TYPED_TEST(ConstexprCopyConstructibleTest, IsTrueForConstexprCopyConstructibleTypes) {
  STATIC_ASSERT_TRUE(check_is_constexpr_default_and_copy_constructible<TypeParam>());
}

/// @test @c testing::is_constexpr_copy_constructible returns false for types that are not default and copy
/// constructible in a constant expression
TEST(ConstexprCopyConstructibleTest, IsFalseForNotConstexprCopyConstructibleTypes) {
  STATIC_ASSERT_FALSE(check_is_constexpr_default_and_copy_constructible<not_constexpr_default_constructible>());
  STATIC_ASSERT_FALSE(check_is_constexpr_default_and_copy_constructible<
                      constexpr_default_constructible_not_constexpr_copy_constructible>());

  STATIC_ASSERT_FALSE(check_is_constexpr_default_and_copy_constructible<constexpr_only_copy_constructible>());
  STATIC_ASSERT_FALSE(check_is_constexpr_default_and_copy_constructible<not_default_constructible>());
  STATIC_ASSERT_FALSE(check_is_constexpr_default_and_copy_constructible<not_constexpr_default_and_copy_constructible>()
  );
  STATIC_ASSERT_FALSE(check_is_constexpr_default_and_copy_constructible<not_default_or_copy_constructible>());
}

struct not_constexpr_invocable {
  auto operator()() const { return int{}; }
};

TEST(ConstexprInvocableTest, TestNotInvocable) {
  static_assert(
      !testing::is_constexpr_invocable_v<not_constexpr_invocable>,
      "must be `false` for a nullary invocable default function object"
  );
}

struct constexpr_invocable {
  constexpr auto operator()() const { return int{}; }
};

TEST(ConstexprInvocableTest, TestIsInvocable) {
  static_assert(
      testing::is_constexpr_invocable_v<constexpr_invocable>,
      "must be `true` for a nullary invocable default function object"
  );
}

}  // namespace

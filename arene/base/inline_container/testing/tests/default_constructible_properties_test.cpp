// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/inline_container/testing/default_constructible_properties.hpp"

#include <type_traits>

#include <gtest/gtest.h>

#include "arene/base/testing/gtest.hpp"

namespace {

using ::arene::base::testing::assert_default_constructible_properties;

// NOLINTNEXTLINE(hicpp-special-member-functions)
struct constexpr_default_constructible_and_copyable {
  constexpr_default_constructible_and_copyable() = default;
  constexpr_default_constructible_and_copyable(constexpr_default_constructible_and_copyable const&) = default;
  constexpr auto operator==(constexpr_default_constructible_and_copyable const&) const { return true; }
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
struct constexpr_default_constructible_and_not_copyable {
  constexpr_default_constructible_and_not_copyable() = default;
  constexpr_default_constructible_and_not_copyable(constexpr_default_constructible_and_not_copyable const&) = delete;
  constexpr auto operator==(constexpr_default_constructible_and_not_copyable const&) const { return true; }
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
struct default_constructible_and_copyable {
  // Cannot use default as this constructor should not be constexpr
  // NOLINTNEXTLINE(hicpp-use-equals-default)
  default_constructible_and_copyable() {}
  // Cannot use default as this constructor should not be constexpr
  // NOLINTNEXTLINE(hicpp-use-equals-default)
  default_constructible_and_copyable(default_constructible_and_copyable const&) {}
  auto operator==(default_constructible_and_copyable const&) const { return true; }
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
struct default_constructible_and_not_copyable {
  // Cannot use default as this constructor should not be constexpr
  // NOLINTNEXTLINE(hicpp-use-equals-default)
  default_constructible_and_not_copyable() {}
  default_constructible_and_not_copyable(default_constructible_and_not_copyable const&) = delete;
  auto operator==(default_constructible_and_not_copyable const&) const { return true; }
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
struct not_default_constructible_and_not_copyable {
  not_default_constructible_and_not_copyable() = delete;
  not_default_constructible_and_not_copyable(default_constructible_and_not_copyable const&) = delete;
  auto operator==(not_default_constructible_and_not_copyable const&) const { return true; }
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
struct default_constructible_and_copyable_not_equal {
  // Cannot use default as this constructor should not be constexpr
  // NOLINTNEXTLINE(hicpp-use-equals-default)
  default_constructible_and_copyable_not_equal() {}
  // Cannot use default as this constructor should not be constexpr
  // NOLINTNEXTLINE(hicpp-use-equals-default)
  default_constructible_and_copyable_not_equal(default_constructible_and_copyable_not_equal const&) {}
  auto operator==(default_constructible_and_copyable_not_equal const&) const { return false; }
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
struct default_constructible_and_copyable_not_equal_after_copy {
  // Cannot use default as this constructor should not be constexpr
  // NOLINTNEXTLINE(hicpp-use-equals-default)
  default_constructible_and_copyable_not_equal_after_copy() {}
  default_constructible_and_copyable_not_equal_after_copy(
      default_constructible_and_copyable_not_equal_after_copy const& rhs
  )
      : value{rhs.value + 1} {}

  auto operator==(default_constructible_and_copyable_not_equal_after_copy const& rhs) const {
    return value == rhs.value;
  }

  int value{};
};

/// @test Assertion passes for types that are default and copy constructible in a constant expression.
CONSTEXPR_TEST(DefaultConstructibleProperties, PassesForDefaultConstructibleAndCopyableInConstexpr) {
  assert_default_constructible_properties<int>();
  assert_default_constructible_properties<constexpr_default_constructible_and_copyable>();
}

/// @test Assertion passes for types that are default constructible and not copy constructible in a constant expression.
CONSTEXPR_TEST(DefaultConstructibleProperties, PassesForDefaultConstructibleAndNotCopyableInConstexpr) {
  assert_default_constructible_properties<constexpr_default_constructible_and_not_copyable>();
}

/// @test Assertion passes for types that are default and copy constructible but not in a constant expression.
TEST(DefaultConstructibleProperties, PassesForDefaultConstructibleAndCopyableNotInConstexpr) {
  assert_default_constructible_properties<default_constructible_and_copyable>();
}

/// @test Assertion passes for types that are default constructible and not copy constructible but not in a constant
/// expression.
TEST(DefaultConstructibleProperties, PassesForDefaultConstructibleAndNotCopyableNotInConstexpr) {
  assert_default_constructible_properties<default_constructible_and_not_copyable>();
}

/// @test Assertion passes for types that are not default or copy constructible
CONSTEXPR_TEST(DefaultConstructibleProperties, PassesForNotDefaultConstructibleOrCopyable) {
  assert_default_constructible_properties<not_default_constructible_and_not_copyable>();
}

/// @test Assertion fails for types that are default and copy constructible but do not compare equal
TEST(DefaultConstructiblePropertiesDeathTest, AssertsAtRuntimeWhenDefaultConstructedAreNotEqual) {
  ASSERT_DEATH(
      assert_default_constructible_properties<default_constructible_and_copyable_not_equal>(),
      "but two default constructed instances do not compare equal"
  );
}

/// @test Assertion fails for types that are default and copy constructible but do not compare equal after copying
TEST(DefaultConstructiblePropertiesDeathTest, AssertsAtRuntimeWhenCopyingDefaultConstructedIsNotEqual) {
  ASSERT_DEATH(
      assert_default_constructible_properties<default_constructible_and_copyable_not_equal_after_copy>(),
      "but a copy of the default constructed instance does not compare equal to the original"
  );
}

}  // namespace

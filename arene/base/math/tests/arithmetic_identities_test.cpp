// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/math/arithmetic_identities.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

struct custom_type {
  int val{0};
};

struct custom_type_with_specialization {
  int val{-2};
};

/// @brief Program-defined type whose identity values are not usable in a constant expression.
struct non_constexpr_type {
  int val;

  // Non-constexpr constructor forces any specialization to be initialized at runtime.
  explicit non_constexpr_type(int first)
      : val{first} {}
};

/// @brief Program-defined type that is not default-constructible and not constructible from a single @c int.
struct not_default_constructible {
  int val1;
  int val2;

  constexpr not_default_constructible(int first, int second) noexcept
      : val1{first},
        val2{second} {}
};

}  // namespace

template <>
constexpr custom_type_with_specialization arene::base::numbers::zero_v<custom_type_with_specialization>{
    custom_type_with_specialization{0}
};

template <>
constexpr custom_type_with_specialization arene::base::numbers::one_v<custom_type_with_specialization>{
    custom_type_with_specialization{1}
};

// NOLINTBEGIN(fuchsia-statically-constructed-objects)
template <>
non_constexpr_type const arene::base::numbers::zero_v<non_constexpr_type>{0};

template <>
non_constexpr_type const arene::base::numbers::one_v<non_constexpr_type>{1};

// NOLINTEND(fuchsia-statically-constructed-objects)

template <>
constexpr not_default_constructible arene::base::numbers::zero_v<not_default_constructible>{0, 0};

template <>
constexpr not_default_constructible arene::base::numbers::one_v<not_default_constructible>{1, 1};

namespace {

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "Test code: float equal comparison is checking a constant");

/// @test @c zero_v and @c one_v resolve to the expected identity for built-in floating-point types.
TEST(ArithmeticIdentities, BuiltinFloatingPoint) {
  STATIC_ASSERT_EQ(arene::base::numbers::zero_v<float>, float{0});
  STATIC_ASSERT_EQ(arene::base::numbers::zero_v<double>, double{0});
  STATIC_ASSERT_EQ(arene::base::numbers::one_v<float>, float{1});
  STATIC_ASSERT_EQ(arene::base::numbers::one_v<double>, double{1});
}

/// @test @c zero_v and @c one_v resolve to the expected identity for built-in types.
TEST(ArithmeticIdentities, BuiltinTypes) {
  STATIC_ASSERT_EQ(arene::base::numbers::zero_v<int>, int{0});
  STATIC_ASSERT_EQ(arene::base::numbers::one_v<int>, int{1});

  STATIC_ASSERT_EQ(arene::base::numbers::zero_v<double>, double{0.});
  STATIC_ASSERT_EQ(arene::base::numbers::one_v<double>, double{1.});
}

/// @test The default @c zero_v and @c one_v use @c T{} and @c T{1} for user defined types.
TEST(ArithmeticIdentities, DefaultCustomType) {
  STATIC_ASSERT_EQ(arene::base::numbers::zero_v<custom_type>.val, int{0});
  STATIC_ASSERT_EQ(arene::base::numbers::one_v<custom_type>.val, int{1});
}

/// @test Users can override @c zero_v and @c one_v for user defined types with @c constexpr specializations.
TEST(ArithmeticIdentities, CustomSpecialization) {
  STATIC_ASSERT_EQ(custom_type_with_specialization{}.val, int{-2});
  STATIC_ASSERT_EQ(arene::base::numbers::zero_v<custom_type_with_specialization>.val, int{0});
  STATIC_ASSERT_EQ(arene::base::numbers::one_v<custom_type_with_specialization>.val, int{1});
}

/// @test @c zero_v and @c one_v may be specialized for types whose value is not usable in a constant expression.
TEST(ArithmeticIdentities, NonConstexprSpecialization) {
  EXPECT_EQ(arene::base::numbers::zero_v<non_constexpr_type>.val, 0);
  EXPECT_EQ(arene::base::numbers::one_v<non_constexpr_type>.val, 1);
}

/// @test @c zero_v and @c one_v may be specialized for types that are not default-constructible.
TEST(ArithmeticIdentities, NotDefaultConstructibleSpecialization) {
  STATIC_ASSERT_EQ(arene::base::numbers::zero_v<not_default_constructible>.val1, int{0});
  STATIC_ASSERT_EQ(arene::base::numbers::zero_v<not_default_constructible>.val2, int{0});
  STATIC_ASSERT_EQ(arene::base::numbers::one_v<not_default_constructible>.val1, int{1});
  STATIC_ASSERT_EQ(arene::base::numbers::one_v<not_default_constructible>.val2, int{1});
}

ARENE_IGNORE_END();

}  // namespace

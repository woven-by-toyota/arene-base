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

/// @cond
template <>
struct arene::base::numbers::zero<custom_type_with_specialization> {
  static constexpr custom_type_with_specialization value{custom_type_with_specialization{0}};
};

template <>
struct arene::base::numbers::one<custom_type_with_specialization> {
  static constexpr custom_type_with_specialization value{custom_type_with_specialization{1}};
};

// Separate definitions required for ODR use by the zero_v / one_v reference accessors.
constexpr custom_type_with_specialization arene::base::numbers::zero<custom_type_with_specialization>::value;
constexpr custom_type_with_specialization arene::base::numbers::one<custom_type_with_specialization>::value;

// A type whose identity value is not usable in a constant expression specializes the struct with a non-constexpr value.
template <>
struct arene::base::numbers::zero<non_constexpr_type> {
  static non_constexpr_type const value;
};

template <>
struct arene::base::numbers::one<non_constexpr_type> {
  static non_constexpr_type const value;
};

// NOLINTBEGIN(fuchsia-statically-constructed-objects)
non_constexpr_type const arene::base::numbers::zero<non_constexpr_type>::value{0};
non_constexpr_type const arene::base::numbers::one<non_constexpr_type>::value{1};
// NOLINTEND(fuchsia-statically-constructed-objects)

template <>
struct arene::base::numbers::zero<not_default_constructible> {
  static constexpr not_default_constructible value{0, 0};
};

template <>
struct arene::base::numbers::one<not_default_constructible> {
  static constexpr not_default_constructible value{1, 1};
};

// Separate definitions required for ODR use by the zero_v / one_v reference accessors.
constexpr not_default_constructible arene::base::numbers::zero<not_default_constructible>::value;
constexpr not_default_constructible arene::base::numbers::one<not_default_constructible>::value;
/// @endcond

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

/// @test @c zero and @c one may be specialized for types whose value is not usable in a constant expression. The
/// @c zero_v / @c one_v accessors bind a reference to the struct's @c value member, so such values can be read through
/// the accessors at run time even though they are not usable in a constant expression.
TEST(ArithmeticIdentities, NonConstexprSpecialization) {
  EXPECT_EQ(arene::base::numbers::zero_v<non_constexpr_type>.val, 0);
  EXPECT_EQ(arene::base::numbers::one_v<non_constexpr_type>.val, 1);

  // The accessor binds a reference to the struct's value member; it is not a copy.
  EXPECT_EQ(&arene::base::numbers::zero_v<non_constexpr_type>, &arene::base::numbers::zero<non_constexpr_type>::value);
  EXPECT_EQ(&arene::base::numbers::one_v<non_constexpr_type>, &arene::base::numbers::one<non_constexpr_type>::value);
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

// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_traits/has_overloaded_address_operator.hpp"

#include <string>
#include <tuple>
#include <vector>

#include <gtest/gtest.h>

namespace {
using arene::base::has_overloaded_address_operator;
using arene::base::has_overloaded_address_operator_v;

template <typename T>
class HasOverloadedAddressOperatorBuiltinTypes : public testing::Test {};

// NOLINTBEGIN(google-runtime-int) explicitly want builtins.
using builtin_types = ::testing::Types<
    char,
    signed char,
    unsigned char,
    short,
    unsigned short,
    int,
    unsigned int,
    long,
    unsigned long,
    long long,
    unsigned long long,
    float,
    double,
    void*>;
// NOLINTEND(google-runtime-int) explicitly want builtins.

TYPED_TEST_SUITE(HasOverloadedAddressOperatorBuiltinTypes, builtin_types, );

/// @test Built-in types do not have overloaded address operators
/// @tparam TypeParam The type to check
TYPED_TEST(HasOverloadedAddressOperatorBuiltinTypes, BuiltinTypesDoNotHaveOverloadedAddressOperators) {
  static_assert(
      !has_overloaded_address_operator_v<TypeParam>,
      "Builtin type does not have overloaded address operator"
  );
  static_assert(
      !has_overloaded_address_operator<TypeParam>::value,
      "Builtin type does not have overloaded address operator"
  );
}

struct class_with_overloaded_address_of_with_non_standard_signature {
  // NOLINTNEXTLINE(google-runtime-operator)
  auto operator&() -> int { return 0; }
};

struct class_with_overloaded_address_of_with_non_standard_signatures_for_const_and_non_const {
  // NOLINTNEXTLINE(google-runtime-operator)
  auto operator&() const -> int { return 0; }
  // NOLINTNEXTLINE(google-runtime-operator)
  auto operator&() -> int { return 0; }
};

struct class_with_overloaded_address_of_for_non_const_only {
  // NOLINTNEXTLINE(google-runtime-operator)
  auto operator&() -> class_with_overloaded_address_of_for_non_const_only* { return nullptr; }
};

struct class_with_overloaded_address_of_for_const_only {
  // NOLINTNEXTLINE(google-runtime-operator)
  auto operator&() const -> class_with_overloaded_address_of_for_const_only const* { return nullptr; }
};

struct class_with_overloaded_address_of_with_expected_signature {
  // NOLINTNEXTLINE(google-runtime-operator)
  auto operator&() -> class_with_overloaded_address_of_with_expected_signature* { return nullptr; }
  // NOLINTNEXTLINE(google-runtime-operator)
  auto operator&() const -> class_with_overloaded_address_of_with_expected_signature const* { return nullptr; }
};

struct derived_overloaded_address : class_with_overloaded_address_of_with_expected_signature {};

template <typename T>
class HasOverloadedAddressOperatorTypesWithOverloads : public testing::Test {};

using types_with_overloads = ::testing::Types<
    class_with_overloaded_address_of_for_const_only,
    class_with_overloaded_address_of_for_non_const_only,
    class_with_overloaded_address_of_with_expected_signature,
    class_with_overloaded_address_of_with_non_standard_signature,
    class_with_overloaded_address_of_with_non_standard_signatures_for_const_and_non_const,
    derived_overloaded_address>;
TYPED_TEST_SUITE(HasOverloadedAddressOperatorTypesWithOverloads, types_with_overloads, );

/// @test `has_overloaded_address_operator_v` evaluates to `true` for types that do have an overloaded address operator
/// @tparam TypeParam The type to check
TYPED_TEST(HasOverloadedAddressOperatorTypesWithOverloads, TypesWithOverloadedAddressOperatorAreIndicatedAsSuch) {
  static_assert(has_overloaded_address_operator_v<TypeParam>, "Type has overloaded address operator");
  static_assert(has_overloaded_address_operator<TypeParam>::value, "Type has overloaded address operator");
}

struct plain_struct {
  int data;
};

template <typename T>
class HasOverloadedAddressOperatorTypesWithoutOverloads : public testing::Test {};

using types_without_overloads = ::testing::Types<plain_struct, std::string, std::vector<int>, std::tuple<int, double>>;
TYPED_TEST_SUITE(HasOverloadedAddressOperatorTypesWithoutOverloads, types_without_overloads, );

/// @test `has_overloaded_address_operator_v` evaluates to `false` for types that do not have an overloaded address
/// operator
/// @tparam TypeParam The type to check
TYPED_TEST(HasOverloadedAddressOperatorTypesWithoutOverloads, TypesWithoutOverloadedAddressOperatorAreIndicatedAsSuch) {
  static_assert(!has_overloaded_address_operator_v<TypeParam>, "Type does not have overloaded address operator");
  static_assert(!has_overloaded_address_operator<TypeParam>::value, "Type does not have overloaded address operator");
}
}  // namespace

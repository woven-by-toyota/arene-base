// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "testlibs/minitest/minitest.hpp"

struct some_class {
  double dummy;
};
class some_other_class {};

struct some_other_large_class {
  double dummy;
};

using types_for_testing = ::testing::Types<char, double, some_class, some_other_class, some_other_large_class>;

template <typename T>
class CondTypedTestTests : public testing::Test {};

TYPED_TEST_SUITE(CondTypedTestTests, types_for_testing, );

template <typename T>
struct enable_trait {
  static constexpr bool value = false;
};

template <>
struct enable_trait<char> {
  static constexpr bool value = true;
};

template <>
struct enable_trait<some_other_class> {
  static constexpr bool value = true;
};

template <>
struct enable_trait<some_other_large_class> {
  static constexpr bool value = true;
};

CONDITIONAL_TYPED_TEST(CondTypedTestTests, TestOne, enable_trait<TypeParam>::value) {
  ASSERT_TRUE(sizeof(TypeParam) == 1);
}
